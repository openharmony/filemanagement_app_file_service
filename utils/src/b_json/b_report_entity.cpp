/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "b_json/b_report_entity.h"

#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "sandbox_helper.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
namespace {
const char ATTR_SEP = ';';
const char LINE_SEP = '\n';
const char LINE_WRAP = '\r';
const int64_t HASH_BUFFER_SIZE = 4096; // 每次读取的size
const int INFO_ALIGN_NUM = 2;
const size_t ENCODE_FLAG_VERSION = 7; // 7: "path", "mode", "dir", "size", "mtime", "hash", "isIncremental"
const std::string DEFAULT_VALUE = "1";
const std::vector<std::string> Data_Header = {
    "path", "mode", "dir", "size", "mtime", "hash", "isIncremental", "encodeFlag"
};

} // namespace

static void SplitStringByChar(const string &str, const char &sep, vector<string>& splits)
{
    string newStr = str;
    if (str.empty()) {
        return;
    }
    if (str.rfind(sep) == str.size() - 1) {
        newStr += sep;
    }
    stringstream ss(newStr);
    string res;
    while (getline(ss, res, sep)) {
        splits.emplace_back(res);
    }
}

static ErrCode ParseReportInfo(struct ReportFileInfo &fileStat,
                               const vector<string> &splits,
                               const size_t keyLen)
{
    // 根据数据拼接结构体
    // 处理path路径
    try {
        // 识别path字段与其他字段
        size_t dataLen = splits.size();
        if (dataLen != keyLen || dataLen < ENCODE_FLAG_VERSION) {
            HILOGE("Error data size");
            return EPERM;
        }
        string path = splits[static_cast<size_t>(KeyType::PATH)] + ATTR_SEP;
        if (dataLen == ENCODE_FLAG_VERSION) {
            fileStat.encodeFlag = false;
        } else {
            fileStat.encodeFlag = splits[static_cast<size_t>(KeyType::ENCODE_FLAG)] == DEFAULT_VALUE;
        }
        path = (path.length() > 0 && path[0] == '/') ? path.substr(1, path.length() - 1) : path;
        auto fileRawPath = (path.length() > 0) ? path.substr(0, path.length() - 1) : path;
        fileStat.filePath = BReportEntity::DecodeReportItem(fileRawPath, fileStat.encodeFlag);
        HILOGD("Briefings file %{public}s", fileStat.filePath.c_str());
        fileStat.mode = splits[static_cast<size_t>(KeyType::MODE)];
        fileStat.isDir = splits[static_cast<size_t>(KeyType::DIR)] == DEFAULT_VALUE;

        stringstream sizeStream(splits[static_cast<size_t>(KeyType::SIZE)]);
        off_t size = 0;
        if (!(sizeStream >> size)) {
            HILOGE("Transfer size err");
        }
        fileStat.size = size;

        stringstream mtimeStream(splits[static_cast<size_t>(KeyType::MTIME)]);
        off_t mtime = 0;
        if (!(mtimeStream >> mtime)) {
            HILOGE("Transfer mtime err");
        }
        fileStat.mtime = mtime;
        fileStat.hash = splits[static_cast<size_t>(KeyType::HASH)];
        fileStat.isIncremental = splits[static_cast<size_t>(KeyType::IS_INCREMENTAL)] == DEFAULT_VALUE;
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to ParseReportInfo");
        return EPERM;
    }
}

static void DealLine(vector<std::string> &keys,
                     int &num,
                     const string &line,
                     unordered_map<string, struct ReportFileInfo> &infos)
{
    if (line.empty()) {
        return;
    }

    string currentLine = line;
    if (currentLine[currentLine.length() - 1] == LINE_WRAP) {
        currentLine.pop_back();
    }
    vector<string> splits;
    SplitStringByChar(currentLine, ATTR_SEP, splits);
    if (num < INFO_ALIGN_NUM) {
        if (num == 1) {
            keys = splits;
            if (keys != Data_Header) {
                HILOGE("File halder check err");
            }
        }
        num++;
    } else {
        struct ReportFileInfo fileState;
        auto code = ParseReportInfo(fileState, splits, keys.size());
        if (code != ERR_OK) {
            HILOGE("ParseReportInfo err:%{public}d, %{public}s", code, currentLine.c_str());
        } else {
            infos.try_emplace(fileState.filePath, fileState);
        }
    }
}

void BReportEntity::GetReportInfos(unordered_map<string, struct ReportFileInfo> &infos) const
{
    char buffer[HASH_BUFFER_SIZE];
    ssize_t bytesRead;
    string currentLine;
    vector<std::string> keys;

    int num = 0;
    while ((bytesRead = read(srcFile_, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            if (buffer[i] == LINE_SEP) {
                DealLine(keys, num, currentLine, infos);
                currentLine.clear();
            } else {
                currentLine += buffer[i];
            }
        }
    }

    // 处理文件中的最后一行
    if (!currentLine.empty()) {
        DealLine(keys, num, currentLine, infos);
    }
}

bool BReportEntity::GetStorageReportInfos(std::unordered_map<string, struct ReportFileInfo> &infos)
{
    char buffer[HASH_BUFFER_SIZE];
    ssize_t bytesRead = 0;

    if ((bytesRead = read(srcFile_, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            if (buffer[i] == LINE_SEP) {
                DealLine(keys_, currLineNum_, currLineInfo_, infos);
                currLineInfo_.clear();
            } else {
                currLineInfo_ += buffer[i];
            }
        }
    } else {
        if (currLineInfo_.empty()) {
            return false;
        }
        DealLine(keys_, currLineNum_, currLineInfo_, infos);
        currLineInfo_.clear();
    }
    return true;
}

void BReportEntity::CheckAndUpdateIfReportLineEncoded(std::string &path)
{
    if (path.empty()) {
        return;
    }

    unordered_map<string, struct ReportFileInfo> infos;
    GetReportInfos(infos);
    constexpr int BIG_FILE_REPORT_INFO_NUM = 1;
    if (infos.size() == BIG_FILE_REPORT_INFO_NUM) {
        auto info = infos.begin();
        path = info->first;
    } else {
        HILOGE("Invalid item sizes in report, current is %{public}zu", infos.size());
    }
}

string BReportEntity::EncodeReportItem(const string &reportItem, bool enableEncode)
{
    string encodeItem;
    if (enableEncode) {
        encodeItem = AppFileService::SandboxHelper::Encode(reportItem);
    } else {
        encodeItem = reportItem;
    }
    return encodeItem;
}

string BReportEntity::DecodeReportItem(const string &reportItem, bool enableEncode)
{
    string decodeItem;
    if (enableEncode) {
        decodeItem = AppFileService::SandboxHelper::Decode(reportItem);
    } else {
        decodeItem = reportItem;
    }
    return decodeItem;
}
} // namespace OHOS::FileManagement::Backup