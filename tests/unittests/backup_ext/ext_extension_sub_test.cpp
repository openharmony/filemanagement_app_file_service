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

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_json/b_json_entity_extension_config.h"
#include "ext_backup_mock.h"
#include "ext_extension_mock.h"
#include "tar_file.h"
#include "untar_file.h"

#include "sub_ext_extension.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string FILE_NAME = "1.txt";
const string BUNDLE_NAME = "com.example.app2backup/";
const string MANAGE_JSON = "manage.json";
const string PATH = "/data/storage/el2/backup/test/";
const string TAR_FILE = "1.tar";
const string BUNDLE_BASE_DIR = "/data/storage/el2/base/";
const string PATH_BUNDLE_BACKUP_HOME_EL1 = "/data/storage/el1/base/.backup";
const string PATH_BUNDLE_BACKUP_HOME = "/data/storage/el2/base/.backup";
} // namespace

class ExtExtensionSubTest : public testing::Test {
public:
    //所有测试用例执行之前执行
    static void SetUpTestCase(void);
    //所有测试用例执行之后执行
    static void TearDownTestCase(void);
    //每次测试用例执行之前执行
    void SetUp() {};
    //每次测试用例执行之后执行
    void TearDown() {};

    static inline sptr<BackupExtExtension> extExtension = nullptr;
    static inline shared_ptr<ExtBackup> extension = nullptr;
    static inline shared_ptr<ExtBackupMock> extBackupMock = nullptr;
    static inline shared_ptr<ExtExtensionMock> extExtensionMock = nullptr;
};

void ExtExtensionSubTest::SetUpTestCase(void)
{
    //创建测试路径
    string cmdMkdir = string("mkdir -p ") + PATH + BUNDLE_NAME;
    system(cmdMkdir.c_str());

    string cmdMkdir1 = string("mkdir -p ") + BUNDLE_BASE_DIR;
    system(cmdMkdir1.c_str());

    string cmdMkdir2 = string("mkdir -p ") + PATH_BUNDLE_BACKUP_HOME_EL1;
    system(cmdMkdir2.c_str());

    string cmdMkdir3 = string("mkdir -p ") + PATH_BUNDLE_BACKUP_HOME;
    system(cmdMkdir3.c_str());
    //创建测试文件
    string touchFile = string("touch ") + PATH + BUNDLE_NAME + FILE_NAME;
    system(touchFile.c_str());
    string touchFile2 = string("touch ") + PATH + BUNDLE_NAME + "2.txt";
    system(touchFile2.c_str());
    string touchFile3 = string("touch ") + PATH + BUNDLE_NAME + TAR_FILE;
    system(touchFile3.c_str());

    extBackupMock = make_shared<ExtBackupMock>();
    ExtBackupMock::extBackup = extBackupMock;

    extExtensionMock = make_shared<ExtExtensionMock>();
    ExtExtensionMock::extExtension = extExtensionMock;

    extExtension = sptr<BackupExtExtension>(new BackupExtExtension(
        nullptr, BUNDLE_NAME));
    extension = make_shared<ExtBackup>();
    extExtension->extension_ = extension;
};

void ExtExtensionSubTest::TearDownTestCase(void)
{
    //删除测试文件夹和文件
    string rmDir = string("rm -r ") + PATH + BUNDLE_NAME;
    system(rmDir.c_str());

    rmDir = string("rm -r ") + "/data/storage/el2/";
    system(rmDir.c_str());

    rmDir = string("rm -r ") + "/data/storage/el1/";
    system(rmDir.c_str());

    extension = nullptr;
    extExtension = nullptr;
    ExtBackupMock::extBackup = nullptr;
    extBackupMock = nullptr;
    ExtExtensionMock::extExtension = nullptr;
    extExtensionMock = nullptr;
};

