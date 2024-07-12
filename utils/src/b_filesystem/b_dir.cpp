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
#include <glob.h>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "directory_ex.h"
#include "errors.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const int32_t PATH_MAX_LEN = 4096;

static bool IsEmptyDirectory(const string &path)
{
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
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

static tuple<ErrCode, map<string, struct stat>, vector<string>> GetFile(const string &path, off_t size = -1)
{
    map<string, struct stat> files;
    vector<string> smallFiles;
    struct stat sta = {};
    if (stat(path.data(), &sta) == -1) {
        return {BError(errno).GetCode(), files, smallFiles};
    }
    if (path == "/") {
        return {BError(BError::Codes::OK).GetCode(), files, smallFiles};
    }
    if (sta.st_size <= size) {
        smallFiles.emplace_back(path);
    } else {
        files.try_emplace(path, sta);
    }
    return {BError(BError::Codes::OK).GetCode(), files, smallFiles};
}

static uint32_t CheckOverLongPath(const string &path)
{
    uint32_t len = path.length();
    if (len >= PATH_MAX_LEN) {
        size_t found = path.find_last_of('/');
        string sub = path.substr(found + 1);
        HILOGE("Path over long, length:%{public}d, fileName:%{public}s.", len, sub.c_str());
    }
    return len;
}

static tuple<ErrCode, map<string, struct stat>, vector<string>> GetDirFilesDetail(const string &path,
                                                                                  bool recursion,
                                                                                  off_t size = -1)
{
    map<string, struct stat> files;
    vector<string> smallFiles;

    if (IsEmptyDirectory(path)) {
        string newPath = path;
        if (path.at(path.size()-1) != '/') {
            newPath += '/';
        }
        smallFiles.emplace_back(newPath);
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
        } else if (ptr->d_type == DT_DIR) {
            if (!recursion) {
                continue;
            }
            auto [errCode, subFiles, subSmallFiles] =
                GetDirFilesDetail(IncludeTrailingPathDelimiter(path) + string(ptr->d_name), recursion, size);
            if (errCode != 0) {
                return {errCode, files, smallFiles};
            }
            files.merge(subFiles);
            smallFiles.insert(smallFiles.end(), subSmallFiles.begin(), subSmallFiles.end());
        } else if (ptr->d_type == DT_LNK) {
            continue;
        } else {
            struct stat sta = {};
            string fileName = IncludeTrailingPathDelimiter(path) + string(ptr->d_name);
            if (CheckOverLongPath(fileName) >= PATH_MAX_LEN || stat(fileName.data(), &sta) == -1) {
                continue;
            }
            if (sta.st_size <= size) {
                smallFiles.emplace_back(fileName);
                continue;
            }

            files.try_emplace(fileName, sta);
        }
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

static set<string> ExpandPathWildcard(const vector<string> &vec, bool onlyPath)
{
    unique_ptr<glob_t, function<void(glob_t *)>> gl {new glob_t, [](glob_t *ptr) { globfree(ptr); }};
    *gl = {};

    int flags = GLOB_DOOFFS | GLOB_MARK;
    for (const string &pattern : vec) {
        if (!pattern.empty()) {
            glob(pattern.data(), flags, NULL, gl.get());
            flags |= GLOB_APPEND;
        }
    }

    set<string> expandPath, filteredPath;
    for (size_t i = 0; i < gl->gl_pathc; ++i) {
        expandPath.emplace(gl->gl_pathv[i]);
    }

    for (auto it = expandPath.begin(); it != expandPath.end(); ++it) {
        filteredPath.insert(*it);
        if (onlyPath && *it->rbegin() != '/') {
            continue;
        }
        auto jt = it;
        for (++jt; jt != expandPath.end() && (jt->find(*it) == 0); ++jt) {
        }

        it = --jt;
    }

    return filteredPath;
}

tuple<ErrCode, map<string, struct stat>, vector<string>> BDir::GetBigFiles(const vector<string> &includes,
                                                                           const vector<string> &excludes)
{
    set<string> inc = ExpandPathWildcard(includes, false);

    map<string, struct stat> incFiles;
    vector<string> incSmallFiles;
    for (const auto &item : inc) {
        auto [errCode, files, smallFiles] = GetDirFilesDetail(item, true, BConstants::BIG_FILE_BOUNDARY);
        if (errCode == 0) {
            int32_t num = static_cast<int32_t>(files.size());
            incFiles.merge(move(files));
            HILOGI("big files: %{public}d; small files: %{public}d", num, static_cast<int32_t>(smallFiles.size()));
            incSmallFiles.insert(incSmallFiles.end(), smallFiles.begin(), smallFiles.end());
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
            if (excludeItem.at(item.size() - 1) == '/') {
                excludeItem += "*";
            }
            if (fnmatch(excludeItem.data(), str.data(), FNM_LEADING_DIR) == 0) {
                return true;
            }
        }
        return false;
    };

    vector<string> resSmallFiles;
    for (const auto &item : incSmallFiles) {
        if (!isMatch(excludes, item)) {
            resSmallFiles.emplace_back(item);
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

vector<string> BDir::GetDirs(const vector<string_view> &paths)
{
    vector<string> wildcardPath(paths.begin(), paths.end());
    set<string> inc = ExpandPathWildcard(wildcardPath, true);
    vector<string> dirs(inc.begin(), inc.end());
    return dirs;
}

} // namespace OHOS::FileManagement::Backup