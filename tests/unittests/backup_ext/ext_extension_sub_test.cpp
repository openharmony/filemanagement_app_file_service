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

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <fstream>

#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "ext_backup_mock.h"
#include "ext_extension_mock.h"

#include "tar_file.cpp"
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

    extBackupMock = make_shared<ExtBackupMock>();
    ExtBackupMock::extBackup = extBackupMock;

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

    rmDir = string("rm -r ") + BUNDLE_BASE_DIR;
    system(rmDir.c_str());

    rmDir = string("rm -r ") + PATH_BUNDLE_BACKUP_HOME_EL1;
    system(rmDir.c_str());

    rmDir = string("rm -r ") + PATH_BUNDLE_BACKUP_HOME;
    system(rmDir.c_str());

    extension = nullptr;
    extExtension = nullptr;
    ExtBackupMock::extBackup = nullptr;
    extBackupMock = nullptr;
};

/**
 * @tc.number: Ext_Extension_Sub_Test_0001
 * @tc.name: Ext_Extension_Sub_Test_0001
 * @tc.desc: 测试SetClearDataFlag
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionSubTest, Ext_Extension_Sub_Test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_Sub_Test_0001";
    try {
        EXPECT_TRUE(extExtension != nullptr);
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
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_Test_0001";
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
        std::string extInfo = "test";
        std::string compatibilityInfo = "";
        int32_t scenario = BConstants::ExtensionScenario::BACKUP;
        auto ret = extExtension->HandleGetCompatibilityInfo(extInfo, scenario, compatibilityInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        scenario = BConstants::ExtensionScenario::RESTORE;
        ret = extExtension->HandleGetCompatibilityInfo(extInfo, scenario, compatibilityInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK).GetCode());

        scenario = BConstants::ExtensionScenario::INVALID;
        ret = extExtension->HandleGetCompatibilityInfo(extInfo, scenario, compatibilityInfo);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_INVAL_ARG).GetCode());

        EXPECT_CALL(*extBackupMock, GetBackupCompatibilityInfo(_, _))
            .WillOnce(Return(BError(BError::Codes::EXT_INVAL_ARG).GetCode()));
        ret = extExtension->HandleGetCompatibilityInfo(extInfo, scenario, compatibilityInfo);
        EXPECT_EQ(ret, BError(BError::Codes::EXT_INVAL_ARG).GetCode());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ExtExtensionSubTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_Sub_HandleGetCompatibilityInfo_Test_0100";
}
}