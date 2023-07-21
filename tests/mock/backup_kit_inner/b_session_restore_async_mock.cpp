/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "b_session_restore_async.h"

#include <fcntl.h>
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>

#include <file_ex.h>
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
static BSessionRestoreAsync::Callbacks callbacks_ = {};
static vector<BundleName> bundlesToRestore_ = {};
} // namespace

BSessionRestoreAsync::~BSessionRestoreAsync() {}

shared_ptr<BSessionRestoreAsync> BSessionRestoreAsync::Init(Callbacks callbacks)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsync::Init";
    try {
        auto restore = make_shared<BSessionRestoreAsync>(callbacks);
        return restore;
    } catch (const exception &e) {
        return nullptr;
    }
    return nullptr;
}

ErrCode BSessionRestoreAsync::PublishFile(BFileInfo fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode BSessionRestoreAsync::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

ErrCode BSessionRestoreAsync::AppendBundles(UniqueFd remoteCap,
                                            vector<BundleName> bundlesToRestore,
                                            RestoreTypeEnum restoreType,
                                            int32_t userId)
{
    GTEST_LOG_(INFO) << "BSessionRestoreAsync::AppendBundles";
    if (restoreType == RestoreTypeEnum::RESTORE_DATA_READDY) {
        callbacks_.onBackupServiceDied();
        return BError(BError::Codes::OK);
    }
    callbacks_.onBundleStarted(0, "com.example.app2backup");

    BFileInfo bFileInfo("com.example.app2backup", "1.tar", 0);
    TestManager tm("BSessionRestoreAsyncMock_GetFd_0100");
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

void BSessionRestoreAsync::OnBackupServiceDied() {}

void BSessionRestoreAsync::PopBundleInfo() {}

void BSessionRestoreAsync::AppendBundlesImpl(AppendBundleInfo info) {}

void BSessionRestoreAsync::OnBundleStarted(ErrCode errCode, const vector<BundleName> &bundlesToRestore) {}

void BSessionRestoreAsync::RegisterBackupServiceDied(function<void()> functor) {}
} // namespace OHOS::FileManagement::Backup