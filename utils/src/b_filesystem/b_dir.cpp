/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "b_filesystem/b_dir.h"

#include <algorithm>
#include <cinttypes>
#include <dirent.h>
#include <fnmatch.h>
#include <functional>
#include <filesystem>
#include <glob.h>
#include <memory>
#include <set>
#include <string>
#include <stack>
#include <tuple>
#include <vector>

#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_filesystem/b_file_hash.h"
#include "b_resources/b_constants.h"
#include "b_utils/string_utils.h"
#include "directory_ex.h"
#include "errors.h"
#include "filemgmt_libhilog.h"
#include "sandbox_helper.h"
#include "b_utils/scan_file_singleton.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const int32_t PATH_MAX_LEN = 4096;
const size_t TOP_ELE = 0;
const std::string APP_DATA_DIR = BConstants::PATH_PUBLIC_HOME +
    BConstants::PATH_APP_DATA + BConstants::FILE_SEPARATOR_CHAR;

static bool IsEmptyDirectory(const string &path)
{
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        HILOGE("Opendir failed, errno:%{public}d", errno);
        return false;
    }
    bool isEmpty = true;
    struct dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type != DT_DIR || (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)) {
            isEmpty = false;
            break;
        }
    }
    closedir(dir);
    return isEmpty;
}

static uint32_t CheckOverLongPath(const string &path)
{
    uint32_t len = path.length();
    if (len >= PATH_MAX_LEN) {
        size_t found = path.find_last_of(BConstants::FILE_SEPARATOR_CHAR);
        string sub = "";
        if (found != std::string::npos && found != path.length() - 1) {
            sub = path.substr(found + 1);
        }
        HILOGE("Path over long, length:%{public}d, fileName:%{private}s.", len, sub.c_str());
    }
    return len;
}

static void ProcessFile(const ProcessInfo& info, int64_t& bigFileSize, int64_t& smallFileSize,
    const std::vector<std::string> &excludes)
{
    if (info.restorePath_.empty() && BDir::IsDirsMatch(excludes, info.backupPath_)) {
        return;
    }
    struct stat sta = {};
    if (CheckOverLongPath(info.backupPath_) >= PATH_MAX_LEN) {
        return;
    }
    if (stat(info.backupPath_.data(), &sta) == -1) {
        HILOGE("stat file fail, errno=%{public}d", errno);
        return;
    }
    if (sta.st_size <= info.sizeBoundary_) {
        ScanFileSingleton::GetInstance().AddSmallFile(info.backupPath_, sta.st_size, info.restorePath_);
        smallFileSize += sta.st_size;
    } else {
        ScanFileSingleton::GetInstance().AddBigFile(info.backupPath_, sta, info.restorePath_);
        bigFileSize += sta.st_size;
    }
}

static tuple<ErrCode, int64_t, int64_t> ProcessSingleFile(const vector<string> &excludes, const string &backupPath,
    const string& restorePath, off_t sizeBoundary = -1)
{
    if (backupPath == "/") {
        return {ERR_OK, 0, 0};
    }
    int64_t bigFileSize = 0;
    int64_t smallFileSize = 0;
    ProcessFile({backupPath, restorePath, sizeBoundary}, bigFileSize, smallFileSize, excludes);
    return {ERR_OK, bigFileSize, smallFileSize};
}

