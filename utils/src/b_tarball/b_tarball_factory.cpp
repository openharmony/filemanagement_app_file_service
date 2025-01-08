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

#include "b_tarball/b_tarball_factory.h"

#include <algorithm>
#include <climits>
#include <cstring>
#include <iostream>
#include <iterator>
#include <libgen.h>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_tarball/b_tarball_cmdline.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
namespace {
const string UNTAT_ROOT = "/";
} // namespace

/**
 * @brief Verifying untar input parameters
 *
 * @param root Root directory for storing unpacked files
 * An absolute path is required.
 */
static void UntarFort(string_view root)
{
    auto resolvedPath = BExcepUltils::Canonicalize(root);
    if (string_view(UNTAT_ROOT) != root) {
        resolvedPath += UNTAT_ROOT;
    }
    if (string_view(resolvedPath) != root) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "The root must be an existing canonicalized path");
    }
}

/**
 * @brief Filtering tar input parameters
 *
 * @param tarballDir Directory where the package file is stored
 * @param root Root directory of the file to be packed
 * An absolute path is required.
 * @param includes Path to be packed in the root directory.
 * The relative path is required. If this parameter is not specified, all packages are packed by default.
 * @param excludes The part that does not need to be packed in the path to be packed
 * Requires a relative path. Can be used to exclude some subdirectories
 * @return std::tuple<vector<string>, vector<string>> 返回合法的includes, excludes
 */
static tuple<vector<string>, vector<string>> TarFilter(string_view tarballDir,
                                                       string_view root,
                                                       const vector<string_view> &includes,
                                                       const vector<string_view> &excludes)
{
    auto resolvedPath = make_unique<char[]>(PATH_MAX);
    if (!realpath(root.data(), resolvedPath.get()) || (string_view(resolvedPath.get()) != root)) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "The root must be an existing canonicalized path");
    }

    auto removeBackSlash = [](const string_view &arg) -> string {
        if (arg.empty()) {
            return "";
        }

        size_t i = arg.size() - 1;
        for (; i > 0; --i) {
            if (arg[i] != '/') {
                break;
            }
        }
        return {arg.data(), i + 1};
    };

    vector<string> newExcludes;
    for (auto &item : excludes) {
        string str = removeBackSlash(item);
        if (!str.empty()) {
            newExcludes.emplace_back(str);
        }
    }

    return {{includes.begin(), includes.end()}, newExcludes};
}

/**
 * @brief 校验tarball路径，并将之拆分为路径和文件名
 *
 * @param tarballPath tarball全路径
 * @return tuple<string, string> 路径和文件名
 */
static tuple<string, string> GetTarballDirAndName(string_view tarballPath)
{
    char *buf4Dir = strdup(tarballPath.data());
    if (!buf4Dir) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "Out of memory");
    }
    string tarballDir = dirname(buf4Dir);
    free(buf4Dir);

    char *buf4Name = strdup(tarballPath.data());
    if (!buf4Name) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "Out of memory");
    }
    string tarballName = basename(buf4Name);
    free(buf4Name);

    auto resolvedPath = make_unique<char[]>(PATH_MAX);
    if (!realpath(tarballDir.data(), resolvedPath.get())) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, generic_category().message(errno));
    }
    if (auto canonicalizedTarballDir = string_view(resolvedPath.get()); canonicalizedTarballDir != tarballDir) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "Tarball path differed after canonicalizing");
    }
    if (auto suffix = string_view(".tar");
        tarballPath.length() <= suffix.length() ||
        !equal(tarballPath.rbegin(), next(tarballPath.rbegin(), suffix.length()), suffix.rbegin(), suffix.rend())) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "Tarball path didn't end with '.tar'");
    }
    return {tarballDir, tarballName};
}

/**
 * @brief 绑定命令行实现的打包器
 *
 * @param tarballDir taball路径
 * @param tarballName taball文件名
 * @return unique_ptr<BTarballFactory::Impl> 打包器实现，包括tar和untar两种方法
 * @see GetTarballDirAndName
 */
static unique_ptr<BTarballFactory::Impl> BindCmdline(string_view tarballDir, string_view tarballName)
{
    auto ptr = make_shared<BTarballCmdline>(tarballDir, tarballName);

    return make_unique<BTarballFactory::Impl>(BTarballFactory::Impl {
        .tar = bind(&BTarballCmdline::Tar, ptr, placeholders::_1, placeholders::_2, placeholders::_3),
        .untar = bind(&BTarballCmdline::Untar, ptr, placeholders::_1),
    });
}

unique_ptr<BTarballFactory::Impl> BTarballFactory::Create(string_view implType, string_view tarballPath)
{
    static map<string_view, function<unique_ptr<BTarballFactory::Impl>(string_view, string_view)>> mapType2Tarball = {
        {"cmdline", BindCmdline},
    };

    try {
        auto [tarballDir, tarballName] = GetTarballDirAndName(tarballPath);
        auto tarballImpl = mapType2Tarball.at(implType)(tarballDir, tarballName);
        if (tarballImpl->tar) {
            tarballImpl->tar = [tarballDir {string(tarballDir)}, tar {tarballImpl->tar}](
                                   string_view root, vector<string_view> includes, vector<string_view> excludes) {
                auto [newIncludes, newExcludes] = TarFilter(tarballDir, root, includes, excludes);
                tar(root, {newIncludes.begin(), newIncludes.end()}, {newExcludes.begin(), newExcludes.end()});
            };
        }
        if (tarballImpl->untar) {
            tarballImpl->untar = [untar {tarballImpl->untar}](string_view root) {
                UntarFort(root);
                untar(root);
            };
        }
        return tarballImpl;
    } catch (const out_of_range &e) {
        stringstream ss;
        ss << "Unsupported implementation " << implType;
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, ss.str());
    }
}
} // namespace OHOS::FileManagement::Backup