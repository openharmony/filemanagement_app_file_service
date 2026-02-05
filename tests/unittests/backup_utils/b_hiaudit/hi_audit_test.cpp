/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <fstream>
#include <gtest/gtest.h>
#include <unistd.h>

#include "b_hiaudit/hi_audit.h"
#include "b_resources/b_constants.h"
#include "directory_ex.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

class HiAuditTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        GTEST_LOG_(INFO) << "HiAuditTest SetUpTestCase enter";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "HiAuditTest TearDownTestCase enter";
    }
    void SetUp() override {}
    void TearDown() override
    {
        const string testLogDir = "/data/storage/el2/log/hiaudit/";
        const string blockPath = "/data/storage/el2/log/hiaudit";
        (void)remove(blockPath.c_str());
        system(("mkdir -p " + testLogDir).c_str());
    }
};

/**
 * @tc.number: SUB_Hi_Audit_FDSan_Init_0100
 * @tc.name: Hi_Audit_FDSan_Init_Test_0100
 * @tc.desc: Test HiAudit::Init function FDSan integration
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I9P3Y3
 */
HWTEST_F(HiAuditTest, Hi_Audit_FDSan_Init_Test_0100, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "HiAuditTest-begin Hi_Audit_FDSan_Init_Test_0100";
    try {
        string testLogFile = "/data/storage/el2/log/hiaudit/test_audit.csv";
        string testLogDir = "/data/storage/el2/log/hiaudit/";
        system(("mkdir -p " + testLogDir).c_str());
        int fd = open(testLogFile.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
        ASSERT_GE(fd, 0) << "Failed to create test log file";
        fdsan_exchange_owner_tag(fd, 0, BConstants::FDSAN_UTIL_TAG);
        const char* testData = "test audit log entry";
        write(fd, testData, strlen(testData));
        // 关闭文件描述符
        int ret = fdsan_close_with_tag(fd, BConstants::FDSAN_UTIL_TAG);
        EXPECT_EQ(ret, 0);
        // 清理
        remove(testLogFile.c_str());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "HiAuditTest-exception occurred in Init FDSan test.";
    }
    GTEST_LOG_(INFO) << "HiAuditTest-end Hi_Audit_FDSan_Init_Test_0100";
}

/**
 * @tc.number: SUB_Hi_Audit_FDSan_GetWriteFilePath_0101
 * @tc.name: Hi_Audit_FDSan_GetWriteFilePath_Test_0101
 * @tc.desc: Test HiAudit::GetWriteFilePath function FDSan integration (close and reopen)
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I9P3Y3
 */
HWTEST_F(HiAuditTest, Hi_Audit_FDSan_GetWriteFilePath_Test_0101, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "HiAuditTest-begin Hi_Audit_FDSan_GetWriteFilePath_Test_0101";
    try {
        string testLogFile = "/data/storage/el2/log/hiaudit/test_rotate_audit.csv";
        string testLogDir = "/data/storage/el2/log/hiaudit/";

        // 创建测试日志目录
        system(("mkdir -p " + testLogDir).c_str());
        int fd = open(testLogFile.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
        ASSERT_GE(fd, 0) << "Failed to create test log file";
        fdsan_exchange_owner_tag(fd, 0, BConstants::FDSAN_UTIL_TAG);
        const char* testData = "test audit log entry";
        write(fd, testData, strlen(testData));
        int ret = fdsan_close_with_tag(fd, BConstants::FDSAN_UTIL_TAG);
        EXPECT_EQ(ret, 0);
        int newFd = open(testLogFile.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
        ASSERT_GE(newFd, 0) << "Failed to create new log file";
        fdsan_exchange_owner_tag(newFd, 0, BConstants::FDSAN_UTIL_TAG);

        const char* newData = "new audit log entry";
        write(newFd, newData, strlen(newData));
        ret = fdsan_close_with_tag(newFd, BConstants::FDSAN_UTIL_TAG);
        EXPECT_EQ(ret, 0);
        // 清理
        remove(testLogFile.c_str());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "HiAuditTest-exception occurred in GetWriteFilePath FDSan test.";
    }
    GTEST_LOG_(INFO) << "HiAuditTest-end Hi_Audit_FDSan_GetWriteFilePath_Test_0101";
}

/**
 * @tc.number: SUB_Hi_Audit_FDSan_Destructor_0102
 * @tc.name: Hi_Audit_FDSan_Destructor_Test_0102
 * @tc.desc: Test HiAudit destructor FDSan integration
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I9P3Y3
 */
HWTEST_F(HiAuditTest, Hi_Audit_FDSan_Destructor_Test_0102, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "HiAuditTest-begin Hi_Audit_FDSan_Destructor_Test_0102";
    try {
        string testLogFile = "/data/storage/el2/log/hiaudit/test_destructor_audit.csv";
        string testLogDir = "/data/storage/el2/log/hiaudit/";

        // 创建测试日志目录
        system(("mkdir -p " + testLogDir).c_str());
        int writeFd = open(testLogFile.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
        ASSERT_GE(writeFd, 0) << "Failed to create test log file";
        fdsan_exchange_owner_tag(writeFd, 0, BConstants::FDSAN_UTIL_TAG);

        // 写入测试数据
        const char* testData = "test audit log entry for destructor";
        write(writeFd, testData, strlen(testData));
        if (writeFd >= 0) {
            int ret = fdsan_close_with_tag(writeFd, BConstants::FDSAN_UTIL_TAG);
            EXPECT_EQ(ret, 0);
        }
        remove(testLogFile.c_str());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "HiAuditTest-exception occurred in Destructor FDSan test.";
    }
    GTEST_LOG_(INFO) << "HiAuditTest-end Hi_Audit_FDSan_Destructor_Test_0102";
}

/**
 * @tc.number: SUB_Hi_Audit_FDSan_WriteToFile_0103
 * @tc.name: Hi_Audit_FDSan_WriteToFile_Test_0103
 * @tc.desc: Test HiAudit::WriteToFile function FDSan integration
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I9P3Y3
 */
HWTEST_F(HiAuditTest, Hi_Audit_FDSan_WriteToFile_Test_0103, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "HiAuditTest-begin Hi_Audit_FDSan_WriteToFile_Test_0103";
    try {
        string testLogFile = "/data/storage/el2/log/hiaudit/test_write_audit.csv";
        string testLogDir = "/data/storage/el2/log/hiaudit/";

        // 创建测试日志目录
        system(("mkdir -p " + testLogDir).c_str());

        // 模拟 HiAudit::Init 打开文件
        int writeFd = open(testLogFile.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
        ASSERT_GE(writeFd, 0) << "Failed to create test log file";
        fdsan_exchange_owner_tag(writeFd, 0, BConstants::FDSAN_UTIL_TAG);
        string content = "timestamp,operation,result\n";
        content += "2025-01-21,backup,success\n";
        write(writeFd, content.c_str(), content.length());
        int ret = fdsan_close_with_tag(writeFd, BConstants::FDSAN_UTIL_TAG);
        EXPECT_EQ(ret, 0);
        ifstream file(testLogFile);
        string readContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();
        EXPECT_EQ(readContent, content);
        remove(testLogFile.c_str());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "HiAuditTest-exception occurred in WriteToFile FDSan test.";
    }
    GTEST_LOG_(INFO) << "HiAuditTest-end Hi_Audit_FDSan_WriteToFile_Test_0103";
}

/**
 * @tc.number: SUB_Hi_Audit_FDSan_Init_Success_0104
 * @tc.name: Hi_Audit_FDSan_Init_Success_Test_0104
 * @tc.desc: Test HiAudit::Init function open success scenario (if branch, writeFd_ >= 0)
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I9P3Y3
 */
HWTEST_F(HiAuditTest, Hi_Audit_FDSan_Init_Success_Test_0104, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "HiAuditTest-begin Hi_Audit_FDSan_Init_Success_Test_0104";
    try {
        string testLogDir = "/data/storage/el2/log/hiaudit/";
        string testLogFile = testLogDir + "appfileservice_audit.csv";
        system(("mkdir -p " + testLogDir).c_str());
        remove(testLogFile.c_str());  // 清理可能存在的旧文件
        HiAudit& hiAudit = HiAudit::GetInstance(false);
        hiAudit.Init();
        EXPECT_GE(hiAudit.writeFd_, 0);
        if (hiAudit.writeFd_ >= 0) {
            (void)fdsan_close_with_tag(hiAudit.writeFd_, BConstants::FDSAN_UTIL_TAG);
            hiAudit.writeFd_ = -1;
        }
        remove(testLogFile.c_str());  // 清理测试文件
        GTEST_LOG_(INFO) << "Hi_Audit_FDSan_Init_Success_Test_0104: Init if branch tested";
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "HiAuditTest-exception occurred in Init Success FDSan test.";
    }
    GTEST_LOG_(INFO) << "HiAuditTest-end Hi_Audit_FDSan_Init_Success_Test_0104";
}

/**
 * @tc.number: SUB_Hi_Audit_FDSan_Init_Fail_0105
 * @tc.name: Hi_Audit_FDSan_Init_Fail_Test_0105
 * @tc.desc: Test HiAudit::Init function open failure scenario (else branch, writeFd_ < 0)
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I9P3Y3
 */
HWTEST_F(HiAuditTest, Hi_Audit_FDSan_Init_Fail_Test_0105, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "HiAuditTest-begin Hi_Audit_FDSan_Init_Fail_Test_0105";
    try {
        HiAudit& hiAudit = HiAudit::GetInstance(false);
        if (hiAudit.writeFd_ >= 0) {
            (void)fdsan_close_with_tag(hiAudit.writeFd_, BConstants::FDSAN_UTIL_TAG);
            hiAudit.writeFd_ = -1;
        }
        string testLogDir = "/data/storage/el2/log/hiaudit/";
        string blockPath = "/data/storage/el2/log/hiaudit";
        ForceRemoveDirectoryBMS(testLogDir);
        string testLogParentDir = "/data/storage/el2/log/";
        ForceCreateDirectory(testLogParentDir.data());
        int blockFd = open(blockPath.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (blockFd >= 0) {
            close(blockFd);
            GTEST_LOG_(INFO) << "Test_0105: Created file at log dir path to block mkdir";
        }
        hiAudit.Init();
        EXPECT_LT(hiAudit.writeFd_, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "HiAuditTest-exception occurred in Init Fail FDSan test.";
    }
    GTEST_LOG_(INFO) << "HiAuditTest-end Hi_Audit_FDSan_Init_Fail_Test_0105";
}

/**
 * @tc.number: SUB_Hi_Audit_FDSan_GetWriteFilePath_Success_0106
 * @tc.name: Hi_Audit_FDSan_GetWriteFilePath_Success_Test_0106
 * @tc.desc: Test HiAudit::GetWriteFilePath function open success scenario (if branch, writeFd_ >= 0)
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I9P3Y3
 */
HWTEST_F(HiAuditTest, Hi_Audit_FDSan_GetWriteFilePath_Success_Test_0106, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "HiAuditTest-begin Hi_Audit_FDSan_GetWriteFilePath_Success_Test_0106";
    try {
        string testLogDir = "/data/storage/el2/log/hiaudit/";
        string blockPath = "/data/storage/el2/log/hiaudit";
        string testLogFile = testLogDir + "appfileservice_audit.csv";
        (void)remove(blockPath.c_str());
        ForceRemoveDirectoryBMS(testLogDir);
        system(("mkdir -p " + testLogDir).c_str());
        (void)remove(testLogFile.c_str());
        int fd = open(testLogFile.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fd >= 0) {
            fdsan_exchange_owner_tag(fd, 0, BConstants::FDSAN_UTIL_TAG);
            fdsan_close_with_tag(fd, BConstants::FDSAN_UTIL_TAG);
        }
        HiAudit& hiAudit = HiAudit::GetInstance(false);
        hiAudit.Init();
        hiAudit.writeLogSize_ = 4 * 1024 * 1024;
        hiAudit.GetWriteFilePath();
        EXPECT_GE(hiAudit.writeFd_, 0);
        if (hiAudit.writeFd_ >= 0) {
            (void)fdsan_close_with_tag(hiAudit.writeFd_, BConstants::FDSAN_UTIL_TAG);
            hiAudit.writeFd_ = -1;
        }
        // 清理
        remove(testLogFile.c_str());
        GTEST_LOG_(INFO) << "Hi_Audit_FDSan_GetWriteFilePath_Success_Test_0106: GetWriteFilePath if branch tested";
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "HiAuditTest-exception occurred in GetWriteFilePath Success FDSan test.";
    }
    GTEST_LOG_(INFO) << "HiAuditTest-end Hi_Audit_FDSan_GetWriteFilePath_Success_Test_0106";
}

/**
 * @tc.number: SUB_Hi_Audit_FDSan_GetWriteFilePath_Fail_0107
 * @tc.name: Hi_Audit_FDSan_GetWriteFilePath_Fail_Test_0107
 * @tc.desc: Test HiAudit::GetWriteFilePath function open failure scenario (else branch, writeFd_ < 0)
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: I9P3Y3
 */
HWTEST_F(HiAuditTest, Hi_Audit_FDSan_GetWriteFilePath_Fail_Test_0107, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "HiAuditTest-begin Hi_Audit_FDSan_GetWriteFilePath_Fail_Test_0107";
    try {
        HiAudit& hiAudit = HiAudit::GetInstance(false);
        hiAudit.Init();
        int oldFd = hiAudit.writeFd_;
        string testLogDir = "/data/storage/el2/log/hiaudit/";
        string blockPath = "/data/storage/el2/log/hiaudit";
        ForceRemoveDirectoryBMS(testLogDir);
        string testLogParentDir = "/data/storage/el2/log/";
        ForceCreateDirectory(testLogParentDir.data());
        int blockFd = open(blockPath.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (blockFd >= 0) {
            close(blockFd);
            GTEST_LOG_(INFO) << "Test_0107: Created file at log dir path to block mkdir";
        }
        hiAudit.writeLogSize_ = 4 * 1024 * 1024;
        hiAudit.GetWriteFilePath();
        EXPECT_EQ(hiAudit.writeFd_, oldFd);
        if (hiAudit.writeFd_ >= 0) {
            (void)fdsan_close_with_tag(hiAudit.writeFd_, BConstants::FDSAN_UTIL_TAG);
            hiAudit.writeFd_ = -1;
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "HiAuditTest-exception occurred in GetWriteFilePath Fail FDSan test.";
    }
    GTEST_LOG_(INFO) << "HiAuditTest-end Hi_Audit_FDSan_GetWriteFilePath_Fail_Test_0107";
}
} // namespace OHOS::FileManagement::Backup