tuple<ErrCode, int64_t, int64_t> DirScanner::ScanDir(const string &backupPath, const vector<string> &excludes,
    off_t size)
{
    HILOGD("scan dir, path: %{public}s", GetAnonyPath(backupPath).c_str());
    if (!filesystem::is_directory(backupPath)) {
        HILOGE("Invalid directory path: %{private}s", backupPath.c_str());
        return ProcessSingleFile(excludes, backupPath, "", size);
    }
    int64_t bigFileSize = 0;
    int64_t smallFileSize = 0;
    stack<string> dirStack;
    dirStack.push(backupPath);
    while (!dirStack.empty()) {
        auto currentPath = dirStack.top();
        dirStack.pop();
        if (BDir::IsDirsMatch(excludes, currentPath)) {
            continue;
        }
        if (IsEmptyDirectory(currentPath)) {
            ScanFileSingleton::GetInstance().AddSmallFile(StringUtils::PathAddDelimiter(currentPath), 0);
            continue;
        }
        unique_ptr<DIR, function<void(DIR *)>> dir = {opendir(currentPath.c_str()), closedir};
        if (dir == nullptr) {
            HILOGE("openDir fail, path:%{public}s, errno:%{public}d", GetAnonyPath(currentPath).c_str(), errno);
            continue;
        }
        struct dirent *ptr = nullptr;
        while (!!(ptr = readdir(dir.get()))) {
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
                continue;
            }
            std::string filePath = StringUtils::PathAddDelimiter(currentPath) + string(ptr->d_name);
            if (ptr->d_type == DT_REG) {
                ProcessFile({filePath, "", size}, bigFileSize, smallFileSize, excludes);
            } else if (ptr->d_type == DT_DIR) {
                dirStack.push(filePath);
            } else {
                HILOGE("Not support file type");
            }
        }
    }
    return {ERR_OK, bigFileSize, smallFileSize};
}

tuple<ErrCode, int64_t, int64_t> CompatibleDirScanner::ScanDir(const string &path, const vector<string> &excludes,
    off_t size)
{
    auto [backupPath, restorePath] = StringUtils::ParseMappingDir(path);
    HILOGD("scan dir, path: %{public}s, restore:%{public}s", GetAnonyPath(backupPath).c_str(),
        GetAnonyPath(restorePath).c_str());
    if (!filesystem::is_directory(backupPath)) {
        HILOGE("Invalid directory path: %{private}s", path.c_str());
        return ProcessSingleFile(excludes, backupPath, restorePath, size);
    }
    int64_t bigFileSize = 0;
    int64_t smallFileSize = 0;
    stack<pair<string, string>> dirStack;
    dirStack.push({backupPath, restorePath});
    while (!dirStack.empty()) {
        auto [currentPath, currentRestorePath] = dirStack.top();
        dirStack.pop();
        if (BDir::IsDirsMatch(excludes, currentPath)) {
            continue;
        }
        if (IsEmptyDirectory(currentPath)) {
            ScanFileSingleton::GetInstance()
                .AddSmallFile(StringUtils::PathAddDelimiter(currentPath), 0, currentRestorePath);
            continue;
        }
        unique_ptr<DIR, function<void(DIR *)>> dir = {opendir(currentPath.c_str()), closedir};
        if (dir == nullptr) {
            HILOGE("openDir fail, path:%{public}s, errno:%{public}d", GetAnonyPath(currentPath).c_str(), errno);
            continue;
        }
        struct dirent *ptr = nullptr;
        while (!!(ptr = readdir(dir.get()))) {
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
                continue;
            }
            std::string subBackupPath = StringUtils::PathAddDelimiter(currentPath) + string(ptr->d_name);
            std::string subRestorePath = StringUtils::PathAddDelimiter(currentRestorePath) + string(ptr->d_name);
            if (ptr->d_type == DT_REG) {
                ProcessFile({subBackupPath, subRestorePath, size}, bigFileSize, smallFileSize, excludes);
            } else if (ptr->d_type == DT_DIR) {
                dirStack.push({subBackupPath, subRestorePath});
            } else {
                HILOGE("Not support file type");
            }
        }
    }
    return {ERR_OK, bigFileSize, smallFileSize};
}

