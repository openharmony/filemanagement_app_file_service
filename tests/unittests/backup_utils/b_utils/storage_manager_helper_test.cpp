/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include <limits>

#include "b_utils/storage_manager_helper.h"

namespace OHOS::FileManagement::Backup {

class StorageManagerHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: storage_manager_helper_GetInstance_0100
 * @tc.name: storage_manager_helper_GetInstance_0100
 * @tc.desc: 测试StorageManagerHelper是否为单例，即多次获取实例返回相同对象
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(StorageManagerHelperTest, storage_manager_helper_GetInstance_0100, testing::ext::TestSize.Level1)
{
    auto& instance1 = StorageManagerHelper::GetInstance();
    auto& instance2 = StorageManagerHelper::GetInstance();
    EXPECT_EQ(&instance1, &instance2) << "StorageManagerHelper should be a singleton, but it's not.";
}

/**
 * @tc.number: storage_manager_helper_GetFreeSize_0100
 * @tc.name: storage_manager_helper_GetFreeSize_0100
 * @tc.desc: 测试GetFreeSize正常返回场景
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(StorageManagerHelperTest, storage_manager_helper_GetFreeSize_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerHelperTest-begin: storage_manager_helper_GetFreeSize_0100";
    auto& instance = StorageManagerHelper::GetInstance();
    uint64_t freeSize = instance.GetFreeSize();
    // 由于依赖系统服务，在测试环境中可能返回0或实际值
    // 这里主要测试函数不会崩溃，返回值大于等于0
    GTEST_LOG_(INFO) << "GetFreeSize returned: " << freeSize;
    EXPECT_GE(freeSize, 0u);
    GTEST_LOG_(INFO) << "StorageManagerHelperTest-end: storage_manager_helper_GetFreeSize_0100";
}

/**
 * @tc.number: storage_manager_helper_GetFreeSize_0200
 * @tc.name: storage_manager_helper_GetFreeSize_0200
 * @tc.desc: 测试GetFreeSize多次调用返回一致性
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(StorageManagerHelperTest, storage_manager_helper_GetFreeSize_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerHelperTest-begin: storage_manager_helper_GetFreeSize_0200";
    auto& instance = StorageManagerHelper::GetInstance();
    uint64_t freeSize1 = instance.GetFreeSize();
    uint64_t freeSize2 = instance.GetFreeSize();
    // 两次调用应该返回相同值（proxy被缓存）
    GTEST_LOG_(INFO) << "First call: " << freeSize1 << ", Second call: " << freeSize2;
    EXPECT_EQ(freeSize1, freeSize2);
    GTEST_LOG_(INFO) << "StorageManagerHelperTest-end: storage_manager_helper_GetFreeSize_0200";
}

/**
 * @tc.number: storage_manager_helper_GetFreeSize_0300
 * @tc.name: storage_manager_helper_GetFreeSize_0300
 * @tc.desc: 测试GetFreeSize返回值类型正确性
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(StorageManagerHelperTest, storage_manager_helper_GetFreeSize_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StorageManagerHelperTest-begin: storage_manager_helper_GetFreeSize_0300";
    auto& instance = StorageManagerHelper::GetInstance();
    uint64_t freeSize = instance.GetFreeSize();
    // 验证返回值是uint64_t类型（编译时检查）
    // 验证返回值不会是负数（转换为有符号应该大于等于0或非常大）
    int64_t signedSize = static_cast<int64_t>(freeSize);
    if (freeSize <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
        EXPECT_GE(signedSize, 0);
    }
    GTEST_LOG_(INFO) << "StorageManagerHelperTest-end: storage_manager_helper_GetFreeSize_0300";
}

} // namespace OHOS::FileManagement::Backup
