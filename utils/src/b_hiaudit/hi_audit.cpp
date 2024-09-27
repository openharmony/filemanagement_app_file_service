/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "b_hiaudit/hi_audit.h"

#include <chrono>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <iomanip>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>

#include "filemgmt_libhilog.h"
#include "b_hiaudit/zip_util.h"
#include "b_hiaudit/hi_audit.h"

namespace OHOS::FileManagement::Backup {
namespace {
const HiAuditConfig HIAUDIT_CONFIG = { "/data/log/hiaudit/app_file_service/", "appfileservice",
    2 * 1024, 3 * 1204 * 1024, 10 };
const HiAuditConfig HIAUDIT_CONFIG_EXT = { "/data/storage/el2/log/hiaudit/", "appfileservice",
    2 * 1024, 3 * 1204 * 1024, 10 };
constexpr int8_t MILLISECONDS_LENGTH = 3;
constexpr int64_t SEC_TO_MILLISEC = 1000;
constexpr int MAX_TIME_BUFF = 64; // 64 : for example 2021-05-27-01-01-01
const std::string HIAUDIT_LOG_NAME = HIAUDIT_CONFIG.logPath + HIAUDIT_CONFIG.logName + "_audit.csv";
const std::string HIAUDIT_LOG_NAME_EXT = HIAUDIT_CONFIG_EXT.logPath + HIAUDIT_CONFIG_EXT.logName + "_audit.csv";
}

HiAudit::HiAudit(bool isSaJob)
{
    isSaJob_ = isSaJob;
    Init();
}

HiAudit::~HiAudit()
{
    close(writeFd_);
}

HiAudit &HiAudit::GetInstance(bool isSaJob)
{
    static HiAudit hiAudit(isSaJob);
    return hiAudit;
}

void HiAudit::Init()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (isSaJob_) {
        hiAuditConfig_ = HIAUDIT_CONFIG;
    } else {
        hiAuditConfig_ = HIAUDIT_CONFIG_EXT;
    }
    if (!MkLogDirSuccess()) {
        HILOGE("Init, Create log dir failed");
        return;
    }
    std::string logFilePath = hiAuditConfig_.logPath + hiAuditConfig_.logName + "_audit.csv";
    writeFd_ =
        open(logFilePath.c_str(), O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (writeFd_ < 0) {
        HILOGE("Init, writeFd_ open error, logFilePath is:%{public}s", logFilePath.c_str());
    }
    struct stat st;
    writeLogSize_ = stat(HIAUDIT_LOG_NAME.c_str(), &st) ? 0 : static_cast<uint64_t>(st.st_size);
    HILOGI("Init, writeLogSize: %{public}u", writeLogSize_.load());
}

uint64_t HiAudit::GetMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

std::string HiAudit::GetFormattedTimestamp(time_t timeStamp, const std::string &format)
{
    auto seconds = timeStamp / SEC_TO_MILLISEC;
    char date[MAX_TIME_BUFF] = {0};
    struct tm result {};
    if (localtime_r(&seconds, &result) != nullptr) {
        strftime(date, MAX_TIME_BUFF, format.c_str(), &result);
    }
    return std::string(date);
}

std::string HiAudit::GetFormattedTimestampEndWithMilli()
{
    uint64_t milliSeconds = GetMilliseconds();
    std::string formattedTimeStamp = GetFormattedTimestamp(milliSeconds, "%Y%m%d%H%M%S");
    std::stringstream ss;
    ss << formattedTimeStamp;
    milliSeconds = milliSeconds % SEC_TO_MILLISEC;
    ss << std::setfill('0') << std::setw(MILLISECONDS_LENGTH) << milliSeconds;
    return ss.str();
}

void HiAudit::Write(const AuditLog &auditLog)
{
    HILOGI("write");
    std::lock_guard<std::mutex> lock(mutex_);
    if (writeLogSize_ == 0) {
        WriteToFile(auditLog.TitleString());
    }
    std::string writeLog =
        GetFormattedTimestampEndWithMilli() + ", " + hiAuditConfig_.logName + ", NO, " + auditLog.ToString();
    HILOGE("write %{public}s.", writeLog.c_str());
    if (writeLog.length() > hiAuditConfig_.logSize) {
        writeLog = writeLog.substr(0, hiAuditConfig_.logSize);
    }
    writeLog = writeLog + "\n";
    WriteToFile(writeLog);
}

void HiAudit::GetWriteFilePath()
{
    if (writeLogSize_ < hiAuditConfig_.fileSize) {
        return;
    }
    close(writeFd_);
    ZipAuditLog();
    CleanOldAuditFile();
    if (!MkLogDirSuccess()) {
        HILOGE("Create log dir failed");
        return;
    }
    std::string logFilePath = hiAuditConfig_.logPath + hiAuditConfig_.logName + "_audit.csv";
    writeFd_ =
        open(logFilePath.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    writeLogSize_ = 0;
}

void HiAudit::CleanOldAuditFile()
{
    uint32_t zipFileSize = 0;
    std::string oldestAuditFile;
    if (!MkLogDirSuccess()) {
        HILOGE("Create log dir failed");
        return;
    }
    DIR *dir = opendir(hiAuditConfig_.logPath.c_str());
    if (dir == NULL) {
        HILOGE("open dir error, errno:%{public}d", errno);
        return;
    }
    struct dirent* ptr = nullptr;
    while ((ptr = readdir(dir)) != nullptr) {
        if (std::string(ptr->d_name).find(hiAuditConfig_.logName) != std::string::npos &&
            std::string(ptr->d_name).find("zip") != std::string::npos) {
            zipFileSize = zipFileSize + 1;
            if (oldestAuditFile.empty()) {
                oldestAuditFile = hiAuditConfig_.logPath + std::string(ptr->d_name);
                continue;
            }
            struct stat st;
            stat((hiAuditConfig_.logPath + std::string(ptr->d_name)).c_str(), &st);
            struct stat oldestSt;
            stat(oldestAuditFile.c_str(), &oldestSt);
            if (st.st_mtime < oldestSt.st_mtime) {
                oldestAuditFile = HIAUDIT_CONFIG.logPath + std::string(ptr->d_name);
            }
        }
    }
    closedir(dir);
    if (zipFileSize > hiAuditConfig_.fileCount) {
        remove(oldestAuditFile.c_str());
    }
}

void HiAudit::WriteToFile(const std::string &content)
{
    GetWriteFilePath();
    if (writeFd_ > 0) {
        write(writeFd_, content.c_str(), content.length());
    } else {
        HILOGE("write error.");
    }
    writeLogSize_ = writeLogSize_ + content.length();
}

void HiAudit::ZipAuditLog()
{
    if (!MkLogDirSuccess()) {
        HILOGE("Create log dir failed");
        return;
    }
    std::string zipFileName = hiAuditConfig_.logPath + hiAuditConfig_.logName + "_audit_" +
        GetFormattedTimestamp(GetMilliseconds(), "%Y%m%d%H%M%S");
    std::string logFilePath = hiAuditConfig_.logPath + hiAuditConfig_.logName + "_audit.csv";
    std::rename(logFilePath.c_str(), (zipFileName + ".csv").c_str());
    zipFile compressZip = ZipUtil::CreateZipFile(zipFileName + ".zip");
    if (compressZip == nullptr) {
        HILOGW("open zip file failed.");
        return;
    }
    if (ZipUtil::AddFileInZip(compressZip, zipFileName + ".csv", KEEP_NONE_PARENT_PATH) ==
        0) {
        remove((zipFileName + ".csv").c_str());
    }
    ZipUtil::CloseZipFile(compressZip);
}

bool HiAudit::MkLogDirSuccess()
{
    if (access(hiAuditConfig_.logPath.data(), F_OK) != 0) {
        int ret = mkdir(hiAuditConfig_.logPath.data(), S_IRWXU | S_IRWXG);
        if (ret != 0 || errno != 0 || errno != EEXIST) {
            if (access(hiAuditConfig_.logPath.data(), F_OK) != 0) {
                HILOGE("create log dir failed");
                return false;
            }
        }
    }
    return true;
}
} // namespace OHOS::FileManagement::Backup