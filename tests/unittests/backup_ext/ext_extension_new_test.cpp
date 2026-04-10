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

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include <gtest/gtest.h>

#include "ext_backup_mock.h"
#include "ext_extension_mock.h"
#include "i_service_mock.h"

#include "library_func_mock.h"
#include "library_func_define.h"
#include "tar_file.cpp"
#include "untar_file.cpp"
#define Persist GetFd
#include "ext_extension.cpp"
#include "sub_ext_extension.cpp"
#include "library_func_undef.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace OHOS::AppFileService;

namespace {
const string BUNDLE_NAME = "com.example.app2backup/";
}

sptr<IService> ServiceClient::GetInstance()
{
    return serviceProxy_;
}

class ExtExtensionNewTest : public testing::Test {
public:
    //所有测试用例执行之前执行
    static void SetUpTestCase(void);
    //所有测试用例执行之后执行
    static void TearDownTestCase(void);
    void SetUp() override
    {
        funcMock_ = make_shared<LibraryFuncMock>();
        LibraryFuncMock::libraryFunc_ = funcMock_;
        serviceMock_ = new IServiceMock();
        ServiceClient::serviceProxy_ = serviceMock_;

        std::queue<std::shared_ptr<IFileInfo>> emptyQueue;
        ScanFileSingleton::GetInstance().pendingFileQueue_.swap(emptyQueue);
        ScanFileSingleton::GetInstance().smallFiles_.clear();
        extBackupMock_ = make_shared<ExtBackupMock>();
        ExtBackupMock::extBackup = extBackupMock_;  
    };
    void TearDown() override
    {
        LibraryFuncMock::libraryFunc_ = nullptr;
        funcMock_ = nullptr;
        ServiceClient::serviceProxy_ = nullptr;
        serviceMock_ = nullptr;
        ExtBackupMock::extBackup = nullptr;
        extBackupMock_ = nullptr;
    };
    static inline sptr<BackupExtExtension> extExtension_ = nullptr;
    static inline shared_ptr<ExtBackup> extension_ = nullptr;
    static inline shared_ptr<ExtBackupMock> extBackupMock_ = nullptr;
    static inline shared_ptr<LibraryFuncMock> funcMock_ = nullptr;
    static inline sptr<IServiceMock> serviceMock_ = nullptr;
};

void ExtExtensionNewTest::SetUpTestCase(void)
{
    funcMock_ = make_shared<LibraryFuncMock>();
    LibraryFuncMock::libraryFunc_ = funcMock_;
    serviceMock_ = new IServiceMock();
    ServiceClient::serviceProxy_ = serviceMock_;

    extExtension_ = sptr<BackupExtExtension>(new BackupExtExtension(nullptr, "test.example.com"));
    extension_ = make_shared<ExtBackup>();
    extExtension_->extension_ = extension_;
};

void ExtExtensionNewTest::TearDownTestCase(void)
{
    LibraryFuncMock::libraryFunc_ = nullptr;
    funcMock_ = nullptr;
    ServiceClient::serviceProxy_ = nullptr;
    serviceMock_ = nullptr;

    extension_ = nullptr;
    extExtension_ = nullptr;
};

