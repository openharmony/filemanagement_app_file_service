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

#include "b_session_backup.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <file_ex.h>
#include <gtest/gtest.h>

#include "test_manager.h"
#include "b_error/b_error.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
static BSessionBackup::Callbacks callbacks_ = {};
} // namespace

BSessionBackup::~BSessionBackup() {}

unique_ptr<BSessionBackup> BSessionBackup::Init(Callbacks callbacks)
{
    try {
        callbacks_ = callbacks;
        auto backup = make_unique<BSessionBackup>();
        return backup;
    } catch (const exception &e) {
        return nullptr;
    }
    return nullptr;
}

void BSessionBackup::RegisterBackupServiceDied(function<void()> functor) {}

ErrCode BSessionBackup::Start()
{
    callbacks_.onBundleStarted(0, "com.example.app2backup");

    BFileInfo bFileInfo("com.example.app2backup", "manage.json", 0);
    TestManager tm("BSessionBackupMock_GetFd_0100");
    string fileManagePath = tm.GetRootDirCurTest().append("manage.json");
    SaveStringToFile(fileManagePath, R"([{"fileName": "1.tar"}])");
    UniqueFd fd(open(fileManagePath.data(), O_RDWR, S_IRWXU));
    GTEST_LOG_(INFO) << "callbacks_::onFileReady manage.json";
    callbacks_.onFileReady(bFileInfo, move(fd), 0);

    string filePath = tm.GetRootDirCurTest().append("1.tar");
    UniqueFd fdTar(open(filePath.data(), O_RDONLY | O_CREAT, S_IRWXU));
    bFileInfo.fileName = "1.tar";
    GTEST_LOG_(INFO) << "callbacks_::onFileReady 1.tar";
    callbacks_.onFileReady(bFileInfo, move(fdTar), 0);

    callbacks_.onBundleFinished(0, "com.example.app2backup");

    callbacks_.onAllBundlesFinished(0);
    callbacks_.onBundleStarted(BError::Codes::TOOL_INVAL_ARG, "com.example.app2backup");
    callbacks_.onBundleFinished(BError::Codes::TOOL_INVAL_ARG, "com.example.app2backup");
    callbacks_.onAllBundlesFinished(BError::Codes::TOOL_INVAL_ARG);

    string filePathTwo = tm.GetRootDirCurTest().append("1.tar");
    UniqueFd fdFile(open(filePathTwo.data(), O_RDONLY | O_CREAT, S_IRWXU));
    GTEST_LOG_(INFO) << "callbacks_::onFileReady 1.tar";
    callbacks_.onFileReady(bFileInfo, move(fdFile), 0);

    callbacks_.onBackupServiceDied();
    return BError(BError::Codes::OK);
}

ErrCode BSessionBackup::AppendBundles(vector<BundleName> bundlesToBackup)
{
    Start();
    return BError(BError::Codes::OK);
}

ErrCode BSessionBackup::Finish()
{
    return BError(BError::Codes::OK);
}

ErrCode BSessionBackup::Release()
{
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup