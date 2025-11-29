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
    };
    void TearDown() override
    {
        LibraryFuncMock::libraryFunc_ = nullptr;
        funcMock_ = nullptr;
        ServiceClient::serviceProxy_ = nullptr;
        serviceMock_ = nullptr;
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

    extBackupMock_ = make_shared<ExtBackupMock>();
    ExtBackupMock::extBackup = extBackupMock_;
};

void ExtExtensionNewTest::TearDownTestCase(void)
{
    LibraryFuncMock::libraryFunc_ = nullptr;
    funcMock_ = nullptr;
    ServiceClient::serviceProxy_ = nullptr;
    serviceMock_ = nullptr;

    extension_ = nullptr;
    extExtension_ = nullptr;

    ExtBackupMock::extBackup = nullptr;
    extBackupMock_ = nullptr;
};

/**
 * @tc.number: Ext_Extension_ReportAppFileReady_Test_0100
 * @tc.name: Ext_Extension_ReportAppFileReady_Test_0100
 * @tc.desc: 测试ReportAppFileReady open失败且为错误码为没权限场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportAppFileReady_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_ReportAppFileReady_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    errno = ERR_NO_PERMISSION;
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(-1));
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    EXPECT_EQ(extExtension_->ReportAppFileReady(filename, filePath, false), ERR_NO_PERMISSION);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_ReportAppFileReady_Test_0100";
}

/**
 * @tc.number: Ext_Extension_ReportAppFileReady_Test_0200
 * @tc.name: Ext_Extension_ReportAppFileReady_Test_0200
 * @tc.desc: 测试ReportAppFileReady proxy为空场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportAppFileReady_Test_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_ReportAppFileReady_Test_0200";
    ASSERT_TRUE(extExtension_ != nullptr);
    errno = -1;
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(-1));
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    ServiceClient::serviceProxy_ = nullptr;
    EXPECT_EQ(extExtension_->ReportAppFileReady(filename, filePath, false),
        static_cast<int32_t>(BError::Codes::EXT_CLIENT_IS_NULL));
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_ReportAppFileReady_Test_0200";
}

/**
 * @tc.number: Ext_Extension_ReportAppFileReady_Test_0300
 * @tc.name: Ext_Extension_ReportAppFileReady_Test_0300
 * @tc.desc: 测试ReportAppFileReady AppFileReadyWithoutFd/AppFileReady返回-1场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportAppFileReady_Test_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_ReportAppFileReady_Test_0300";
    ASSERT_TRUE(extExtension_ != nullptr);
    errno = -1;
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(-1)).WillOnce(Return(1));
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    EXPECT_CALL(*serviceMock_, AppFileReadyWithoutFd(_, _)).WillOnce(Return(-1));
    EXPECT_EQ(extExtension_->ReportAppFileReady(filename, filePath, false), -1);
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillOnce(Return(-2));
    EXPECT_EQ(extExtension_->ReportAppFileReady(filename, filePath, false), -2);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_ReportAppFileReady_Test_0300";
}

/**
 * @tc.number: Ext_Extension_ReportAppFileReady_Test_0400
 * @tc.name: Ext_Extension_ReportAppFileReady_Test_0400
 * @tc.desc: 测试ReportAppFileReady 成功场景
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_ReportAppFileReady_Test_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_ReportAppFileReady_Test_0400";
    ASSERT_TRUE(extExtension_ != nullptr);
    EXPECT_CALL(*funcMock_, open(_, _)).WillOnce(Return(1));
    string filename = "app_file_ready_test";
    string filePath = "/tmp";
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillOnce(Return(0));
    EXPECT_EQ(extExtension_->ReportAppFileReady(filename, filePath, true), 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_ReportAppFileReady_Test_0400";
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
    EXPECT_CALL(*serviceMock_, AppFileReady(_, _, _)).WillRepeatedly(Return(0));
    GTEST_LOG_(INFO) << "2. test ok";
    extExtension_->DoBackupTask();
    EXPECT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_DoBackupTask_Test_0200";
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
 * @tc.number: Ext_Extension_AsyncDoBackup_Test_0100
 * @tc.name: Ext_Extension_AsyncDoBackup_Test_0100
 * @tc.desc: 测试AsyncDoBackup
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ExtExtensionNewTest, Ext_Extension_AsyncDoBackup_Test_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-begin Ext_Extension_AsyncDoBackup_Test_0100";
    ASSERT_TRUE(extExtension_ != nullptr);
    errno = EPERM;
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
    extExtension_->AsyncDoBackup();
    this_thread::sleep_for(chrono::seconds(5));
    EXPECT_FALSE(ScanFileSingleton::GetInstance().IsProcessCompleted());
    GTEST_LOG_(INFO) << "ExtExtensionSubTest-end Ext_Extension_AsyncDoBackup_Test_0100";
}
}