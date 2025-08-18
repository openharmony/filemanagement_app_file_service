/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <tuple>

#include <fcntl.h>

#include <file_ex.h>
#include <gtest/gtest.h>

#include "b_utils/scan_file_singleton.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class ScanFileSingletonTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @brief 测试ScanFileSingleton的单例特性
 * @tc.number: SUB_scan_file_singleton_GetInstance_0100
 * @tc.name: scan_file_singleton_GetInstance_0100
 * @tc.desc: 测试ScanFileSingleton是否为单例，即多次获取实例返回相同对象
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, scan_file_singleton_GetInstance_0100, testing::ext::TestSize.Level1)
{
    auto& instance1 = ScanFileSingleton::GetInstance();
    auto& instance2 = ScanFileSingleton::GetInstance();
    EXPECT_EQ(&instance1, &instance2) << "ScanFileSingleton should be a singleton, but it's not.";
}

/**
 * @brief 测试获取所有大文件
 * @tc.number: SUB_scan_file_singleton_GetAllBigfiles_0100
 * @tc.name: scan_file_singleton_GetAllBigfiles_0100
 * @tc.desc: 测试ScanFileSingleton的GetAllBigfiles方法是否能正确获取所有文件信息
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, scan_file_singleton_GetAllBigfiles_0100, testing::ext::TestSize.Level1)
{
    auto& instance = ScanFileSingleton::GetInstance();
    struct stat fileStat = {};
    // 假设文件大小为2048字节
    fileStat.st_size = 2048;
    instance.AddBigFile("/path/to/another_file", fileStat);

    auto allBigFiles = instance.GetAllBigFiles();

    EXPECT_EQ(allBigFiles.size(), 1) << "There should be one big file in queue.";
    EXPECT_EQ(allBigFiles["/path/to/another_file"].st_size, 2048) << "the file size should be 2048 bytes.";
}

/**
 * @brief 测试设置和获取完成标志
 * @tc.number: SUB_scan_file_singleton_SetGetCompletedFlag_0100
 * @tc.name: scan_file_singleton_SetGetCompletedFlag_0100
 * @tc.desc: 测试ScanFileSingleton的SetCompletedFlag和GetCompletedFlag方法是否能正确设置和获取完成标志
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, scan_file_singleton_SetGetCompletedFlag_0100, testing::ext::TestSize.Level1)
{
    auto& instance = ScanFileSingleton::GetInstance();
    instance.SetCompletedFlag(true);
    EXPECT_TRUE(instance.GetCompletedFlag()) << "The completed flag should be true.";

    instance.SetCompletedFlag(false);
    EXPECT_FALSE(instance.GetCompletedFlag()) << "The completed flag should be false.";
}

/**
 * @brief 测试添加和获取小文件信息
 * @tc.number: SUB_scan_file_singleton_GetAllSmallFiles_0100
 * @tc.name: scan_file_singleton_GetAllSmallFiles_0100
 * @tc.desc: 测试ScanFileSingleton的GetAllSmallfiles方法是否能正确添加和获取小文件信息
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, scan_file_singleton_GetAllSmallFiles_0100, testing::ext::TestSize.Level1)
{
    auto& instance = ScanFileSingleton::GetInstance();

    instance.AddSmallFile("/path/to/small_file.txt", 512);
    auto allSmallFiles = instance.GetAllSmallFiles();

    EXPECT_EQ(allSmallFiles.size(), 1) << "There should be one small file in queue.";
    EXPECT_EQ(allSmallFiles["/path/to/small_file.txt"], 512) << "The file size should be 512 bytes.";
}

/**
 * @brief 测试设置和获取includeSize
 * @tc.number: SUB_scan_file_singleton_SetGetIncludeSize_0100
 * @tc.name: scan_file_singleton_SetGetIncludeSize_0100
 * @tc.desc: 测试ScanFileSingleton的SetIncludeSize和GetIncludeSize方法是否能正确设置和获取includeSize
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, scan_file_singleton_SetGetIncludeSize_0100, testing::ext::TestSize.Level1)
{
    auto& instance = ScanFileSingleton::GetInstance();

    instance.SetIncludeSize(100);
    EXPECT_EQ(instance.GetIncludeSize(), 100) << "The include size should be 100.";
}

/**
 * @brief 测试设置和获取excludeSize
 * @tc.number: SUB_scan_file_singleton_SetGetExcludeSize_0100
 * @tc.name: scan_file_singleton_SetGetExcludeSize_0100
 * @tc.desc: 测试ScanFileSingleton的SetExcludeSize和GetExcludeSize方法是否能正确设置和获取excludeSize
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(ScanFileSingletonTest, scan_file_singleton_SetGetExcludeSize_0100, testing::ext::TestSize.Level1)
{
    auto& instance = ScanFileSingleton::GetInstance();

    instance.SetExcludeSize(100);
    EXPECT_EQ(instance.GetExcludeSize(), 100) << "The exclude size should be 100.";
}
} // namespace OHOS::FileManagement::Backup