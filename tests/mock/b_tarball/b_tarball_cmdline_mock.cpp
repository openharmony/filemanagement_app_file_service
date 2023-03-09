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

#include "b_tarball/b_tarball_cmdline.h"

#include <sstream>
#include <string_view>

#include <unistd.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace OHOS::FileManagement::Backup {
using namespace std;

void BTarballCmdline::Tar(string_view root, vector<string_view> includes, vector<string_view> excludes)
{
    GTEST_LOG_(INFO) << "BTarballCmdline Tar " << root;
}

void BTarballCmdline::Untar(string_view root)
{
    GTEST_LOG_(INFO) << "BTarballCmdline Untar " << root;
}

BTarballCmdline::BTarballCmdline(string_view tarballDir, string_view tarballName)
    : tarballDir_(tarballDir), tarballName_(tarballName)
{
    tarballPath_ = tarballDir_ + "/" + tarballName_;
}
} // namespace OHOS::FileManagement::Backup