/**
 * @tc.number: Ext_Extension_Sub_SetClearDataFlag_Test_0100
 * @tc.name: Ext_Extension_Sub_SetClearDataFlag_Test_0100
 * @tc.desc: 测试SetClearDataFlag
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_SetClearDataFlag_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_SetClearDataFlag_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        extExtension->SetClearDataFlag(true);
        EXPECT_TRUE(extExtension->isClearData_ == true);

        extExtension->extension_ = nullptr;
        extExtension->SetClearDataFlag(false);
        EXPECT_TRUE(extExtension->isClearData_ == false);
        
        extExtension->extension_ = extension;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_SetClearDataFlag_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_HandleIncrementalBackup_Test_0100
 * @tc.name: Ext_Extension_Sub_HandleIncrementalBackup_Test_0100
 * @tc.desc: 测试HandleIncrementalBackup
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_HandleIncrementalBackup_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_HandleIncrementalBackup_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        string incrementalFile = PATH + BUNDLE_NAME + "2.txt";
        int incrementalFd = open(incrementalFile.data(), O_RDWR | O_TRUNC, S_IRWXU);
        EXPECT_GT(incrementalFd, 0);
        string manifestFile = PATH + BUNDLE_NAME + FILE_NAME;
        int manifestFd = open(manifestFile.data(), O_RDWR | O_TRUNC, S_IRWXU);
        EXPECT_GT(manifestFd, 0);

        EXPECT_EQ(extExtension->HandleIncrementalBackup(incrementalFd, manifestFd), BError::E_FORBID);
        close(incrementalFd);
        close(manifestFd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_HandleIncrementalBackup_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_IncrementalOnBackup_Test_0100
 * @tc.name: Ext_Extension_Sub_IncrementalOnBackup_Test_0100
 * @tc.desc: 测试IncrementalOnBackup
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_IncrementalOnBackup_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_IncrementalOnBackup_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        EXPECT_EQ(extExtension->IncrementalOnBackup(false), BError::E_FORBID);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_IncrementalOnBackup_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_WaitToSendFd_Test_0100
 * @tc.name: Ext_Extension_Sub_WaitToSendFd_Test_0100
 * @tc.desc: 测试WaitToSendFd
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_WaitToSendFd_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_WaitToSendFd_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto startTime = std::chrono::system_clock::now();
        int fdSendNum = 61;
        extExtension->WaitToSendFd(startTime, fdSendNum);
        EXPECT_EQ(fdSendNum, 0);
        fdSendNum = 1;
        extExtension->WaitToSendFd(startTime, fdSendNum);
        EXPECT_EQ(fdSendNum, 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_WaitToSendFd_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_RefreshTimeInfo_Test_0100
 * @tc.name: Ext_Extension_Sub_RefreshTimeInfo_Test_0100
 * @tc.desc: 测试RefreshTimeInfo
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_RefreshTimeInfo_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_RefreshTimeInfo_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto startTime = std::chrono::system_clock::now();
        int fdSendNum = 1;
        extExtension->RefreshTimeInfo(startTime, fdSendNum);
        EXPECT_EQ(fdSendNum, 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_RefreshTimeInfo_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_GetBundlePath_Test_0100
 * @tc.name: Ext_Extension_Sub_GetBundlePath_Test_0100
 * @tc.desc: 测试GetBundlePath
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_GetBundlePath_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_GetBundlePath_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
        auto ret = extExtension->GetBundlePath();
        EXPECT_EQ(ret, path);
        
        extExtension->bundleName_ = BConstants::BUNDLE_FILE_MANAGER;
        path = string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
        ret = extExtension->GetBundlePath();
        EXPECT_EQ(ret, path);

        extExtension->bundleName_ = BConstants::BUNDLE_MEDIAL_DATA;
        path = string(BConstants::PATH_MEDIALDATA_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
        ret = extExtension->GetBundlePath();
        EXPECT_EQ(ret, path);
        extExtension->bundleName_ = BUNDLE_NAME;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_GetBundlePath_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_CheckRstoreFileInfos_Test_0100
 * @tc.name: Ext_Extension_Sub_CheckRstoreFileInfos_Test_0100
 * @tc.desc: 测试CheckRstoreFileInfos
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_CheckRstoreFileInfos_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_CheckRstoreFileInfos_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        tuple<bool, vector<string>> result;
        string tarName = PATH_BUNDLE_BACKUP_HOME + "/part0.tar";
        string untarPath = PATH_BUNDLE_BACKUP_HOME;

        result = extExtension->CheckRestoreFileInfos();
        EXPECT_EQ(std::get<0>(result), true);

        auto [err, fileInfos, errInfos] = UntarFile::GetInstance().UnPacket(tarName, untarPath);
        extExtension->endFileInfos_[tarName] = 1;
        extExtension->endFileInfos_.merge(fileInfos);

        result = extExtension->CheckRestoreFileInfos();
        EXPECT_EQ(std::get<0>(result), false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_CheckRstoreFileInfos_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_OnRestoreCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_OnRestoreCallback_Test_0100
 * @tc.desc: 测试OnRestoreCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_OnRestoreCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_OnRestoreCallback_Test_0100";
    try {
        ErrCode errCode = 0;
        string errMsg = "";

        ASSERT_TRUE(extExtension != nullptr);
        extExtension->SetClearDataFlag(false);
        std::function<void(ErrCode, string)> restoreCallBack = extExtension->OnRestoreCallback(nullptr);
        restoreCallBack(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        errMsg = "err";
        extExtension->isExecAppDone_.store(true);
        restoreCallBack = extExtension->OnRestoreCallback(extExtension);
        restoreCallBack(errCode, errMsg);
        extExtension->isExecAppDone_.store(false);
        EXPECT_EQ(errMsg, "err");
        
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_OnRestoreCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_OnRestoreExCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_OnRestoreExCallback_Test_0100
 * @tc.desc: 测试OnRestoreExCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_OnRestoreExCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_OnRestoreExCallback_Test_0100";
    try {
        ErrCode errCode = 0;
        string restoreRetInfo = "";

        ASSERT_TRUE(extExtension != nullptr);
        extExtension->SetClearDataFlag(false);
        std::function<void(ErrCode, string)> restoreCallBack = extExtension->OnRestoreExCallback(nullptr);
        restoreCallBack(errCode, restoreRetInfo);
        EXPECT_EQ(restoreRetInfo, "");

        restoreRetInfo = "";
        restoreCallBack = extExtension->OnRestoreExCallback(extExtension);
        extExtension->extension_ = nullptr;
        restoreCallBack(errCode, restoreRetInfo);
        extExtension->extension_ = extension;
        EXPECT_EQ(restoreRetInfo, "");

        restoreRetInfo = "err";
        extExtension->isExecAppDone_.store(true);
        restoreCallBack = extExtension->OnRestoreExCallback(extExtension);
        restoreCallBack(errCode, restoreRetInfo);
        extExtension->isExecAppDone_.store(false);
        EXPECT_EQ(restoreRetInfo, "err");

        extExtension->isExecAppDone_.store(false);
        restoreRetInfo = "";
        restoreCallBack = extExtension->OnRestoreExCallback(extExtension);
        restoreCallBack(errCode, restoreRetInfo);
        EXPECT_EQ(restoreRetInfo, "");

        restoreRetInfo = "err";
        restoreCallBack = extExtension->OnRestoreExCallback(extExtension);
        restoreCallBack(errCode, restoreRetInfo);
        EXPECT_EQ(restoreRetInfo, "err");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_OnRestoreExCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_AppDoneCallbackEx_Test_0100
 * @tc.name: Ext_Extension_Sub_AppDoneCallbackEx_Test_0100
 * @tc.desc: 测试AppDoneCallbackEx
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_AppDoneCallbackEx_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_AppDoneCallbackEx_Test_0100";
    try {
        ErrCode errCode = 0;
        string errMessage = "";

        ASSERT_TRUE(extExtension != nullptr);
        extExtension->SetClearDataFlag(false);
        std::function<void(ErrCode, string)> restoreCallBack = extExtension->AppDoneCallbackEx(nullptr);
        restoreCallBack(errCode, errMessage);
        EXPECT_EQ(errMessage, "");

        restoreCallBack = extExtension->AppDoneCallbackEx(extExtension);
        restoreCallBack(errCode, errMessage);
        extExtension->extension_ = extension;
        EXPECT_EQ(errMessage, "");
        
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_AppDoneCallbackEx_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_GetComInfoCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_GetComInfoCallback_Test_0100
 * @tc.desc: 测试GetComInfoCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_GetComInfoCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_GetComInfoCallback_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        std::string compatibilityInfo = "test";
        extExtension->GetComInfoCallback(nullptr)(BError(BError::Codes::OK).GetCode(), compatibilityInfo);
        EXPECT_NE(extExtension->compatibilityInfo_, compatibilityInfo);

        auto extensionTmp = extExtension->extension_;
        extExtension->extension_ = nullptr;
        auto ptr = wptr<BackupExtExtension>(extExtension);
        extExtension->GetComInfoCallback(ptr)(BError(BError::Codes::OK).GetCode(), compatibilityInfo);
        extExtension->extension_ = extensionTmp;
        EXPECT_NE(extExtension->compatibilityInfo_, compatibilityInfo);

        bool stopGetComInfo = extExtension->stopGetComInfo_.load();
        extExtension->stopGetComInfo_.store(true);
        extExtension->GetComInfoCallback(ptr)(BError(BError::Codes::OK).GetCode(), compatibilityInfo);
        extExtension->stopGetComInfo_.store(stopGetComInfo);
        EXPECT_NE(extExtension->compatibilityInfo_, compatibilityInfo);

        extExtension->stopGetComInfo_.store(false);
        extExtension->GetComInfoCallback(ptr)(BError(BError::Codes::OK).GetCode(), compatibilityInfo);
        extExtension->stopGetComInfo_.store(stopGetComInfo);
        EXPECT_EQ(extExtension->compatibilityInfo_, compatibilityInfo);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_GetComInfoCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_HandleGetCompatibilityInfo_Test_0100
 * @tc.name: Ext_Extension_Sub_HandleGetCompatibilityInfo_Test_0100
 * @tc.desc: 测试HandleGetCompatibilityInfo
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_HandleGetCompatibilityInfo_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_HandleGetCompatibilityInfo_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        std::string extInfo = "test";
        std::string compatibilityInfo = "";
        int32_t scenario = BConstants::ExtensionScenario::BACKUP;
        bool isExist = true;
        auto ret = extExtension->HandleGetCompatibilityInfo(extInfo, scenario, isExist, compatibilityInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        isExist = false;
        ret = extExtension->HandleGetCompatibilityInfo(extInfo, scenario, isExist, compatibilityInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        scenario = BConstants::ExtensionScenario::RESTORE;
        ret = extExtension->HandleGetCompatibilityInfo(extInfo, scenario, isExist, compatibilityInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        scenario = BConstants::ExtensionScenario::INVALID;
        ret = extExtension->HandleGetCompatibilityInfo(extInfo, scenario, isExist, compatibilityInfo);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_INVAL_ARG).GetCode());

        scenario = BConstants::ExtensionScenario::BACKUP;
        EXPECT_CALL(*extBackupMock, GetBackupCompatibilityInfo(_, _))
            .WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG).GetCode()));
        ret = extExtension->HandleGetCompatibilityInfo(extInfo, scenario, isExist, compatibilityInfo);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_HandleGetCompatibilityInfo_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_HandleOnRelease_Test_0100
 * @tc.name: Ext_Extension_Sub_HandleOnRelease_Test_0100
 * @tc.desc: 测试HandleOnRelease
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_HandleOnRelease_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_HandleOnRelease_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        int32_t scenario = 1;
        bool isOnReleased = extExtension->isOnReleased_.load();
        extExtension->isOnReleased_.store(true);
        auto ret = extExtension->HandleOnRelease(scenario);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        extExtension->isOnReleased_.store(false);
        auto ext = extExtension->extension_;
        extExtension->extension_ = nullptr;
        ret = extExtension->HandleOnRelease(scenario);
        extExtension->extension_ = ext;
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        ret = extExtension->HandleOnRelease(scenario);
        extExtension->isOnReleased_.store(isOnReleased);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_HandleOnRelease_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_HandleExtOnRelease_Test_0100
 * @tc.name: Ext_Extension_Sub_HandleExtOnRelease_Test_0100
 * @tc.desc: 测试HandleExtOnRelease
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_HandleExtOnRelease_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_HandleExtOnRelease_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto curScenario = extExtension->curScenario_;
        extExtension->curScenario_ = BackupRestoreScenario::FULL_BACKUP;
        auto ext = extExtension->extension_;
        extExtension->extension_ = nullptr;
        extExtension->HandleExtOnRelease(false, BError(BError::Codes::OK).GetCode());
        EXPECT_TRUE(true);

        extExtension->curScenario_ = BackupRestoreScenario::FULL_RESTORE;
        extExtension->HandleExtOnRelease(false, BError(BError::Codes::OK).GetCode());
        EXPECT_TRUE(true);

        extExtension->extension_ = ext;
        EXPECT_NE(extExtension->extension_, nullptr);
        bool isOnReleased = extExtension->isOnReleased_.load();
        extExtension->isOnReleased_.store(true);
        extExtension->HandleExtOnRelease(false, BError(BError::Codes::OK).GetCode());
        EXPECT_TRUE(true);

        extExtension->isOnReleased_.store(false);
        extExtension->HandleExtOnRelease(false, BError(BError::Codes::OK).GetCode());
        EXPECT_TRUE(true);

        extExtension->needAppResultReport_.store(false);
        extExtension->HandleExtOnRelease(false, BError(BError::Codes::OK).GetCode());
        EXPECT_TRUE(true);

        extExtension->isOnReleased_.store(isOnReleased);
        extExtension->curScenario_ = curScenario;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_HandleExtOnRelease_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_IfCloudSpecialRestore_Test_0100
 * @tc.name: Ext_Extension_Sub_IfCloudSpecialRestore_Test_0100
 * @tc.desc: 测试IfCloudSpecialRestore
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_IfCloudSpecialRestore_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_IfCloudSpecialRestore_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        std::string tarName = "test";
        auto ret = extExtension->IfCloudSpecialRestore(tarName);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_IfCloudSpecialRestore_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_CompareFiles_Test_0100
 * @tc.name: Ext_Extension_Sub_CompareFiles_Test_0100
 * @tc.desc: 测试CompareFiles
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_CompareFiles_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_CompareFiles_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        string actualPath1 = PATH + BUNDLE_NAME + FILE_NAME;
        string actualPath2 = PATH + BUNDLE_NAME + "2.txt";
        ReportFileInfo fileInfo1, fileInfo2, fileInfo3, fileInfo4, fileInfo5;
        fileInfo1.filePath = "";
        fileInfo2.filePath = "test2";
        fileInfo2.isIncremental = true;
        fileInfo2.isDir = true;
        fileInfo3.filePath = actualPath1;
        fileInfo3.isIncremental = true;
        fileInfo4.filePath = actualPath2;
        fileInfo4.isIncremental = true;
        fileInfo4.size = BConstants::BIG_FILE_BOUNDARY + 1;
        fileInfo5.filePath = "test5";
        fileInfo5.isIncremental = true;
        unordered_map<string, struct ReportFileInfo> localFilesInfo {{"test1", fileInfo1}, {"test2", fileInfo2},
            {actualPath1, fileInfo3}, {actualPath2, fileInfo4}, {"test5", fileInfo5}};
        unordered_map<string, struct ReportFileInfo> cloudFiles {{"test5", fileInfo5}};
        vector<struct ReportFileInfo> allFiles, smallFiles, bigFiles;
        extExtension->CompareFiles(allFiles, smallFiles, bigFiles, cloudFiles, localFilesInfo);
        EXPECT_EQ(allFiles.size(), 4);
        EXPECT_EQ(smallFiles.size(), 2);
        EXPECT_EQ(bigFiles.size(), 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_CompareFiles_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_ReportOnProcessResultCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_ReportOnProcessResultCallback_Test_0100
 * @tc.desc: 测试ReportOnProcessResultCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_ReportOnProcessResultCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_ReportOnProcessResultCallback_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        BackupRestoreScenario scenario = BackupRestoreScenario::FULL_BACKUP;
        ErrCode errCode = BError(BError::Codes::OK).GetCode();
        string processInfo = "";
        extExtension->ReportOnProcessResultCallback(nullptr, scenario)(errCode, processInfo);

        auto ptr = wptr<BackupExtExtension>(extExtension);
        extExtension->onProcessTimeout_.store(true);
        extExtension->ReportOnProcessResultCallback(ptr, scenario)(errCode, processInfo);
        EXPECT_FALSE(extExtension->onProcessTimeout_.load());

        extExtension->onProcessTimeoutCnt_.store(1);
        extExtension->ReportOnProcessResultCallback(ptr, scenario)(errCode, processInfo);
        EXPECT_EQ(extExtension->onProcessTimeoutCnt_.load(), 0);

        extExtension->onProcessTimeoutCnt_.store(0);
        processInfo = "test";
        extExtension->ReportOnProcessResultCallback(ptr, scenario)(errCode, processInfo);
        EXPECT_EQ(extExtension->onProcessTimeoutCnt_.load(), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_ReportOnProcessResultCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_UpdateFileStat_Test_0100
 * @tc.name: Ext_Extension_Sub_UpdateFileStat_Test_0100
 * @tc.desc: 测试UpdateFileStat
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_UpdateFileStat_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_UpdateFileStat_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        string actualPath = PATH + BUNDLE_NAME + FILE_NAME;
        extExtension->UpdateFileStat(actualPath, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_UpdateFileStat_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_IfAllowToBackupRestore_Test_0100
 * @tc.name: Ext_Extension_Sub_IfAllowToBackupRestore_Test_0100
 * @tc.desc: 测试IfAllowToBackupRestore
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_IfAllowToBackupRestore_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_IfAllowToBackupRestore_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto extension = extExtension->extension_;
        extExtension->extension_ = nullptr;
        auto ret = extExtension->IfAllowToBackupRestore();
        EXPECT_FALSE(ret);

        extExtension->extension_ = extension;
        string usrConfig = "{\"allowToBackupRestore\":false}";
        EXPECT_CALL(*extBackupMock, GetUsrConfig()).WillOnce(Return(usrConfig));
        ret = extExtension->IfAllowToBackupRestore();
        EXPECT_FALSE(ret);

        usrConfig = "{\"allowToBackupRestore\":true}";
        EXPECT_CALL(*extBackupMock, GetUsrConfig()).WillOnce(Return(usrConfig));
        ret = extExtension->IfAllowToBackupRestore();
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_IfAllowToBackupRestore_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_SetStagingPathProperties_Test_0100
 * @tc.name: Ext_Extension_Sub_SetStagingPathProperties_Test_0100
 * @tc.desc: 测试SetStagingPathProperties
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_SetStagingPathProperties_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_SetStagingPathProperties_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto ret = extExtension->SetStagingPathProperties();
        EXPECT_TRUE(ret);

        string dirAct = string("rm -r ") + BUNDLE_BASE_DIR;
        system(dirAct.c_str());
        ret = extExtension->SetStagingPathProperties();
        dirAct = string("mkdir -p ") + BUNDLE_BASE_DIR;
        system(dirAct.c_str());
        EXPECT_FALSE(ret);

        dirAct = string("rm -r ") + PATH_BUNDLE_BACKUP_HOME;
        system(dirAct.c_str());
        ret = extExtension->SetStagingPathProperties();
        dirAct = string("mkdir -p ") + PATH_BUNDLE_BACKUP_HOME;
        system(dirAct.c_str());
        EXPECT_FALSE(ret);

        dirAct = string("rm -r ") + PATH_BUNDLE_BACKUP_HOME_EL1;
        system(dirAct.c_str());
        ret = extExtension->SetStagingPathProperties();
        dirAct = string("mkdir -p ") + PATH_BUNDLE_BACKUP_HOME_EL1;
        system(dirAct.c_str());
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_SetStagingPathProperties_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_UpdateFdSendRate_Test_0100
 * @tc.name: Ext_Extension_Sub_UpdateFdSendRate_Test_0100
 * @tc.desc: 测试UpdateFdSendRate
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_UpdateFdSendRate_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_UpdateFdSendRate_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto ret = extExtension->UpdateFdSendRate(BUNDLE_NAME, 0);
        EXPECT_EQ(extExtension->sendRate_, 0);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        ret = extExtension->UpdateFdSendRate(BUNDLE_NAME, 10);
        EXPECT_EQ(extExtension->sendRate_, 10);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_UpdateFdSendRate_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_GetBackupInfo_Test_0100
 * @tc.name: Ext_Extension_Sub_GetBackupInfo_Test_0100
 * @tc.desc: 测试GetBackupInfo
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_GetBackupInfo_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_GetBackupInfo_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        string result = "";
        auto ext = extExtension->extension_;
        extExtension->extension_ = nullptr;
        auto ret = extExtension->GetBackupInfo(result);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_INVAL_ARG).GetCode());

        extExtension->extension_ = ext;
        EXPECT_CALL(*extBackupMock, GetBackupInfo(_)).WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG).GetCode()));
        ret = extExtension->GetBackupInfo(result);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_INVAL_ARG).GetCode());

        EXPECT_CALL(*extBackupMock, GetBackupInfo(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        ret = extExtension->GetBackupInfo(result);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_GetBackupInfo_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_SyncCallJsOnProcessTask_Test_0100
 * @tc.name: Ext_Extension_Sub_SyncCallJsOnProcessTask_Test_0100
 * @tc.desc: 测试SyncCallJsOnProcessTask
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_SyncCallJsOnProcessTask_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_SyncCallJsOnProcessTask_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto obj = wptr<BackupExtExtension>(extExtension);
        BackupRestoreScenario scenario = BackupRestoreScenario::FULL_BACKUP;
        extExtension->stopCallJsOnProcess_.store(true);
        extExtension->SyncCallJsOnProcessTask(obj, scenario);
        EXPECT_TRUE(extExtension->stopCallJsOnProcess_.load());

        extExtension->stopCallJsOnProcess_.store(false);
        extExtension->SyncCallJsOnProcessTask(nullptr, scenario);
        EXPECT_FALSE(extExtension->stopCallJsOnProcess_.load());

        auto ext = extExtension->extension_;
        extExtension->extension_ = nullptr;
        extExtension->SyncCallJsOnProcessTask(obj, scenario);
        EXPECT_FALSE(extExtension->stopCallJsOnProcess_.load());

        extExtension->extension_ = ext;
        EXPECT_CALL(*extBackupMock, OnProcess(_)).WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG).GetCode()));
        extExtension->SyncCallJsOnProcessTask(obj, scenario);
        EXPECT_FALSE(extExtension->stopCallJsOnProcess_.load());

        EXPECT_CALL(*extBackupMock, OnProcess(_)).WillOnce(Return(BError(BError::Codes::OK).GetCode()));
        extExtension->SyncCallJsOnProcessTask(obj, scenario);
        EXPECT_FALSE(extExtension->stopCallJsOnProcess_.load());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_SyncCallJsOnProcessTask_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_IncreOnRestoreExCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_IncreOnRestoreExCallback_Test_0100
 * @tc.desc: 测试IncreOnRestoreExCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_IncreOnRestoreExCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_IncreOnRestoreExCallback_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto obj = wptr<BackupExtExtension>(extExtension);
        ErrCode errCode = BError(BError::Codes::OK).GetCode();
        string restoreRetInfo = "";
        extExtension->IncreOnRestoreExCallback(nullptr)(errCode, restoreRetInfo);
        EXPECT_EQ(restoreRetInfo, "");

        auto ext = extExtension->extension_;
        extExtension->extension_ = nullptr;
        extExtension->IncreOnRestoreExCallback(obj)(errCode, restoreRetInfo);
        EXPECT_EQ(restoreRetInfo, "");

        extExtension->extension_ = ext;
        extExtension->isExecAppDone_.store(true);
        extExtension->IncreOnRestoreExCallback(obj)(errCode, restoreRetInfo);
        EXPECT_TRUE(extExtension->isExecAppDone_.load());

        extExtension->isExecAppDone_.store(false);
        restoreRetInfo = "test";
        extExtension->IncreOnRestoreExCallback(obj)(errCode, restoreRetInfo);
        EXPECT_EQ(restoreRetInfo, "test");

        restoreRetInfo = "";
        extExtension->IncreOnRestoreExCallback(obj)(errCode, restoreRetInfo);
        EXPECT_EQ(restoreRetInfo, "");

        errCode = BError(BError::Codes::EXT_INVAL_ARG).GetCode();
        extExtension->IncreOnRestoreExCallback(obj)(errCode, restoreRetInfo);
        EXPECT_EQ(restoreRetInfo, "");

        restoreRetInfo = "test";
        extExtension->IncreOnRestoreExCallback(obj)(errCode, restoreRetInfo);
        EXPECT_EQ(restoreRetInfo, "test");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_IncreOnRestoreExCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_IncreOnRestoreCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_IncreOnRestoreCallback_Test_0100
 * @tc.desc: 测试IncreOnRestoreCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_IncreOnRestoreCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_IncreOnRestoreCallback_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto obj = wptr<BackupExtExtension>(extExtension);
        ErrCode errCode = BError(BError::Codes::OK).GetCode();
        string errMsg = "";
        extExtension->IncreOnRestoreCallback(nullptr)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        extExtension->isExecAppDone_.store(true);
        extExtension->IncreOnRestoreCallback(obj)(errCode, errMsg);
        EXPECT_TRUE(extExtension->isExecAppDone_.load());

        extExtension->isExecAppDone_.store(false);
        errMsg = "test";
        extExtension->IncreOnRestoreCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "test");

        errMsg = "";
        extExtension->IncreOnRestoreCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        errCode = BError(BError::Codes::EXT_INVAL_ARG).GetCode();
        extExtension->IncreOnRestoreCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_IncreOnRestoreCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_OnBackupCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_OnBackupCallback_Test_0100
 * @tc.desc: 测试OnBackupCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_OnBackupCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_OnBackupCallback_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto obj = wptr<BackupExtExtension>(extExtension);
        ErrCode errCode = BError(BError::Codes::OK).GetCode();
        string errMsg = "";
        extExtension->OnBackupCallback(nullptr)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        auto ext = extExtension->extension_;
        extExtension->extension_ = nullptr;
        extExtension->OnBackupCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        extExtension->extension_ = ext;
        extExtension->isExecAppDone_.store(true);
        extExtension->OnBackupCallback(obj)(errCode, errMsg);
        EXPECT_TRUE(extExtension->isExecAppDone_.load());

        extExtension->isExecAppDone_.store(false);
        errMsg = "test";
        string usrConfig = "{\"allowToBackupRestore\":true}";
        EXPECT_CALL(*extBackupMock, GetUsrConfig()).WillOnce(Return(usrConfig));
        extExtension->OnBackupCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "test");

        errCode = BError(BError::Codes::EXT_INVAL_ARG).GetCode();
        extExtension->OnBackupCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "test");

        errMsg = "";
        extExtension->OnBackupCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_OnBackupCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_OnBackupExCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_OnBackupExCallback_Test_0100
 * @tc.desc: 测试OnBackupExCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_OnBackupExCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_OnBackupExCallback_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto obj = wptr<BackupExtExtension>(extExtension);
        ErrCode errCode = BError(BError::Codes::OK).GetCode();
        string errMsg = "";
        extExtension->OnBackupExCallback(nullptr)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        auto ext = extExtension->extension_;
        extExtension->extension_ = nullptr;
        extExtension->OnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        extExtension->extension_ = ext;
        extExtension->isExecAppDone_.store(true);
        extExtension->OnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_TRUE(extExtension->isExecAppDone_.load());

        extExtension->isExecAppDone_.store(false);
        errMsg = "test";
        string usrConfig = "{\"allowToBackupRestore\":true}";
        EXPECT_CALL(*extBackupMock, GetUsrConfig()).WillOnce(Return(usrConfig));
        extExtension->OnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "test");

        errMsg = "";
        extExtension->OnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        errCode = BError(BError::Codes::EXT_INVAL_ARG).GetCode();
        extExtension->OnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        errMsg = "test";
        extExtension->OnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "test");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_OnBackupExCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_IncOnBackupCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_IncOnBackupCallback_Test_0100
 * @tc.desc: 测试IncOnBackupCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_IncOnBackupCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_IncOnBackupCallback_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto obj = wptr<BackupExtExtension>(extExtension);
        ErrCode errCode = BError(BError::Codes::OK).GetCode();
        string errMsg = "";
        extExtension->IncOnBackupCallback(nullptr)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        extExtension->isExecAppDone_.store(true);
        extExtension->IncOnBackupCallback(obj)(errCode, errMsg);
        EXPECT_TRUE(extExtension->isExecAppDone_.load());

        extExtension->isExecAppDone_.store(false);
        errMsg = "test";
        extExtension->IncOnBackupCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "test");

        errCode = BError(BError::Codes::EXT_INVAL_ARG).GetCode();
        extExtension->IncOnBackupCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "test");

        errMsg = "";
        extExtension->IncOnBackupCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_IncOnBackupCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_IncOnBackupExCallback_Test_0100
 * @tc.name: Ext_Extension_Sub_IncOnBackupExCallback_Test_0100
 * @tc.desc: 测试IncOnBackupExCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_IncOnBackupExCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_IncOnBackupExCallback_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        auto obj = wptr<BackupExtExtension>(extExtension);
        ErrCode errCode = BError(BError::Codes::OK).GetCode();
        string errMsg = "";
        extExtension->IncOnBackupExCallback(nullptr)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        auto ext = extExtension->extension_;
        extExtension->extension_ = nullptr;
        extExtension->IncOnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        extExtension->extension_ = ext;
        extExtension->isExecAppDone_.store(true);
        extExtension->IncOnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_TRUE(extExtension->isExecAppDone_.load());

        extExtension->isExecAppDone_.store(false);
        errMsg = "test";
        extExtension->IncOnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "test");

        errMsg = "";
        extExtension->IncOnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        errCode = BError(BError::Codes::EXT_INVAL_ARG).GetCode();
        extExtension->IncOnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "");

        errMsg = "test";
        extExtension->IncOnBackupExCallback(obj)(errCode, errMsg);
        EXPECT_EQ(errMsg, "test");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_IncOnBackupExCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_GetScanDirList_Test_0100
 * @tc.name: Ext_Extension_Sub_GetScanDirList_Test_0100
 * @tc.desc: 测试GetScanDirList
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_GetScanDirList_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_GetScanDirList_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        extension->backupScene_ = "test1";
        vector<string> includes = {};
        string usrConfig = "{\"allowToBackupRestore\":false}";

        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity1(usrConfig);
        extExtension->extension_ = nullptr;
        extExtension->GetScanDirList(includes, BConstants::INCLUDES, cachedEntity1.Structuralize());
        extExtension->extension_ = extension;
        EXPECT_TRUE(includes.empty());

        usrConfig = "{\"includes\":[\"test/\"]}";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity2(usrConfig);
        extExtension->GetScanDirList(includes, BConstants::INCLUDES, cachedEntity2.Structuralize());
        EXPECT_FALSE(includes.empty());

        usrConfig = "{\"optionDir\":[{\"includes\":[\"test/\"], \"sceneId\":\"test\"}]}";
        extension->backupScene_ = "test";
        BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity3(usrConfig);
        extExtension->GetScanDirList(includes, BConstants::INCLUDES, cachedEntity3.Structuralize());
        EXPECT_FALSE(includes.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_GetScanDirList_Test_0100";
}

/**
 * @tc.number: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0100
 * @tc.name: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0100
 * @tc.desc: 测试PathHasEl3OrEl4 不包含el3/el4
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_PathHasEl3OrEl4_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_PathHasEl3OrEl4_Test_0100";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        const std::set<std::string> includes = {
            "/data/storage/el2/",
            "/data/storage/el1/"
        };
        const std::vector<std::string> excludes = {
            "/data/storage/el2/base/"
        };
        extExtension->PathHasEl3OrEl4(includes, excludes);
        EXPECT_FALSE(extExtension->appStatistic_->hasEl3OrEl4_.load());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_PathHasEl3OrEl4_Test_0100";
}
 
/**
 * @tc.number: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0101
 * @tc.name: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0101
 * @tc.desc: 测试PathHasEl3OrEl4 排除el3
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_PathHasEl3OrEl4_Test_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_PathHasEl3OrEl4_Test_0101";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        const std::set<std::string> includes = {
            "/data/storage/el2/",
            "/data/storage/el3/"
        };
        const std::vector<std::string> excludes = {
            "/data/storage/el3/",
            "/data/storage/el2/base/"
        };
        extExtension->PathHasEl3OrEl4(includes, excludes);
        EXPECT_FALSE(extExtension->appStatistic_->hasEl3OrEl4_.load());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_PathHasEl3OrEl4_Test_0101";
}
 
/**
 * @tc.number: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0102
 * @tc.name: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0102
 * @tc.desc: 测试PathHasEl3OrEl4 排除el3/el4目录
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_PathHasEl3OrEl4_Test_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_PathHasEl3OrEl4_Test_0102";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        const std::set<std::string> includes = {
            "/data/storage/"
        };
        const std::vector<std::string> excludes = {
            "/data/storage/el3/",
            "/data/storage/el4/"
        };
        extExtension->PathHasEl3OrEl4(includes, excludes);
        EXPECT_FALSE(extExtension->appStatistic_->hasEl3OrEl4_.load());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_PathHasEl3OrEl4_Test_0102";
}
 
/**
 * @tc.number: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0103
 * @tc.name: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0103
 * @tc.desc: 测试PathHasEl3OrEl4 包含el3
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_PathHasEl3OrEl4_Test_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_PathHasEl3OrEl4_Test_0103";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        const std::set<std::string> includes = {
            "/data/storage/el3/"
        };
        const std::vector<std::string> excludes = {
            "/data/storage/el3/base/"
        };
        extExtension->PathHasEl3OrEl4(includes, excludes);
        EXPECT_TRUE(extExtension->appStatistic_->hasEl3OrEl4_.load());
        extExtension->appStatistic_->hasEl3OrEl4_.store(false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_PathHasEl3OrEl4_Test_0103";
}
 
/**
 * @tc.number: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0104
 * @tc.name: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0104
 * @tc.desc: 测试PathHasEl3OrEl4 状态已经为真，再次扫到el4
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_PathHasEl3OrEl4_Test_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_PathHasEl3OrEl4_Test_0104";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        extExtension->appStatistic_->hasEl3OrEl4_.store(true);
        const std::set<std::string> includes = {
            "/data/storage/el4/"
        };
        const std::vector<std::string> excludes = {
            "/data/storage/el4/base/"
        };
        extExtension->PathHasEl3OrEl4(includes, excludes);
        EXPECT_TRUE(extExtension->appStatistic_->hasEl3OrEl4_.load());
        extExtension->appStatistic_->hasEl3OrEl4_.store(false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_PathHasEl3OrEl4_Test_0104";
}
 
/**
 * @tc.number: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0105
 * @tc.name: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0105
 * @tc.desc: 测试PathHasEl3OrEl4 状态已经为真，再次扫到非el3/el4
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_PathHasEl3OrEl4_Test_0105, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_PathHasEl3OrEl4_Test_0105";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        extExtension->appStatistic_->hasEl3OrEl4_.store(true);
        const std::set<std::string> includes = {
            "/data/storage/el2/"
        };
        const std::vector<std::string> excludes = {
            "/data/storage/el2/base/"
        };
        extExtension->PathHasEl3OrEl4(includes, excludes);
        EXPECT_TRUE(extExtension->appStatistic_->hasEl3OrEl4_.load());
        extExtension->appStatistic_->hasEl3OrEl4_.store(false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_PathHasEl3OrEl4_Test_0105";
}
 
/**
 * @tc.number: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0106
 * @tc.name: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0106
 * @tc.desc: 测试PathHasEl3OrEl4 包含整个应用文件的二级目录,只排除了el3
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_PathHasEl3OrEl4_Test_0106, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_PathHasEl3OrEl4_Test_0106";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        const std::set<std::string> includes = {
            "/data/storage/"
        };
        const std::vector<std::string> excludes = {
            "/data/storage/el2/",
            "/data/storage/el3/"
        };
        extExtension->PathHasEl3OrEl4(includes, excludes);
        EXPECT_TRUE(extExtension->appStatistic_->hasEl3OrEl4_.load());
        extExtension->appStatistic_->hasEl3OrEl4_.store(false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_PathHasEl3OrEl4_Test_0106";
}
 
/**
 * @tc.number: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0107
 * @tc.name: Ext_Extension_Sub_PathHasEl3OrEl4_Test_0107
 * @tc.desc: 测试PathHasEl3OrEl4 应用自定义的次级目录命名为el3
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_PathHasEl3OrEl4_Test_0107, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_PathHasEl3OrEl4_Test_0107";
    try {
        ASSERT_TRUE(extExtension != nullptr);
        const std::set<std::string> includes = {
            "/data/storage/el2/base/el3/",
            "/data/storage/el1/"
        };
        const std::vector<std::string> excludes = {
            "/data/storage/el1/base/"
        };
        extExtension->PathHasEl3OrEl4(includes, excludes);
        EXPECT_FALSE(extExtension->appStatistic_->hasEl3OrEl4_.load());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_PathHasEl3OrEl4_Test_0107";
}
}