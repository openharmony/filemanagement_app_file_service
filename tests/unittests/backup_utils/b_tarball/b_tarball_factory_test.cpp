/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <string>

#include <file_ex.h>
#include <gtest/gtest.h>

#include "b_tarball/b_tarball_factory.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BTarballFactoryTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_b_tarball_factory_0100
 * @tc.name: b_tarball_factory_0100
 * @tc.desc: 测试BTarballFactory类create函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BTarballFactoryTest, b_tarball_factory_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballFactoryTest-begin b_tarball_factory_0100";
    try {
        // 预置路径
        TestManager tm("b_tarball_factory_0100");
        string root = tm.GetRootDirCurTest();
        string implType = "cmdline";
        string tarballPath = root + "/test.tar";
        SaveStringToFile(tarballPath, "data/storage/el2/database/");
        // 调用create获取打包解包能力
        auto tarballTar = BTarballFactory::Create(implType, tarballPath);
        EXPECT_TRUE(tarballTar != nullptr);
        GTEST_LOG_(INFO) << "BTarballFactoryTest-tar";
        (tarballTar->tar)("/", {}, {"/data/storage/el2/database/", {}});
        GTEST_LOG_(INFO) << "BTarballFactoryTest-untar";
        (tarballTar->untar)("/");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballFactoryTest-an exception occurred by BTarballFactory.";
    }
    GTEST_LOG_(INFO) << "BTarballFactoryTest-end b_tarball_factory_0100";
}

/**
 * @tc.number: SUB_b_tarball_factory_0200
 * @tc.name: b_tarball_factory_0200
 * @tc.desc: 测试BTarballFactory类create函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BTarballFactoryTest, b_tarball_factory_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballFactoryTest-begin b_tarball_factory_0200";
    try {
        // 预置路径
        TestManager tm("b_tarball_factory_0200");
        string root = tm.GetRootDirCurTest();
        string type = "";
        string tarballPath = root + "/test.tar";
        SaveStringToFile(tarballPath, "data/storage/el2/database/");
        // 调用create获取打包解包能力
        auto tarballTar = BTarballFactory::Create(type, tarballPath);
        EXPECT_TRUE(tarballTar != nullptr);
        GTEST_LOG_(INFO) << "BTarballFactoryTest-tar";
        (tarballTar->tar)("/", {}, {"/data/storage/el2/database/", {}});
        GTEST_LOG_(INFO) << "BTarballFactoryTest-untar";
        (tarballTar->untar)("/");
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "BTarballFactoryTest-an exception occurred by BTarballFactory.";
    }
    GTEST_LOG_(INFO) << "BTarballFactoryTest-end b_tarball_factory_0200";
}

/**
 * @tc.number: SUB_b_tarball_factory_0300
 * @tc.name: b_tarball_factory_0300
 * @tc.desc: 测试BTarballFactory类create函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BTarballFactoryTest, b_tarball_factory_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballFactoryTest-begin b_tarball_factory_0300";
    try {
        // 预置路径
        TestManager tm("b_tarball_factory_0300");
        string root = tm.GetRootDirCurTest();
        string implType = "cmdline";
        string path = root + "";
        SaveStringToFile(path, "data/storage/el2/database/");
        // 调用create获取打包解包能力
        auto tarballTar = BTarballFactory::Create(implType, path);
        EXPECT_TRUE(tarballTar != nullptr);
        GTEST_LOG_(INFO) << "BTarballFactoryTest-tar";
        (tarballTar->tar)("/", {}, {"/data/storage/el2/database/", {}});
        GTEST_LOG_(INFO) << "BTarballFactoryTest-untar";
        (tarballTar->untar)("/");
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "BTarballFactoryTest-an exception occurred by BTarballFactory.";
    }
    GTEST_LOG_(INFO) << "BTarballFactoryTest-end b_tarball_factory_0300";
}

/**
 * @tc.number: SUB_b_tarball_factory_0400
 * @tc.name: b_tarball_factory_0400
 * @tc.desc: 测试BTarballFactory类create函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BTarballFactoryTest, b_tarball_factory_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballFactoryTest-begin b_tarball_factory_0400";
    try {
        // 预置路径
        TestManager tm("b_tarball_factory_0400");
        string root = tm.GetRootDirCurTest();
        string implType = "";
        string tarballPath = root + "";
        SaveStringToFile(tarballPath, "data/storage/el2/database/");
        // 调用create获取打包解包能力
        auto tarballTar = BTarballFactory::Create(implType, tarballPath);
        EXPECT_TRUE(tarballTar != nullptr);
        GTEST_LOG_(INFO) << "BTarballFactoryTest-tar";
        (tarballTar->tar)("/", {}, {"/data/storage/el2/database/", {}});
        GTEST_LOG_(INFO) << "BTarballFactoryTest-untar";
        (tarballTar->untar)("/");
    } catch (...) {
        EXPECT_TRUE(true);
        GTEST_LOG_(INFO) << "BTarballFactoryTest-an exception occurred by BTarballFactory.";
    }
    GTEST_LOG_(INFO) << "BTarballFactoryTest-end b_tarball_factory_0400";
}
} // namespace OHOS::FileManagement::Backup