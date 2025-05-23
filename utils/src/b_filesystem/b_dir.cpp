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

#include "b_anony/b_anony.h"
#include "b_error/b_error.h"
#include "b_filesystem/b_file_hash.h"
#include "b_resources/b_constants.h"
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

static tuple<ErrCode, map<string, struct stat>, map<string, size_t>> GetFile(const string &path, off_t size = -1)
{
    map<string, struct stat> files;
    map<string, size_t> smallFiles;
    struct stat sta = {};
    if (stat(path.data(), &sta) == -1) {
        HILOGE("File not exist, errno:%{public}d, fileName:%{private}s.", errno, path.c_str());
        return {BError(BError::Codes::OK).GetCode(), files, smallFiles};
    }
    if (path == "/") {
        return {BError(BError::Codes::OK).GetCode(), files, smallFiles};
    }
    if (sta.st_size <= size) {
        smallFiles.emplace(make_pair(path, sta.st_size));
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
        HILOGE("Path over long, length:%{public}d, fileName:%{private}s.", len, sub.c_str());
    }
    return len;
}

static void InsertSmallFiles(std::map<string, size_t> &smallFiles, std::string fileName, size_t size)
{
    ScanFileSingleton::GetInstance().AddSmallFile(fileName, size);
    smallFiles.emplace(make_pair(fileName, size));
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
        InsertSmallFiles(smallFiles, newPath, 0);
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
                InsertSmallFiles(smallFiles, fileName, sta.st_size);
                continue;
            }
            ScanFileSingleton::GetInstance().AddBigFile(fileName, sta);
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

static void PreDealExcludes(std::vector<std::string> &excludes)
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

static void UpdateFileStat(std::shared_ptr<RadarAppStatistic> appStatistic, std::string filePath, uint64_t fileSize,
    uint32_t& maxDirDepth)
{
    appStatistic->UpdateFileDist(ExtractFileExt(filePath), fileSize);
    uint32_t dirDepth = 0;
    const char* pstr = filePath.c_str();
    char pre = '-';
    uint32_t pathLen = filePath.size();
    for (int i = 0; i < pathLen; i++) {
        if (pstr[i] == '/' && pre != '/') {
            dirDepth++;
        }
        pre = pstr[i];
    }
    if (dirDepth > maxDirDepth) {
        maxDirDepth = dirDepth;
    }
}

tuple<ErrCode, map<string, struct stat>, map<string, size_t>> BDir::GetBigFiles(const vector<string> &includes,
    const vector<string> &excludes, std::shared_ptr<RadarAppStatistic> appStatistic)
{
    set<string> inc = ExpandPathWildcard(includes, true);

    map<string, struct stat> incFiles;
    map<string, size_t> incSmallFiles;
    for (const auto &item : inc) {
        HILOGW("GetBigFiles, path = %{public}s", item.c_str());
        auto [errCode, files, smallFiles] = GetDirFilesDetail(item, true, BConstants::BIG_FILE_BOUNDARY);
        if (errCode == 0) {
            HILOGW("big files: %{public}zu; small files: %{public}zu", files.size(), smallFiles.size());
            incFiles.merge(move(files));
            incSmallFiles.merge(move(smallFiles));
        }
    }
    vector<string> endExcludes = excludes;
    PreDealExcludes(endExcludes);
    if (excludes.empty()) {
        return {ERR_OK, move(incFiles), move(incSmallFiles)};
    }
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

    map<string, size_t> resSmallFiles;
    uint32_t maxDirDepth = BConstants::APP_BASE_PATH_DEPTH;
    for (const auto &item : incSmallFiles) {
        if (!isMatch(endExcludes, item.first)) {
            resSmallFiles.emplace(item);
            UpdateFileStat(appStatistic, item.first, item.second, maxDirDepth);
        }
    }

    map<string, struct stat> bigFiles;
    for (const auto &item : incFiles) {
        if (!isMatch(endExcludes, item.first)) {
            bigFiles.emplace(item);
            UpdateFileStat(appStatistic, item.first, item.second.st_size, maxDirDepth);
        }
    }
    appStatistic->dirDepth_ = maxDirDepth - BConstants::APP_BASE_PATH_DEPTH;
    HILOGW("total number of big files is %{public}zu", bigFiles.size());
    HILOGW("total number of small files is %{public}zu", resSmallFiles.size());
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

bool BDir::CheckAndRmSoftLink(const std::string &filePath)
{
    if (std::filesystem::is_symlink(filePath)) {
        HILOGE("Soft link is not allowed, path = %{public}s", GetAnonyPath(filePath).c_str());
        if (unlink(filePath.c_str()) < 0) {
            HILOGE("Failed to unlink the backup file : %{public}s", GetAnonyPath(filePath).c_str());
        }
        return true;
    }
    return false;
}

bool BDir::CheckAndRmSoftLink(const EndFileInfo &filePaths)
{
    bool isSoftLink = false;
    for (const auto &it : filePaths) {
        if (std::filesystem::is_symlink(it.first)) {
            HILOGE("Soft link is not allowed, path = %{public}s", GetAnonyPath(it.first).c_str());
            isSoftLink = true;
            if (unlink(it.first.c_str()) < 0) {
                HILOGE("Failed to unlink the backup file : %{public}s", GetAnonyPath(it.first).c_str());
            }
        }
    }
    return isSoftLink;
}
} // namespace OHOS::FileManagement::Backup