tuple<ErrCode, int64_t, int64_t> IDirScanner::ScanAllDirs(const std::set<std::string> &includes,
    const std::vector<std::string> &excludes)
{
    int64_t totalBigFileSize = 0;
    int64_t totalSmallFileSize = 0;
    int64_t start = TimeUtils::GetTimeMS();
    ErrCode finalErrCode = ERR_OK;
    for (const auto &item : includes) {
        auto [errCode, bigFileSize, smallFileSize] = ScanDir(item, excludes, BConstants::BIG_FILE_BOUNDARY);
        if (errCode == 0) {
            HILOGI("big files: %{public}" PRId64 "; small files: %{public}" PRId64 "", bigFileSize, smallFileSize);
            totalBigFileSize += bigFileSize ;
            totalSmallFileSize += smallFileSize;
        } else {
            HILOGE("scan dir fail, err=%{public}d, path=%{public}s", errCode, GetAnonyPath(item).c_str());
            finalErrCode = static_cast<int>(BError::Codes::EXT_SCAN_DIR_FAIL);
        }
    }
    int64_t spendMilli = TimeUtils::GetSpendMS(start);
    HILOGI("scan spend: %{public}" PRId64 ", bigfile: %{public}" PRId64 ", smallFile: %{public}" PRId64 "",
        spendMilli, totalBigFileSize, totalSmallFileSize);
    return {finalErrCode, totalBigFileSize, totalSmallFileSize};
}

std::tuple<ErrCode, int64_t, int64_t> BDir::ScanAllDirs(const std::set<std::string> &includes,
    const std::set<std::string> &compatIncludes, const std::vector<std::string> &excludes)
{
    HILOGI("scan all dirs inlcude:%{public}zu, compatIncludes:%{public}zu, excludes:%{public}zu",
        includes.size(), compatIncludes.size(), excludes.size());
    ErrCode errCode = ERR_OK;
    int64_t bigFileSize = 0;
    int64_t smallFileSize = 0;
    if (!includes.empty()) {
        DirScanner scanner;
        tie(errCode, bigFileSize, smallFileSize) = scanner.ScanAllDirs(includes, excludes);
    }
    if (errCode != ERR_OK) {
        HILOGE("scan normal dirs fail, err=%{public}d", errCode);
    }
    if (!compatIncludes.empty()) {
        CompatibleDirScanner compatScanner;
        auto [compatErrCode, compatBigFileSize, compatSmallFileSize]
            = compatScanner.ScanAllDirs(compatIncludes, excludes);
        if (compatErrCode != ERR_OK) {
            HILOGE("scan compat dirs fail, err=%{public}d", compatErrCode);
        }
        bigFileSize += compatBigFileSize;
        smallFileSize += compatSmallFileSize;
        if (errCode == ERR_OK && compatErrCode != ERR_OK) {
            errCode = compatErrCode;
        }
    }
    return {errCode, bigFileSize, smallFileSize};
}

void BDir::PreDealExcludes(std::vector<std::string> &excludes)
{
    size_t lenEx = excludes.size();
    int j = 0;
    for (size_t i = 0; i < lenEx; ++i) {
        if (!excludes[i].empty()) {
            if (excludes[i].at(excludes[i].size() - 1) == BConstants::FILE_SEPARATOR_CHAR) {
                excludes[i] += "*";
            }
            if (excludes[i].find(BConstants::FILE_SEPARATOR_CHAR) != string::npos &&
                excludes[i].at(TOP_ELE) != BConstants::FILE_SEPARATOR_CHAR) {
                excludes[i] = BConstants::FILE_SEPARATOR_CHAR + excludes[i];
            }
            excludes[j++] = excludes[i];
        }
    }
    excludes.resize(j);
}

tuple<ErrCode, vector<string>> BDir::GetDirFiles(const string &path)
{
    vector<string> files;
    unique_ptr<DIR, function<void(DIR *)>> dir = {opendir(path.c_str()), closedir};
    if (!dir) {
        HILOGE("Invalid directory path: %{private}s", path.c_str());
        return {BError(errno).GetCode(), files};
    }

    struct dirent *ptr = nullptr;
    while (!!(ptr = readdir(dir.get()))) {
        // current dir OR parent dir
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            continue;
        } else if (ptr->d_type == DT_DIR) {
            continue;
        } else {
            files.emplace_back(IncludeTrailingPathDelimiter(path) + string(ptr->d_name));
        }
    }

    return {ERR_OK, files};
}