/**
 * @tc.number: Ext_Extension_ReportNormalAppFileReady_Test_0100
 * @tc.name: Ext_Extension_ReportNormalAppFileReady_Test_0100
 * @tc.desc: 测试ReportNormalAppFileReady open失败且为错误码为没权限场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportNormalAppFileReady_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_ReportNormalAppFileReady_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    errno = ERR_NO_PERMISSION;
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(-1));
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    EXPECT_EQ(extExtension_->ReportNormalAppFileReady(filename, filePath, false), 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_ReportNormalAppFileReady_Test_0100";
}

/**
 * @tc.number: Ext_Extension_ReportNormalAppFileReady_Test_0200
 * @tc.name: Ext_Extension_ReportNormalAppFileReady_Test_0200
 * @tc.desc: 测试ReportNormalAppFileReady proxy为空场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportNormalAppFileReady_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_ReportNormalAppFileReady_Test_0200";
    ASSERT_TRUE(extExtension_ != nullptr);
    errno = -1;
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(-1));
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    ServiceClient::serviceProxy_ = nullptr;
    EXPECT_EQ(extExtension_->ReportNormalAppFileReady(filename, filePath, false),
        static_cast<int32_t>(BError::Codes::EXT_CLIENT_IS_NULL));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_ReportNormalAppFileReady_Test_0200";
}

/**
 * @tc.number: Ext_Extension_ReportNormalAppFileReady_Test_0300
 * @tc.name: Ext_Extension_ReportNormalAppFileReady_Test_0300
 * @tc.desc: 测试ReportNormalAppFileReady AppFileReadyWithoutFd/AppFileReady返回-1场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportNormalAppFileReady_Test_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_ReportNormalAppFileReady_Test_0300";
    ASSERT_TRUE(extExtension_ != nullptr);
    errno = -1;
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(-1)).WillOnce(Return(1));
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).WillOnce(Return(0));
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    EXPECT_CALL(*serviceMock_, AppFileReadyWithoutFd(_, _)).WillOnce(Return(-1));
    EXPECT_EQ(extExtension_->ReportNormalAppFileReady(filename, filePath, false), -1);
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillOnce(Return(-2));
    EXPECT_EQ(extExtension_->ReportNormalAppFileReady(filename, filePath, false), -2);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_ReportNormalAppFileReady_Test_0300";
}

/**
 * @tc.number: Ext_Extension_ReportNormalAppFileReady_Test_0400
 * @tc.name: Ext_Extension_ReportNormalAppFileReady_Test_0400
 * @tc.desc: 测试ReportNormalAppFileReady 成功场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportNormalAppFileReady_Test_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_ReportNormalAppFileReady_Test_0400";
    ASSERT_TRUE(extExtension_ != nullptr);
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(1));
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).WillOnce(Return(0));
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillOnce(Return(0));
    EXPECT_EQ(extExtension_->ReportNormalAppFileReady(filename, filePath, true), 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_ReportNormalAppFileReady_Test_0400";
}

/**
 * @tc.number: Ext_Extension_IndexFileReady_Test_0100
 * @tc.name: Ext_Extension_IndexFileReady_Test_0100
 * @tc.desc: 测试IndexFileReady
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_IndexFileReady_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_IndexFileReady_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce([](const char* path, int mode) -> int {
        errno = EPERM;
        return -1;
    });
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    struct stat sta;
    shared_ptr<IFileInfo> file1 = make_shared<FileInfo>(filename, filePath, sta, false);
    vector<shared_ptr<IFileInfo>> files = {file1};
    EXPECT_EQ(extExtension_->IndexFileReady(files), BError::BackupErrorCode::E_PERM);

    errno = EPERM;
    EXPECT_CALL(*funcMock_, open(_, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(*funcMock_, close(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, fstat(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, lseek(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, read(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, stat(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_THROW(extExtension_->IndexFileReady(files), BError);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_IndexFileReady_Test_0100";
}

/**
 * @tc.number: Ext_Extension_DoPacketOnce_Test_0100
 * @tc.name: Ext_Extension_DoPacketOnce_Test_0100
 * @tc.desc: 测试DoPacketOnce失败场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DoPacketOnce_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DoPacketOnce_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    EXPECT_CALL(*funcMock_, close(_)).WillRepeatedly(Return(0));
    FILE * tmpFile = tmpfile();
    EXPECT_CALL(*funcMock_, fopen(_, _)).WillOnce(Return(tmpFile));
    string path = "/tmp";
    std::vector<std::shared_ptr<ISmallFileInfo>> srcFiles;
    uint64_t totalTarSpend = 0;
    auto reportCb = [](std::string path, int err) {
        return;
    };
    extExtension_->DoPacketOnce(srcFiles, path, reportCb, totalTarSpend);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 0);
    fclose(tmpFile);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DoPacketOnce_Test_0100";
}

/**
 * @tc.number: Ext_Extension_DoPacketOnce_Test_0200
 * @tc.name: Ext_Extension_DoPacketOnce_Test_0200
 * @tc.desc: 测试DoPacketOnce成功场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DoPacketOnce_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DoPacketOnce_Test_0200";
    ASSERT_TRUE(extExtension_ != nullptr);
    string filename = "app_file_ready_test";
    size_t fileSize = 100;
    string path = "/tmp";
    shared_ptr<ISmallFileInfo> file1 = make_shared<SmallFileInfo>(filename, fileSize);
    std::vector<std::shared_ptr<ISmallFileInfo>> srcFiles = {file1};
    FILE * tmpFile = tmpfile();
    EXPECT_CALL(*funcMock_, fopen(_, _)).WillOnce(Return(tmpFile));
    EXPECT_CALL(*funcMock_, open(_, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(*funcMock_, close(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, ferror(_)).WillRepeatedly(Return(0));
    struct passwd pw;
    EXPECT_CALL(*funcMock_, getpwuid(_)).WillRepeatedly(Return(&pw));
    struct group gr;
    EXPECT_CALL(*funcMock_, getgrgid(_)).WillRepeatedly(Return(&gr));
    EXPECT_CALL(*funcMock_, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, lstat(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, read(_, _, _)).WillRepeatedly(Return(BLOCK_SIZE));
    EXPECT_CALL(*funcMock_, fwrite(_, _, _, _)).WillRepeatedly(Return(BLOCK_SIZE));
    uint64_t totalTarSpend = 0;
    auto reportCb = [](std::string path, int err) {
        return;
    };
    extExtension_->DoPacketOnce(srcFiles, path, reportCb, totalTarSpend);
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DoPacketOnce_Test_0200";
}

/**
 * @tc.number: Ext_Extension_DoPacket_Test_0200
 * @tc.name: Ext_Extension_DoPacket_Test_0200
 * @tc.desc: 测试DoPacket成功场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DoPacket_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DoPacket_Test_0200";
    ASSERT_TRUE(extExtension_ != nullptr);
    FILE * tmpFile = tmpfile();
    EXPECT_CALL(*funcMock_, fopen(_, _)).WillRepeatedly(Return(tmpFile));
    EXPECT_CALL(*funcMock_, open(_, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(*funcMock_, close(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, ferror(_)).WillRepeatedly(Return(0));
    struct passwd pw;
    EXPECT_CALL(*funcMock_, getpwuid(_)).WillRepeatedly(Return(&pw));
    struct group gr;
    EXPECT_CALL(*funcMock_, getgrgid(_)).WillRepeatedly(Return(&gr));
    EXPECT_CALL(*funcMock_, access(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, lstat(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, read(_, _, _)).WillRepeatedly(Return(BLOCK_SIZE));
    EXPECT_CALL(*funcMock_, fwrite(_, _, _, _)).WillRepeatedly(Return(BLOCK_SIZE));
    EXPECT_CALL(*serviceMock_, StartAncoPacket(_)).WillRepeatedly(Return(ERR_OK));
    string filename1 = "app_file_ready_test1";
    size_t fileSize1 = BConstants::DEFAULT_SLICE_SIZE;
    shared_ptr<ISmallFileInfo> file1 = make_shared<SmallFileInfo>(filename1, fileSize1);
    ScanFileSingleton::GetInstance().smallFiles_.push_back(file1);
    extExtension_->DoPacket();
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 1);

    string filename2 = "app_file_ready_test2";
    size_t fileSize2 = 10;
    shared_ptr<ISmallFileInfo> file2 = make_shared<SmallFileInfo>(filename2, fileSize2);
    ScanFileSingleton::GetInstance().smallFiles_.push_back(file1);
    ScanFileSingleton::GetInstance().smallFiles_.push_back(file2);
    extExtension_->DoPacket();
    EXPECT_EQ(ScanFileSingleton::GetInstance().pendingFileQueue_.size(), 3);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DoPacket_Test_0200";
}

/**
 * @tc.number: Ext_Extension_ScanAllDirs_Test_0100
 * @tc.name: Ext_Extension_ScanAllDirs_Test_0100
 * @tc.desc: 测试ScanAllDirs
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ScanAllDirs_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_ScanAllDirs_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    int64_t totalSize = 0;
    string config = "";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(config);
    auto cache = cachedEntity.Structuralize();

    errno = EPERM;
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillOnce(Return(-1));
    EXPECT_CALL(*serviceMock_, StartAncoScanAllDirs(_)).WillRepeatedly(Return(ERR_OK));
    EXPECT_EQ(extExtension_->ScanAllDirs(cache, totalSize), BError::BackupErrorCode::E_PERM);
    errno = EEXIST;
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillOnce(Return(-1)).WillRepeatedly(Return(0));
    EXPECT_EQ(extExtension_->ScanAllDirs(cache, totalSize), 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_ScanAllDirs_Test_0100";
}

/**
 * @tc.number: Ext_Extension_OnBackupExCallback_Test_0100
 * @tc.name: Ext_Extension_OnBackupExCallback_Test_0100
 * @tc.desc: 测试OnBackupExCallback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_OnBackupExCallback_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_OnBackupExCallback_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    auto obj = wptr<BackupExtExtension>(extExtension_);
    ErrCode errCode = BError(BError::Codes::OK).GetCode();
    string errMsg1 = "";
    extExtension_->OnBackupExCallback(obj)(errCode, errMsg1);
    EXPECT_EQ(extExtension_->compatibleDirs_.size(), 0);

    string errMsg2 = "{\"" + BConstants::COMPATIBLE_DIR_MAPPING + "\": [\"path1\", \"path2\"]}";
    extExtension_->OnBackupExCallback(obj)(errCode, errMsg2);
    EXPECT_EQ(extExtension_->compatibleDirs_.size(), 2);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_OnBackupExCallback_Test_0100";
}

/**
 * @tc.number: Ext_Extension_DoBackupTask_Test_0100
 * @tc.name: Ext_Extension_DoBackupTask_Test_0100
 * @tc.desc: 测试DoBackupTask open失败场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DoBackupTask_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DoBackupTask_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    struct stat sta;
    shared_ptr<IFileInfo> file1 = make_shared<FileInfo>(filename, filePath, sta, false);
    ScanFileSingleton::GetInstance().pendingFileQueue_.push(file1);
    ScanFileSingleton::GetInstance().SetCompletedFlag(true);
    EXPECT_CALL(*funcMock_, open(_, _)).WillRepeatedly([](const char* path, int mode) -> int {
        errno = EPERM;
        return -1;
    });
    int ret = 0;
    EXPECT_CALL(*serviceMock_, AppDone(_)).WillRepeatedly([&ret](int err) -> int {
        ret = err;
        return 0;
    });
    GTEST_LOG_(INFO) << "1. test open fail";
    extExtension_->DoBackupTask();
    EXPECT_EQ(ret, static_cast<int>(BError::Codes::EXT_REPORT_FILE_READY_FAIL));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DoBackupTask_Test_0100";
}

/**
 * @tc.number: Ext_Extension_DoBackupTask_Test_0200
 * @tc.name: Ext_Extension_DoBackupTask_Test_0200
 * @tc.desc: 测试DoBackupTask 正常场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DoBackupTask_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DoBackupTask_Test_0200";
    ASSERT_TRUE(extExtension_ != nullptr);
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    struct stat sta;
    shared_ptr<IFileInfo> file1 = make_shared<FileInfo>(filename, filePath, sta, false);
    shared_ptr<IFileInfo> file2 = make_shared<FileInfo>(filename, filePath, sta, true);
    ScanFileSingleton::GetInstance().pendingFileQueue_.push(file1);
    ScanFileSingleton::GetInstance().pendingFileQueue_.push(file2);
    ScanFileSingleton::GetInstance().pendingFileQueue_.push(nullptr);
    ScanFileSingleton::GetInstance().SetCompletedFlag(true);
    int ret = 0;
    EXPECT_CALL(*serviceMock_, AppDone(_)).WillRepeatedly([&ret](int err) -> int {
        ret = err;
        return 0;
    });
    EXPECT_CALL(*funcMock_, open(_, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(*funcMock_, close(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, fstat(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, lseek(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, read(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, stat(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillRepeatedly(
        [](const std::string &fileName, int fd, int32_t errCode) -> int {
            ScanFileSingleton::GetInstance().SetCompletedFlag(true);
            return 0;
    });
    GTEST_LOG_(INFO) << "2. test ok";
    extExtension_->DoBackupTask();
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DoBackupTask_Test_0200";
}

/**
 * @tc.number: Ext_Extension_DoBackupTask_Test_0300
 * @tc.name: Ext_Extension_DoBackupTask_Test_0300
 * @tc.desc: 测试DoBackupTask open权限场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DoBackupTask_Test_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DoBackupTask_Test_0300";
    ASSERT_TRUE(extExtension_ != nullptr);
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    struct stat sta;
    shared_ptr<IFileInfo> file1 = make_shared<FileInfo>(filename, filePath, sta, false);
    shared_ptr<IFileInfo> file2 = make_shared<FileInfo>(filename, filePath, sta, true);
    ScanFileSingleton::GetInstance().pendingFileQueue_.push(file1);
    ScanFileSingleton::GetInstance().pendingFileQueue_.push(file2);
    ScanFileSingleton::GetInstance().pendingFileQueue_.push(nullptr);
    ScanFileSingleton::GetInstance().SetCompletedFlag(true);
    int ret = 0;
    EXPECT_CALL(*funcMock_, open(_, _)).WillRepeatedly([](const char* path, int mode) -> int {
        errno = ERR_NO_PERMISSION;
        return -1;
    });
    EXPECT_CALL(*serviceMock_, AppDone(_)).WillRepeatedly([&ret](int err) -> int {
        ret = err;
        return 0;
    });
    GTEST_LOG_(INFO) << "3. test open no permission";
    extExtension_->DoBackupTask();
    EXPECT_EQ(ret, static_cast<int>(BError::Codes::EXT_REPORT_FILE_READY_FAIL));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DoBackupTask_Test_0300";
}

/**
 * @tc.number: Ext_Extension_DivideIncludesByCompatInfo_Test_0100
 * @tc.name: Ext_Extension_DivideIncludesByCompatInfo_Test_0100
 * @tc.desc: 测试 DivideIncludesByCompatInfo dirMapping为空场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DivideIncludesByCompatInfo_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DivideIncludesByCompatInfo_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    string config = "";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(config);
    auto cache = cachedEntity.Structuralize();
    vector<string> includes = {};
    extExtension_->compatibleDirs_ = {"/p1/f1", "/p2"};
    EXPECT_EQ(extExtension_->DivideIncludesByCompatInfo(includes, cache).size(), 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DivideIncludesByCompatInfo_Test_0100";
}

/**
 * @tc.number: Ext_Extension_DivideIncludesByCompatInfo_Test_0200
 * @tc.name: Ext_Extension_DivideIncludesByCompatInfo_Test_0200
 * @tc.desc: 测试 DivideIncludesByCompatInfo compatibleDirs_为空场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DivideIncludesByCompatInfo_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DivideIncludesByCompatInfo_Test_0200";
    ASSERT_TRUE(extExtension_ != nullptr);
    string_view sv1 = R"({"compatibleDirMapping": [{"backupDir": "123", "restoreDir": "456"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv1);
    auto cache = cachedEntity.Structuralize();
    vector<string> includes = {};
    extExtension_->compatibleDirs_ = {};
    EXPECT_EQ(extExtension_->DivideIncludesByCompatInfo(includes, cache).size(), 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DivideIncludesByCompatInfo_Test_0200";
}

/**
 * @tc.number: Ext_Extension_DivideIncludesByCompatInfo_Test_300
 * @tc.name: Ext_Extension_DivideIncludesByCompatInfo_Test_300
 * @tc.desc: 测试 DivideIncludesByCompatInfo includes为空场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DivideIncludesByCompatInfo_Test_300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DivideIncludesByCompatInfo_Test_300";
    ASSERT_TRUE(extExtension_ != nullptr);
    string_view sv1 = R"({"compatibleDirMapping": [{"backupDir": "/bak/p1", "restoreDir": "/p1"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv1);
    auto cache = cachedEntity.Structuralize();
    vector<string> includes = {};
    extExtension_->compatibleDirs_ = {"/p2", "/p3"};
    EXPECT_EQ(extExtension_->DivideIncludesByCompatInfo(includes, cache).size(), 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DivideIncludesByCompatInfo_Test_300";
}

/**
 * @tc.number: Ext_Extension_DivideIncludesByCompatInfo_Test_0400
 * @tc.name: Ext_Extension_DivideIncludesByCompatInfo_Test_0400
 * @tc.desc: 测试 DivideIncludesByCompatInfo enabledCompatDirs_为空场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DivideIncludesByCompatInfo_Test_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DivideIncludesByCompatInfo_Test_0400";
    ASSERT_TRUE(extExtension_ != nullptr);
    string_view sv1 = R"({"compatibleDirMapping": [{"backupDir": "/bak/p1", "restoreDir": "/p1"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv1);
    auto cache = cachedEntity.Structuralize();
    vector<string> includes = {"/p1", "/p4"};
    extExtension_->compatibleDirs_ = {"/p2", "/p3"};
    EXPECT_EQ(extExtension_->DivideIncludesByCompatInfo(includes, cache).size(), 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DivideIncludesByCompatInfo_Test_0400";
}

/**
 * @tc.number: Ext_Extension_DivideIncludesByCompatInfo_Test_0500
 * @tc.name: Ext_Extension_DivideIncludesByCompatInfo_Test_0500
 * @tc.desc: 测试 DivideIncludesByCompatInfo 成功场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DivideIncludesByCompatInfo_Test_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_DivideIncludesByCompatInfo_Test_0500";
    ASSERT_TRUE(extExtension_ != nullptr);
    string_view sv1 = R"({"compatibleDirMapping": [{"backupDir": "/bak/p1", "restoreDir": "/p1"}]})";
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(sv1);
    auto cache = cachedEntity.Structuralize();
    vector<string> includes = {"/p1", "/p4"};
    extExtension_->compatibleDirs_ = {"/p1", "/p3"};
    EXPECT_EQ(extExtension_->DivideIncludesByCompatInfo(includes, cache).size(), 1);
    EXPECT_EQ(includes.size(), 1);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DivideIncludesByCompatInfo_Test_0500";
}

/**
 * @tc.number: Ext_Extension_OpenFileWithFDSan_Test_0100
 * @tc.name: Ext_Extension_OpenFileWithFDSan_Test_0100
 * @tc.desc: 测试OpenFileWithFDSan成功场景（fd >= 0）
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_OpenFileWithFDSan_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_OpenFileWithFDSan_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(5));
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    string testPath = "/data/test/file.txt";
    int fd = extExtension_->OpenFileWithFDSan(testPath);
    EXPECT_EQ(fd, 5);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_OpenFileWithFDSan_Test_0100";
}

/**
 * @tc.number: Ext_Extension_OpenFileWithFDSan_Test_0200
 * @tc.name: Ext_Extension_OpenFileWithFDSan_Test_0200
 * @tc.desc: 测试OpenFileWithFDSan失败场景（fd < 0）
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_OpenFileWithFDSan_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_OpenFileWithFDSan_Test_0200";
    ASSERT_TRUE(extExtension_ != nullptr);
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(-1));
    string testPath = "/data/test/nonexistent.txt";
    int fd = extExtension_->OpenFileWithFDSan(testPath);
    EXPECT_LT(fd, 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_OpenFileWithFDSan_Test_0200";
}

/**
 * @tc.number: Ext_Extension_CloseFileWithFDSan_Test_0100
 * @tc.name: Ext_Extension_CloseFileWithFDSan_Test_0100
 * @tc.desc: 测试CloseFileWithFDSan有效fd场景（fd >= 0）
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_CloseFileWithFDSan_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_CloseFileWithFDSan_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).WillOnce(Return(0));
    int validFd = 5;
    extExtension_->CloseFileWithFDSan(validFd);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_CloseFileWithFDSan_Test_0100";
}

/**
 * @tc.number: Ext_Extension_CloseFileWithFDSan_Test_0200
 * @tc.name: Ext_Extension_CloseFileWithFDSan_Test_0200
 * @tc.desc: 测试CloseFileWithFDSan无效fd场景（fd < 0）
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_CloseFileWithFDSan_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_CloseFileWithFDSan_Test_0200";
    ASSERT_TRUE(extExtension_ != nullptr);
    // 当fd < 0时，不应该调用fdsan_close_with_tag
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).Times(0);
    int invalidFd = -1;
    extExtension_->CloseFileWithFDSan(invalidFd);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_CloseFileWithFDSan_Test_0200";
}

/**
 * @tc.number: Ext_Extension_GetRestoreTempPath_Test_0000
 * @tc.name: Ext_Extension_GetRestoreTempPath_Test_0000
 * @tc.desc: 测试 GetRestoreTempPath
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_GetRestoreTempPath_Test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_GetRestoreTempPath_Test_0000";
    std::string bundleName = BConstants::BUNDLE_MEDIAL_DATA;
    auto ret = GetRestoreTempPath(bundleName);
    EXPECT_EQ(ret, string(BConstants::PATH_MEDIALDATA_BACKUP_HOME)
        .append(BConstants::SA_BUNDLE_BACKUP_RESTORE));

    bundleName = "test";
    ret = GetRestoreTempPath(bundleName);
    EXPECT_EQ(ret, string(BConstants::PATH_BUNDLE_BACKUP_HOME)
        .append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_GetRestoreTempPath_Test_0000";
}

/**
 * @tc.number: Ext_Extension_GetRestoreTempPath_Test_0001
 * @tc.name: Ext_Extension_GetRestoreTempPath_Test_0001
 * @tc.desc: 测试 GetRestoreTempPath
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_GetRestoreTempPath_Test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_GetRestoreTempPath_Test_0001";
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillRepeatedly(Return(ERR_OK));

    std::string hashName = "file_anco";
    std::string hashName2 = "file.txt";
    EXPECT_EQ(GetRestoreTempPath(BConstants::BUNDLE_FILE_MANAGER, hashName),
        string(BConstants::PATH_FILEMANAGE_BACKUP_HOME_ANCO).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
    EXPECT_EQ(GetRestoreTempPath(BConstants::BUNDLE_FILE_MANAGER, hashName2),
        string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
    EXPECT_EQ(GetRestoreTempPath(BConstants::BUNDLE_FILE_MANAGER, hashName2),
        string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_GetRestoreTempPath_Test_0001";
}

/**
 * @tc.number: Ext_Extension_GetRestoreTempPath_Test_0002
 * @tc.name: Ext_Extension_GetRestoreTempPath_Test_0002
 * @tc.desc: 测试 GetRestoreTempPath
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_GetRestoreTempPath_Test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_GetRestoreTempPath_Test_0002";
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillRepeatedly(Invoke([&](const char *pathname, mode_t mode) {
        errno = EEXIST;
        return -1;
    }));

    std::string hashName = "file_anco";
    std::string hashName2 = "file.txt";
    EXPECT_EQ(GetRestoreTempPath(BConstants::BUNDLE_FILE_MANAGER, hashName),
        string(BConstants::PATH_FILEMANAGE_BACKUP_HOME_ANCO).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
    EXPECT_EQ(GetRestoreTempPath(BConstants::BUNDLE_FILE_MANAGER, hashName2),
        string(BConstants::PATH_FILEMANAGE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE));
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_GetRestoreTempPath_Test_0002";
}

/**
 * @tc.number: Ext_Extension_GetIncrementalFileHandlePath_Test_0000
 * @tc.name: Ext_Extension_GetIncrementalFileHandlePath_Test_0000
 * @tc.desc: 测试 GetIncrementalFileHandlePath
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_GetIncrementalFileHandlePath_Test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_GetIncrementalFileHandlePath_Test_0000";
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillRepeatedly(Return(ERR_OK));

    const string fileName = "1.txt";
    const string fileName2 = "test_anco";
    string tarName = "2.tar";
    EXPECT_EQ(GetIncrementalFileHandlePath(fileName, BUNDLE_NAME, tarName), ERR_OK);
    EXPECT_EQ(GetIncrementalFileHandlePath(fileName, BConstants::BUNDLE_FILE_MANAGER, tarName), ERR_OK);
    EXPECT_EQ(GetIncrementalFileHandlePath(fileName2, BConstants::BUNDLE_FILE_MANAGER, tarName), ERR_OK);
    EXPECT_EQ(GetIncrementalFileHandlePath(fileName, BConstants::BUNDLE_MEDIAL_DATA, tarName), ERR_OK);
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_GetIncrementalFileHandlePath_Test_0000";
}

/**
 * @tc.number: Ext_Extension_GetIncrementalFileHandlePath_Test_0001
 * @tc.name: Ext_Extension_GetIncrementalFileHandlePath_Test_0001
 * @tc.desc: 测试 GetIncrementalFileHandlePath - FAILURE
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_GetIncrementalFileHandlePath_Test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_GetIncrementalFileHandlePath_Test_0001";
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillRepeatedly(Invoke([&](const char *pathname, mode_t mode) {
        errno = ENOENT;
        return -1;
    }));

    const string fileName = "1.txt";
    const string fileName2 = "test_anco";
    string tarName = "2.tar";
    EXPECT_NE(GetIncrementalFileHandlePath(fileName, BUNDLE_NAME, tarName), ERR_OK);
    EXPECT_NE(GetIncrementalFileHandlePath(fileName, BConstants::BUNDLE_FILE_MANAGER, tarName), ERR_OK);
    EXPECT_NE(GetIncrementalFileHandlePath(fileName2, BConstants::BUNDLE_FILE_MANAGER, tarName), ERR_OK);
    EXPECT_NE(GetIncrementalFileHandlePath(fileName, BConstants::BUNDLE_MEDIAL_DATA, tarName), ERR_OK);
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_GetIncrementalFileHandlePath_Test_0001";
}

/**
 * @tc.number: Ext_Extension_GetIncrementalFileHandlePath_Test_0002
 * @tc.name: Ext_Extension_GetIncrementalFileHandlePath_Test_0002
 * @tc.desc: 测试 GetIncrementalFileHandlePath - SUCCESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_GetIncrementalFileHandlePath_Test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_GetIncrementalFileHandlePath_Test_0002";
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillRepeatedly(Invoke([&](const char *pathname, mode_t mode) {
        errno = EEXIST;
        return -1;
    }));

    const string fileName = "1.txt";
    const string fileName2 = "test_anco";
    string tarName = "2.tar";
    EXPECT_EQ(GetIncrementalFileHandlePath(fileName, BUNDLE_NAME, tarName), ERR_OK);
    EXPECT_EQ(GetIncrementalFileHandlePath(fileName, BConstants::BUNDLE_FILE_MANAGER, tarName), ERR_OK);
    EXPECT_EQ(GetIncrementalFileHandlePath(fileName2, BConstants::BUNDLE_FILE_MANAGER, tarName), ERR_OK);
    EXPECT_EQ(GetIncrementalFileHandlePath(fileName, BConstants::BUNDLE_MEDIAL_DATA, tarName), ERR_OK);
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_GetIncrementalFileHandlePath_Test_0002";
}

/**
 * @tc.number: Ext_Extension_ReportAppFileReady_Test_0000
 * @tc.name: Ext_Extension_ReportAppFileReady_Test_0000
 * @tc.desc: 测试 ReportAppFileReady - 普通小文件
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportAppFileReady_Test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_ReportAppFileReady_Test_0000";
    struct stat sta;
    auto fileInfo = make_shared<FileInfo>("", "", sta, false);
    int fdNum = 0;

    // success
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(100));
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock_, RemoveFile(_)).WillOnce(Return(true));

    EXPECT_TRUE(SUCCEEDED(extExtension_->ReportAppFileReady(fileInfo, fdNum)));

    // fail
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Invoke([&](const char *filename, int flags, ...) {
        errno = ERR_NO_PERMISSION;
        return -1;
    }));
    EXPECT_EQ(extExtension_->ReportAppFileReady(fileInfo, fdNum), ERR_NO_PERMISSION);

    // fail
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Invoke([&](const char *filename, int flags, ...) {
        errno = ERR_NO_PERMISSION + 1;
        return -1;
    }));
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    EXPECT_CALL(*serviceMock_, AppFileReadyWithoutFd(_, _)).WillOnce(Return(-1));
    EXPECT_FALSE(SUCCEEDED(extExtension_->ReportAppFileReady(fileInfo, fdNum)));

    // fail
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(100));
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    ServiceClient::serviceProxy_ = nullptr;
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).WillOnce(Return(0));
    EXPECT_EQ(extExtension_->ReportAppFileReady(fileInfo, fdNum),
        static_cast<int32_t>(BError::Codes::EXT_CLIENT_IS_NULL));
    ServiceClient::serviceProxy_ = serviceMock_;

    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_ReportAppFileReady_Test_0000";
}

/**
 * @tc.number: Ext_Extension_ReportAppFileReady_Test_0001
 * @tc.name: Ext_Extension_ReportAppFileReady_Test_0001
 * @tc.desc: 测试 ReportAppFileReady - TRUE - 普通大文件
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportAppFileReady_Test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_ReportAppFileReady_Test_0001";
    struct stat sta;
    auto fileInfo = make_shared<CompatibleFileInfo>("", "", sta, true, "");
    int fdNum = 0;

    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(100));
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).WillOnce(Return(0));

    EXPECT_TRUE(SUCCEEDED(extExtension_->ReportAppFileReady(fileInfo, fdNum)));
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_ReportAppFileReady_Test_0001";
}

/**
 * @tc.number: Ext_Extension_ReportAppFileReady_Test_0002
 * @tc.name: Ext_Extension_ReportAppFileReady_Test_0002
 * @tc.desc: 测试 ReportAppFileReady - 特殊小文件
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportAppFileReady_Test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_ReportAppFileReady_Test_0002";
    struct stat sta;
    auto fileInfo = make_shared<AncoFileInfo>("", "", sta, false, UniqueFd(100));
    auto fileInfo2 = make_shared<AncoFileInfo>("", "", sta, false, UniqueFd(-1));
    int fdNum = 0;

    // success
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock_, RemoveFile(_)).WillOnce(Return(true));

    EXPECT_TRUE(SUCCEEDED(extExtension_->ReportAppFileReady(fileInfo, fdNum)));

    // fail
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    EXPECT_CALL(*serviceMock_, AppFileReadyWithoutFd(_, _)).WillOnce(Return(-1));
    EXPECT_FALSE(SUCCEEDED(extExtension_->ReportAppFileReady(fileInfo2, fdNum)));

    // fail
    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    ServiceClient::serviceProxy_ = nullptr;
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).WillOnce(Return(0));
    EXPECT_EQ(extExtension_->ReportAppFileReady(fileInfo, fdNum),
        static_cast<int32_t>(BError::Codes::EXT_CLIENT_IS_NULL));
    ServiceClient::serviceProxy_ = serviceMock_;
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_ReportAppFileReady_Test_0002";
}

/**
 * @tc.number: Ext_Extension_ReportAppFileReady_Test_0003
 * @tc.name: Ext_Extension_ReportAppFileReady_Test_0003
 * @tc.desc: 测试 ReportAppFileReady - TRUE - 特殊大文件
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportAppFileReady_Test_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_ReportAppFileReady_Test_0003";
    struct stat sta;
    auto fileInfo = make_shared<AncoCompatibleFileInfo>("", "", sta, true, "", UniqueFd(100));
    int fdNum = 0;

    EXPECT_CALL(*funcMock_, fdsan_exchange_owner_tag(_, _, _)).WillOnce(Return());
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock_, fdsan_close_with_tag(_, _)).WillOnce(Return(0));

    EXPECT_TRUE(SUCCEEDED(extExtension_->ReportAppFileReady(fileInfo, fdNum)));
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_ReportAppFileReady_Test_0003";
}

/**
 * @tc.number: Ext_Extension_DoIncrementalRestore_Test_0000
 * @tc.name: Ext_Extension_DoIncrementalRestore_Test_0000
 * @tc.desc: 测试 DoIncrementalRestore
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_DoIncrementalRestore_Test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_DoIncrementalRestore_Test_0000";
    auto oldBundleName = extExtension_->bundleName_;
    extExtension_->bundleName_ = BConstants::BUNDLE_FILE_MANAGER;
    EXPECT_CALL(*funcMock_, open(_, _)).WillRepeatedly(Return(-1));
    EXPECT_EQ(extExtension_->DoIncrementalRestore(), ERR_OK);
    extExtension_->bundleName_ = oldBundleName;
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_DoIncrementalRestore_Test_0000";
}

/**
 * @tc.number: Ext_Extension_ProcessTarFile_Test_0000
 * @tc.name: Ext_Extension_ProcessTarFile_Test_0000
 * @tc.desc: 测试 ProcessTarFile - SUCEESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ProcessTarFile_Test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_ProcessTarFile_Test_0000";
    auto oldParam = std::make_tuple(extExtension_->bundleName_, extExtension_->isDebug_);
    extExtension_->bundleName_ = BConstants::BUNDLE_FILE_MANAGER;
    extExtension_->isDebug_ = true;

    std::string item;
    std::vector<ExtManageInfo> extManageInfo(1);
    std::tuple<std::vector<string>, std::vector<int64_t>, std::vector<string>> ancoTarInfo;
    std::string tempPath;
    item = "1.tar";
    extManageInfo.back().hashName = "1.tar";
    extManageInfo.back().isUserTar = false;
    auto backInput = std::make_tuple(item, extManageInfo, ancoTarInfo, tempPath);

    // -------------------------- if - false
    // 1
    std::tie(item, extManageInfo, ancoTarInfo, tempPath) = backInput;
    item = "1.txt";
    EXPECT_EQ(extExtension_->ProcessTarFile(item, extManageInfo, ancoTarInfo, tempPath), ERR_OK);
    // 2
    std::tie(item, extManageInfo, ancoTarInfo, tempPath) = backInput;
    extManageInfo.back().isUserTar = true;
    EXPECT_EQ(extExtension_->ProcessTarFile(item, extManageInfo, ancoTarInfo, tempPath), ERR_OK);

    // -------------------------- if - true
    // 3
    std::tie(item, extManageInfo, ancoTarInfo, tempPath) = backInput;
    EXPECT_CALL(*extBackupMock_, GetExtensionAction()).WillOnce(Return(BConstants::ExtensionAction::BACKUP));
    EXPECT_EQ(extExtension_->ProcessTarFile(item, extManageInfo, ancoTarInfo, tempPath), EPERM);

    EXPECT_CALL(*extBackupMock_, GetExtensionAction()).WillRepeatedly(Return(BConstants::ExtensionAction::RESTORE));
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillRepeatedly(Return(0));

    // 4
    std::tie(item, extManageInfo, ancoTarInfo, tempPath) = backInput;
    item = "test../../test.tar";
    EXPECT_EQ(extExtension_->ProcessTarFile(item, extManageInfo, ancoTarInfo, tempPath), ERR_INVALID_VALUE);

    std::tie(extExtension_->bundleName_, extExtension_->isDebug_) = oldParam;
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_ProcessTarFile_Test_0000";
}

/**
 * @tc.number: Ext_Extension_ProcessTarFile_Test_0001
 * @tc.name: Ext_Extension_ProcessTarFile_Test_0001
 * @tc.desc: 测试 ProcessTarFile - SUCEESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ProcessTarFile_Test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_ProcessTarFile_Test_0001";
    auto oldParam = std::make_tuple(extExtension_->bundleName_, extExtension_->isDebug_);
    extExtension_->bundleName_ = BConstants::BUNDLE_FILE_MANAGER;
    extExtension_->isDebug_ = true;

    std::string item;
    std::vector<ExtManageInfo> extManageInfo(1);
    std::tuple<std::vector<string>, std::vector<int64_t>, std::vector<string>> ancoTarInfo;
    std::string tempPath;
    item = "1.tar";
    extManageInfo.back().hashName = "1.tar";
    extManageInfo.back().isUserTar = false;
    auto backInput = std::make_tuple(item, extManageInfo, ancoTarInfo, tempPath);

    EXPECT_CALL(*extBackupMock_, GetExtensionAction()).WillRepeatedly(Return(BConstants::ExtensionAction::RESTORE));
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillRepeatedly(Return(0));

    // 5
    std::tie(item, extManageInfo, ancoTarInfo, tempPath) = backInput;
    item = "1_anco.tar";
    EXPECT_EQ(extExtension_->ProcessTarFile(item, extManageInfo, ancoTarInfo, tempPath), ERR_OK);

    EXPECT_CALL(*funcMock_, open(_, _)).WillRepeatedly(Invoke([&](const char *filename, int flags, ...) {
        errno = ERR_NO_PERMISSION;
        return -1;
    }));
    EXPECT_CALL(*funcMock_, fopen(_, _)).WillRepeatedly(Invoke([&](const char *pathname, const char *mode) {
        errno = ERR_OK;
        return nullptr;
    }));
    EXPECT_CALL(*funcMock_, RemoveFile(_)).WillRepeatedly(Return(true));

    // 6
    std::tie(item, extManageInfo, ancoTarInfo, tempPath) = backInput;
    EXPECT_CALL(*extBackupMock_, SpecialVersionForCloneAndCloud()).WillOnce(Return(false));
    EXPECT_CALL(*extBackupMock_, UseFullBackupOnly()).WillOnce(Return(false));
    EXPECT_EQ(extExtension_->ProcessTarFile(item, extManageInfo, ancoTarInfo, tempPath), ERR_OK);

    std::tie(extExtension_->bundleName_, extExtension_->isDebug_) = oldParam;
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_ProcessTarFile_Test_0001";
}

/**
 * @tc.number: Ext_Extension_ProcessTarFile_Test_0002
 * @tc.name: Ext_Extension_ProcessTarFile_Test_0002
 * @tc.desc: 测试 ProcessTarFile - SUCEESS
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ProcessTarFile_Test_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_ProcessTarFile_Test_0002";
    auto oldParam = std::make_tuple(extExtension_->bundleName_, extExtension_->isDebug_);
    extExtension_->bundleName_ = BConstants::BUNDLE_FILE_MANAGER;
    extExtension_->isDebug_ = true;

    std::string item;
    std::vector<ExtManageInfo> extManageInfo(1);
    std::tuple<std::vector<string>, std::vector<int64_t>, std::vector<string>> ancoTarInfo;
    std::string tempPath;
    item = "1.tar";
    extManageInfo.back().hashName = "1.tar";
    extManageInfo.back().isUserTar = false;
    auto backInput = std::make_tuple(item, extManageInfo, ancoTarInfo, tempPath);

    EXPECT_CALL(*extBackupMock_, GetExtensionAction()).WillRepeatedly(Return(BConstants::ExtensionAction::RESTORE));
    EXPECT_CALL(*funcMock_, mkdir(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*funcMock_, open(_, _)).WillRepeatedly(Invoke([&](const char *filename, int flags, ...) {
        errno = ERR_NO_PERMISSION;
        return -1;
    }));
    EXPECT_CALL(*funcMock_, fopen(_, _)).WillRepeatedly(Invoke([&](const char *pathname, const char *mode) {
        errno = ERR_OK;
        return nullptr;
    }));
    EXPECT_CALL(*funcMock_, RemoveFile(_)).WillRepeatedly(Return(true));

    // 7
    std::tie(item, extManageInfo, ancoTarInfo, tempPath) = backInput;
    EXPECT_CALL(*extBackupMock_, SpecialVersionForCloneAndCloud()).WillOnce(Return(true));
    EXPECT_EQ(extExtension_->ProcessTarFile(item, extManageInfo, ancoTarInfo, tempPath), ERR_OK);

    // 8
    std::tie(item, extManageInfo, ancoTarInfo, tempPath) = backInput;
    EXPECT_CALL(*extBackupMock_, SpecialVersionForCloneAndCloud()).WillOnce(Return(false));
    EXPECT_CALL(*extBackupMock_, UseFullBackupOnly()).WillOnce(Return(true));
    EXPECT_EQ(extExtension_->ProcessTarFile(item, extManageInfo, ancoTarInfo, tempPath), ERR_OK);

    std::tie(extExtension_->bundleName_, extExtension_->isDebug_) = oldParam;
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_ProcessTarFile_Test_0002";
}

/**
 * @tc.number: Ext_Extension_RestoreBigFileAfter_Test_0000
 * @tc.name: Ext_Extension_RestoreBigFileAfter_Test_0000
 * @tc.desc: 测试 RestoreBigFileAfter
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9P3Y3
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_RestoreBigFileAfter_Test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionNewTest-begin Ext_Extension_RestoreBigFileAfter_Test_0000";
    string filePath = string(BConstants::PATH_FILEMANAGE_BACKUP_HOME_ANCO).append(BConstants::SA_BUNDLE_BACKUP_RESTORE)
        .append("1.txt");
    struct stat sta;
    std::vector<StatInfo> ancoStats;

    // 1
    EXPECT_CALL(*funcMock_, chmod(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Invoke([&](const char *filename, int flags, ...) {
        errno = ERR_NO_PERMISSION;
        return -1;
    }));
    extExtension_->RestoreBigFileAfter(filePath, sta, ancoStats);
    EXPECT_EQ(ancoStats.size(), 1);

    // 2
    ancoStats.clear();
    EXPECT_CALL(*funcMock_, chmod(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Invoke([&](const char *filename, int flags, ...) {
        errno = ERR_OK;
        return 100;
    }));
    EXPECT_CALL(*funcMock_, futimens(_, _)).WillOnce(Return(0));
    extExtension_->RestoreBigFileAfter(filePath, sta, ancoStats);
    EXPECT_EQ(ancoStats.size(), 1);

    GTEST_LOG_(INFO) << "ExtExtensionNewTest-end Ext_Extension_RestoreBigFileAfter_Test_0000";
}
}