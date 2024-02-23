/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

pair<ErrCode, map<string, struct stat>> GetDirFilesDetail(const string &path, bool recursion, off_t size = -1)
{
    map<string, struct stat> files;
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
            if (!recursion) {
                continue;
            }

            auto [errCode, subfiles] =
                GetDirFilesDetail(IncludeTrailingPathDelimiter(path) + string(ptr->d_name), recursion, size);
            if (errCode != 0) {
                return {errCode, files};
            }
            files.merge(subfiles);
        } else if (ptr->d_type == DT_LNK) {
            continue;
        } else {
            struct stat sta = {};
            string fileName = IncludeTrailingPathDelimiter(path) + string(ptr->d_name);
            if (stat(fileName.data(), &sta) == -1) {
                continue;
            }
            if (sta.st_size < size) {
                continue;
            }
            HILOGI("Find big file");
            files.try_emplace(fileName, sta);
        }
    }

    return {BError(BError::Codes::OK).GetCode(), files};
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

    return {BError(BError::Codes::OK).GetCode(), files};
}

static set<string> ExpandPathWildcard(const vector<string> &vec)
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

    set<string> expandPath;
    set<string> filteredPath;
    for (size_t i = 0; i < gl->gl_pathc; ++i) {
        expandPath.emplace(gl->gl_pathv[i]);
    }

    for (auto it = expandPath.begin(); it != expandPath.end(); ++it) {
        filteredPath.insert(*it);
        if (*it->rbegin() != '/') {
            continue;
        }
        auto jt = it;
        for (++jt; jt != expandPath.end() && (jt->find(*it) == 0); ++jt) {
        }

        it = --jt;
    }

    return filteredPath;
}

pair<ErrCode, map<string, struct stat>> BDir::GetBigFiles(const vector<string> &includes,
                                                          const vector<string> &excludes)
{
    set<string> inc = ExpandPathWildcard(includes);

    map<string, struct stat> incFiles;
    for (const auto &item : inc) {
        auto [errCode, files] =
            OHOS::FileManagement::Backup::GetDirFilesDetail(item, true, BConstants::BIG_FILE_BOUNDARY);
        if (errCode == 0) {
            int32_t num = static_cast<int32_t>(files.size());
            HILOGI("found big files. total number is : %{public}d", num);
            incFiles.merge(move(files));
        }
    }

    auto isMatch = [](const vector<string> &s, const string &str) -> bool {
        if (str.empty()) {
            return false;
        }
        for (const string &item : s) {
            if (!item.empty() && (fnmatch(item.data(), str.data(), FNM_LEADING_DIR) == 0)) {
                HILOGI("file %{public}s matchs exclude condition", str.c_str());
                return true;
            }
        }
        return false;
    };

    map<string, struct stat> bigFiles;
    for (const auto &item : incFiles) {
        if (!isMatch(excludes, item.first)) {
            HILOGI("file %{public}s matchs include condition and unmatchs exclude condition", item.first.c_str());
            bigFiles[item.first] = item.second;
        }
    }
    int32_t num = static_cast<int32_t>(bigFiles.size());
    HILOGI("total number of big files is %{public}d", num);
    return {ERR_OK, move(bigFiles)};
}

vector<string> BDir::GetDirs(const vector<string_view> &paths)
{
    vector<string> wildcardPath(paths.begin(), paths.end());
    set<string> inc = ExpandPathWildcard(wildcardPath);
    vector<string> dirs(inc.begin(), inc.end());
    return dirs;
}
} // namespace OHOS::FileManagement::Backup