static std::set<std::string> GetSubDir(const std::string &path)
{
    if (path.empty()) {
        return {};
    }
    std::set<std::string> result;
    unique_ptr<DIR, function<void(DIR *)>> dir = {opendir(path.c_str()), closedir};
    if (!dir) {
        HILOGE("Invalid directory path: %{private}s", path.c_str());
        return {};
    }

    struct dirent *ptr = nullptr;
    while (!!(ptr = readdir(dir.get()))) {
        // current dir OR parent dir
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            continue;
        } else if (ptr->d_type == DT_DIR) {
            std::string tmpPath = IncludeTrailingPathDelimiter(path) +
                string(ptr->d_name) + BConstants::FILE_SEPARATOR_CHAR;
            if (tmpPath == APP_DATA_DIR) {
                HILOGI("Filter appdata successfully");
                continue;
            }
            result.emplace(tmpPath);
        } else {
            result.emplace(IncludeTrailingPathDelimiter(path) + string(ptr->d_name));
        }
    }
    return result;
}

static void RmForceExcludePath(set<string> &expandPath)
{
    set<string> addPaths;
    for (auto it = expandPath.begin(); it != expandPath.end();) {
        if (*it == BConstants::PATH_PUBLIC_HOME) {
            addPaths = GetSubDir(*it);
        }
        if ((*it).find(APP_DATA_DIR) == 0) {
            it = expandPath.erase(it);
            continue;
        }
        ++it;
    }
    if (!addPaths.empty()) {
        expandPath.erase(BConstants::PATH_PUBLIC_HOME);
        expandPath.merge(addPaths);
    }
}

set<string> BDir::ExpandPathWildcard(const vector<string> &vec, bool onlyPath)
{
    unique_ptr<glob_t, function<void(glob_t *)>> gl {new glob_t, [](glob_t *ptr) {
        globfree(ptr);
        delete ptr;
    }};
    *gl = {};

    unsigned int flags = GLOB_DOOFFS | GLOB_MARK;
    for (const string &pattern : vec) {
        if (!pattern.empty()) {
            glob(pattern.data(), static_cast<int>(flags), NULL, gl.get());
            flags |= GLOB_APPEND;
        }
    }

    set<string> expandPath;
    set<string> filteredPath;
    for (size_t i = 0; i < gl->gl_pathc; ++i) {
        std::string tmpPath = gl->gl_pathv[i];
        auto pos = tmpPath.find(BConstants::FILE_SEPARATOR_CHAR);
        if (pos != 0 && pos != std::string::npos) {
            tmpPath = BConstants::FILE_SEPARATOR_CHAR + tmpPath;
        }
        expandPath.emplace(tmpPath);
    }
    RmForceExcludePath(expandPath);
    for (auto it = expandPath.begin(); it != expandPath.end(); ++it) {
        filteredPath.emplace(*it);
        if (onlyPath && *it->rbegin() != BConstants::FILE_SEPARATOR_CHAR) {
            continue;
        }
        auto jt = it;
        for (++jt; jt != expandPath.end() && (jt->find(*it) == 0); ++jt) {
        }

        it = --jt;
    }

    return filteredPath;
}

bool BDir::CheckAndCreateDirectory(const string &filePath)
{
    size_t pos = filePath.rfind('/');
    if (pos == string::npos) {
        return true;
    }

    string folderPath = "/" + filePath.substr(0, pos);
    if (access(folderPath.c_str(), F_OK) != 0) {
        if (!ForceCreateDirectory(folderPath.data())) {
            return false;
        }
    }
    return true;
}

void BDir::GetUser0FileStat(vector<string> bigFile,
                            vector<string> smallFile,
                            vector<struct ReportFileInfo> &allFiles,
                            vector<struct ReportFileInfo> &smallFiles,
                            vector<struct ReportFileInfo> &bigFiles)
{
    for (const auto &item : smallFile) {
        struct ReportFileInfo storageFiles;
        storageFiles.filePath = item;
        if (filesystem::is_directory(item)) {
            storageFiles.isDir = 1;
            storageFiles.userTar = 0;
        } else {
            storageFiles.isDir = 0;
            auto [res, fileHash] = BackupFileHash::HashWithSHA256(item);
            if (fileHash.empty()) {
                continue;
            }
            storageFiles.hash = fileHash;
            storageFiles.userTar = 1;
        }
        struct stat sta = {};
        if (stat(item.c_str(), &sta) != 0) {
            throw BError(BError::Codes::EXT_INVAL_ARG, "Get file stat failed");
        }
        storageFiles.size = sta.st_size;
        storageFiles.mode = to_string(static_cast<int32_t>(sta.st_mode));
        int64_t lastUpdateTime = static_cast<int64_t>(sta.st_mtime);
        storageFiles.mtime = lastUpdateTime;
        allFiles.emplace_back(storageFiles);
        smallFiles.emplace_back(storageFiles);
    }
    for (const auto &item : bigFile) {
        struct ReportFileInfo storageFiles;
        storageFiles.filePath = item;
        auto [res, fileHash] = BackupFileHash::HashWithSHA256(item);
        if (fileHash.empty()) {
            continue;
        }
        storageFiles.hash = fileHash;
        struct stat sta = {};
        if (stat(item.c_str(), &sta) != 0) {
            throw BError(BError::Codes::EXT_INVAL_ARG, "Get file stat failed");
        }
        storageFiles.size = sta.st_size;
        storageFiles.mode = to_string(static_cast<int32_t>(sta.st_mode));
        int64_t lastUpdateTime = static_cast<int64_t>(sta.st_mtime);
        storageFiles.mtime = lastUpdateTime;
        storageFiles.userTar = 1;
        allFiles.emplace_back(storageFiles);
        bigFiles.emplace_back(storageFiles);
    }
    HILOGI("get FileStat end, bigfiles = %{public}zu, smallFiles = %{public}zu, allFiles = %{public}zu,",
        bigFiles.size(), smallFiles.size(), allFiles.size());
}

static tuple<vector<string>, vector<string>> IsNotPath(const string &path, vector<string> &bigFiles,
    vector<string> &smallFiles, off_t size)
{
    struct stat sta = {};
    if (CheckOverLongPath(path) >= PATH_MAX_LEN || stat(path.data(), &sta) == -1) {
        return {};
    }
    if (sta.st_size <= size) {
        smallFiles.emplace_back(path);
        HILOGI("bigfiles = %{public}zu, smallfiles = %{public}zu", bigFiles.size(), smallFiles.size());
        return {bigFiles, smallFiles};
    }
    bigFiles.emplace_back(path);
    HILOGI("bigfiles = %{public}zu, smallfiles = %{public}zu", bigFiles.size(), smallFiles.size());
    return {bigFiles, smallFiles};
}

