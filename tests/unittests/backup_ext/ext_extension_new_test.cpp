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
}