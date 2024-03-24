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

#include "b_session_restore.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <file_ex.h>
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
static BSessionRestore::Callbacks callbacks_ = {};
static vector<BundleName> bundlesToRestore_ = {};
} // namespace

BSessionRestore::~BSessionRestore() {}

unique_ptr<BSessionRestore> BSessionRestore::Init(Callbacks callbacks)
{
    try {
        callbacks_ = move(callbacks);
        auto restore = make_unique<BSessionRestore>();
        return restore;
    } catch (const exception &e) {
        return nullptr;
    }
    return nullptr;
}

ErrCode BSessionRestore::PublishFile(BFileInfo fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode BSessionRestore::Start()
{
    callbacks_.onBundleStarted(0, "com.example.app2backup");

    BFileInfo bFileInfo("com.example.app2backup", "1.tar", 0);
    TestManager tm("BSessionRestoreMock_GetFd_0100");
    string filePath = tm.GetRootDirCurTest().append("1.tar");
    UniqueFd fd(open(filePath.data(), O_RDWR | O_CREAT, S_IRWXU));
    GTEST_LOG_(INFO) << "callbacks_::onFileReady 1.tar";
    callbacks_.onFileReady(bFileInfo, move(fd));

    string fileManagePath = tm.GetRootDirCurTest().append("manage.json");
    UniqueFd fdManage(open(fileManagePath.data(), O_RDWR | O_CREAT, S_IRWXU));
    bFileInfo.fileName = "manage.json";
    GTEST_LOG_(INFO) << "callbacks_::onFileReady manage.json";
    callbacks_.onFileReady(bFileInfo, move(fdManage));

    callbacks_.onBundleFinished(0, "com.example.app2backup");

    callbacks_.onAllBundlesFinished(0);
    callbacks_.onBundleStarted(1, "com.example.app2backup");
    callbacks_.onBundleFinished(1, "com.example.app2backup");
    callbacks_.onAllBundlesFinished(1);

    callbacks_.onBackupServiceDied();
    return BError(BError::Codes::OK);
}

ErrCode BSessionRestore::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

ErrCode BSessionRestore::AppendBundles(UniqueFd remoteCap, vector<BundleName> bundlesToRestore,
    vector<string> detailInfos)
{
    Start();
    return BError(BError::Codes::OK);
}

ErrCode BSessionRestore::Finish()
{
    return BError(BError::Codes::OK);
}

ErrCode BSessionRestore::Release()
{
    return BError(BError::Codes::OK);
}

void BSessionRestore::RegisterBackupServiceDied(function<void()> functor) {}
} // namespace OHOS::FileManagement::Backup