static tuple<vector<string>, vector<string>> GetUser0DirFilesDetail(const string &path, off_t size = -1)
{
    vector<string> bigFiles;
    vector<string> smallFiles;
    if (IsEmptyDirectory(path)) {
        string newPath = path;
        if (path.at(path.size()-1) != BConstants::FILE_SEPARATOR_CHAR) {
            newPath += BConstants::FILE_SEPARATOR_CHAR;
        }
        smallFiles.emplace_back(newPath);
        return {bigFiles, smallFiles};
    }
    if (filesystem::is_regular_file(path)) {
        return IsNotPath(path, bigFiles, smallFiles, size);
    }
    unique_ptr<DIR, function<void(DIR *)>> dir = {opendir(path.c_str()), closedir};
    if (!dir) {
        HILOGE("Invalid directory path: %{private}s", path.c_str());
        return {};
    }
    struct dirent *ptr = nullptr;
    while (!!(ptr = readdir(dir.get()))) {
        // current dir OR parent dir
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            continue;
        } else if (ptr->d_type == DT_REG) {
            struct stat sta = {};
            string fileName = IncludeTrailingPathDelimiter(path) + string(ptr->d_name);
            if (CheckOverLongPath(fileName) >= PATH_MAX_LEN || stat(fileName.data(), &sta) == -1) {
                continue;
            }
            if (sta.st_size <= size) {
                smallFiles.emplace_back(fileName);
                continue;
            }

            bigFiles.emplace_back(fileName);
            continue;
        } else if (ptr->d_type != DT_DIR) {
            HILOGE("Not support file type");
            continue;
        }
        // DT_DIR type
        auto [subBigFiles, subSmallFiles] =
            GetUser0DirFilesDetail(IncludeTrailingPathDelimiter(path) + string(ptr->d_name), size);
        bigFiles.insert(bigFiles.end(), subBigFiles.begin(), subBigFiles.end());
        smallFiles.insert(smallFiles.end(), subSmallFiles.begin(), subSmallFiles.end());
    }
    HILOGI("bigfiles = %{public}zu, smallfiles = %{public}zu", bigFiles.size(), smallFiles.size());
    return {bigFiles, smallFiles};
}

tuple<vector<string>, vector<string>> BDir::GetBackupList(const vector<string> &includes,
                                                          const vector<string> &excludes)
{
    HILOGI("start get bigfiles and smallfiles");
    set<string> inc = ExpandPathWildcard(includes, true);
    vector<string> bigFiles;
    vector<string> smallFiles;
    for (const auto &item : inc) {
        auto [bigFile, smallFile] = GetUser0DirFilesDetail(item, BConstants::BIG_FILE_BOUNDARY);
        bigFiles.insert(bigFiles.end(), bigFile.begin(), bigFile.end());
        smallFiles.insert(smallFiles.end(), smallFile.begin(), smallFile.end());
    }
    HILOGI("end bigfiles = %{public}zu, smallfiles = %{public}zu", bigFiles.size(), smallFiles.size());
    vector<string> endExcludes = excludes;
    PreDealExcludes(endExcludes);
    auto isMatch = [](const vector<string> &s, const string &str) -> bool {
        if (str.empty()) {
            return false;
        }
        for (const string &item : s) {
            if (fnmatch(item.data(), str.data(), FNM_LEADING_DIR) == 0) {
                return true;
            }
        }
        return false;
    };

    for (auto item = bigFiles.begin(); item != bigFiles.end();) {
        if (isMatch(endExcludes, *item)) {
            item = bigFiles.erase(item);
        } else {
            ++item;
        }
    }
    for (auto item = smallFiles.begin(); item != smallFiles.end();) {
        if (isMatch(endExcludes, *item)) {
            item = smallFiles.erase(item);
        } else {
            ++item;
        }
    }
    HILOGI("End compare bigfiles = %{public}zu, smallfiles = %{public}zu", bigFiles.size(), smallFiles.size());
    return {bigFiles, smallFiles};
}

vector<string> BDir::GetDirs(const vector<string_view> &paths)
{
    vector<string> wildcardPath(paths.begin(), paths.end());
    set<string> inc = ExpandPathWildcard(wildcardPath, true);
    vector<string> dirs(inc.begin(), inc.end());
    return dirs;
}

bool BDir::IsFilePathValid(const std::string &filePath)
{
    return AppFileService::SandboxHelper::IsValidPath(filePath);
}

bool BDir::IsDirsMatch(const vector<string> &excludePaths, const string &path)
{
    if (path.empty()) {
        return false;
    }
    for (const string &item : excludePaths) {
        if (fnmatch(item.data(), path.data(), FNM_LEADING_DIR) == 0) {
            return true;
        }
    }
    return false;
}
} // namespace OHOS::FileManagement::Backup