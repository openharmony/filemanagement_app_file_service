/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include <dirent.h>
#include <fnmatch.h>
#include <functional>
#include <filesystem>
#include <glob.h>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "b_error/b_error.h"
#include "b_filesystem/b_file_hash.h"
#include "b_resources/b_constants.h"
#include "directory_ex.h"
#include "errors.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const int32_t PATH_MAX_LEN = 4096;
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

static tuple<ErrCode, map<string, struct stat>, map<string, size_t>> GetFile(const string &path, off_t size = -1)
{
    map<string, struct stat> files;
    map<string, size_t> smallFiles;
    struct stat sta = {};
    if (stat(path.data(), &sta) == -1) {
        return {BError(errno).GetCode(), files, smallFiles};
    }
    if (path == "/") {
        return {BError(BError::Codes::OK).GetCode(), files, smallFiles};
    }
    if (sta.st_size <= size) {
        smallFiles.insert(make_pair(path, sta.st_size));
    } else {
        files.try_emplace(path, sta);
    }
    return {BError(BError::Codes::OK).GetCode(), files, smallFiles};
}

static uint32_t CheckOverLongPath(const string &path)
{
    uint32_t len = path.length();
    if (len >= PATH_MAX_LEN) {
        size_t found = path.find_last_of(BConstants::FILE_SEPARATOR_CHAR);
        string sub = path.substr(found + 1);
        HILOGE("Path over long, length:%{public}d, fileName:%{public}s.", len, sub.c_str());
    }
    return len;
}

static tuple<ErrCode, map<string, struct stat>, map<string, size_t>> GetDirFilesDetail(const string &path,
                                                                                       bool recursion,
                                                                                       off_t size = -1)
{
    map<string, struct stat> files;
    map<string, size_t> smallFiles;

    if (IsEmptyDirectory(path)) {
        string newPath = path;
        if (path.at(path.size()-1) != BConstants::FILE_SEPARATOR_CHAR) {
            newPath += BConstants::FILE_SEPARATOR_CHAR;
        }
        smallFiles.insert(make_pair(newPath, 0));
        return {ERR_OK, files, smallFiles};
    }

    unique_ptr<DIR, function<void(DIR *)>> dir = {opendir(path.c_str()), closedir};
    if (!dir) {
        HILOGE("Invalid directory path: %{private}s", path.c_str());
        return GetFile(path, size);
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
                smallFiles.insert(make_pair(fileName, sta.st_size));
                continue;
            }

            files.try_emplace(fileName, sta);
            continue;
        } else if (ptr->d_type != DT_DIR) {
            HILOGE("Not support file type");
            continue;
        }
        // DT_DIR type
        if (!recursion) {
            continue;
        }
        auto [errCode, subFiles, subSmallFiles] =
            GetDirFilesDetail(IncludeTrailingPathDelimiter(path) + string(ptr->d_name), recursion, size);
        if (errCode != 0) {
            return {errCode, files, smallFiles};
        }
        files.merge(subFiles);
        smallFiles.insert(subSmallFiles.begin(), subSmallFiles.end());
    }
    return {ERR_OK, files, smallFiles};
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
            files.push_back(IncludeTrailingPathDelimiter(path) + string(ptr->d_name));
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

static set<string> ExpandPathWildcard(const vector<string> &vec, bool onlyPath)
{
    unique_ptr<glob_t, function<void(glob_t *)>> gl {new glob_t, [](glob_t *ptr) { globfree(ptr); }};
    *gl = {};

    unsigned int flags = GLOB_DOOFFS | GLOB_MARK;
    for (const string &pattern : vec) {
        if (!pattern.empty()) {
            glob(pattern.data(), static_cast<int>(flags), NULL, gl.get());
            flags |= GLOB_APPEND;
        }
    }

    set<string> expandPath, filteredPath;
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
        filteredPath.insert(*it);
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

tuple<ErrCode, map<string, struct stat>, map<string, size_t>> BDir::GetBigFiles(const vector<string> &includes,
                                                                                const vector<string> &excludes)
{
    set<string> inc = ExpandPathWildcard(includes, true);

    map<string, struct stat> incFiles;
    map<string, size_t> incSmallFiles;
    for (const auto &item : inc) {
        auto [errCode, files, smallFiles] = GetDirFilesDetail(item, true, BConstants::BIG_FILE_BOUNDARY);
        if (errCode == 0) {
            int32_t num = static_cast<int32_t>(files.size());
            incFiles.merge(move(files));
            HILOGI("big files: %{public}d; small files: %{public}d", num, static_cast<int32_t>(smallFiles.size()));
            incSmallFiles.insert(smallFiles.begin(), smallFiles.end());
        }
    }

    auto isMatch = [](const vector<string> &s, const string &str) -> bool {
        if (str.empty()) {
            return false;
        }
        for (const string &item : s) {
            if (item.empty()) {
                continue;
            }
            string excludeItem = item;
            if (excludeItem.at(item.size() - 1) == BConstants::FILE_SEPARATOR_CHAR) {
                excludeItem += "*";
            }
            if (fnmatch(excludeItem.data(), str.data(), FNM_LEADING_DIR) == 0) {
                return true;
            }
        }
        return false;
    };

    map<string, size_t> resSmallFiles;
    for (const auto &item : incSmallFiles) {
        if (!isMatch(excludes, item.first)) {
            resSmallFiles.insert(make_pair(item.first, item.second));
        }
    }

    map<string, struct stat> bigFiles;
    for (const auto &item : incFiles) {
        if (!isMatch(excludes, item.first)) {
            bigFiles[item.first] = item.second;
        }
    }
    HILOGI("total number of big files is %{public}d", static_cast<int32_t>(bigFiles.size()));
    HILOGI("total number of small files is %{public}d", static_cast<int32_t>(resSmallFiles.size()));
    return {ERR_OK, move(bigFiles), move(resSmallFiles)};
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
        allFiles.push_back(storageFiles);
        smallFiles.push_back(storageFiles);
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
        allFiles.push_back(storageFiles);
        bigFiles.push_back(storageFiles);
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
        smallFiles.push_back(path);
        HILOGI("bigfiles = %{public}zu, smallfiles = %{public}zu", bigFiles.size(), smallFiles.size());
        return {bigFiles, smallFiles};
    }
    bigFiles.push_back(path);
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
        smallFiles.push_back(newPath);
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
                smallFiles.push_back(fileName);
                continue;
            }

            bigFiles.push_back(fileName);
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
    auto isMatch = [](const vector<string> &s, const string &str) -> bool {
        if (str.empty()) {
            return false;
        }
        for (const string &item : s) {
            if (item.empty()) {
                continue;
            }
            string excludeItem = item;
            if (excludeItem.at(item.size() - 1) == BConstants::FILE_SEPARATOR_CHAR) {
                excludeItem += "*";
            }
            if (fnmatch(excludeItem.data(), str.data(), FNM_LEADING_DIR) == 0) {
                return true;
            }
        }
        return false;
    };

    for (auto item = bigFiles.begin(); item != bigFiles.end();) {
        if (isMatch(excludes, *item)) {
            item = bigFiles.erase(item);
        } else {
            ++item;
        }
    }
    for (auto item = smallFiles.begin(); item != smallFiles.end();) {
        if (isMatch(excludes, *item)) {
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
} // namespace OHOS::FileManagement::Backup