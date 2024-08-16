/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <map>
#include <set>
#include <string>
#include <utility>

#include <fcntl.h>
#include <sys/stat.h>

#include <file_ex.h>
#include <gtest/gtest.h>
#include <json/value.h>

#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "test_manager.h"

#include "src/b_json/b_json_entity_ext_manage.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing::ext;
const int32_t INDEX_FIRST = 0;
const int32_t INDEX_SECOND = 1;
const int32_t INDEX_THIRD = 2;
class BJsonEntityExtManageTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

bool IsEqual(const struct stat &lf, const struct stat &rh)
{
    if (lf.st_size != rh.st_size) {
        return false;
    }
    if (lf.st_atim.tv_sec != rh.st_atim.tv_sec) {
        return false;
    }
    if (lf.st_atim.tv_nsec != rh.st_atim.tv_nsec) {
        return false;
    }
    if (lf.st_mtim.tv_sec != rh.st_mtim.tv_sec) {
        return false;
    }
    if (lf.st_mtim.tv_nsec != rh.st_mtim.tv_nsec) {
        return false;
    }
    return true;
}

bool IsEqual(const map<string, tuple<string, struct stat, bool>> &lf,
             const map<string, tuple<string, struct stat, bool>> &rh)
{
    if (lf.size() != rh.size()) {
        return false;
    }

    auto itemLF = lf.begin();
    auto itemRH = rh.begin();
    for (; itemLF != lf.end(); ++itemLF, ++itemRH) {
        if (itemLF->first != itemRH->first) {
            return false;
        }
        if (std::get<INDEX_FIRST>(itemLF->second) != std::get<INDEX_FIRST>(itemRH->second)) {
            return false;
        }
        if (!IsEqual(std::get<INDEX_SECOND>(itemLF->second), std::get<INDEX_SECOND>(itemRH->second))) {
            return false;
        }
        if (std::get<INDEX_THIRD>(itemLF->second) != std::get<INDEX_THIRD>(itemRH->second)) {
            return false;
        }
    }

    return true;
}

bool IsEqual(const std::vector<ExtManageInfo> &lf, const map<string, tuple<string, struct stat, bool>> &rh)
{
    if (lf.size() != rh.size()) {
        return false;
    }
    for (auto &item : lf) {
        if (rh.find(item.hashName) != rh.end()) {
            auto [fileName, sta, isBeforeTar] = rh.at(item.hashName);
            if (item.fileName != fileName || !IsEqual(item.sta, sta)) {
                return false;
            }
        }
    }
    return true;
}

struct stat GetFileStat(const string &pathTestFile)
{
    struct stat sta = {};
    if (stat(pathTestFile.data(), &sta) == -1) {
        GTEST_LOG_(INFO) << pathTestFile << " invoked stat failure, errno :" << errno;
        throw BError(errno);
    }
    return sta;
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0100
 * @tc.name: b_json_entity_ext_manage_0100
 * @tc.desc: 通过向接口SetExtManage传入不包含任何信息的空map参数，测试对索引文件的操作是否正确。
 *           0：通过向索引文件写入0条数据模拟对索引文件的(空)内容的测试覆盖
 *           1：调用接口SetExtManage，向索引文件写入数据
 *           2：调用接口GetExtManage，从索引文件读出文件名数据
 *           3：调用接口GetExtManageInfo，从索引文件读出文件详细数据(含文件名和对应文件的stat数据)
 *           4：判断读出的文件名集合/文件详细数据记录个数是否为0
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0100";
    try {
        // 预置文件数据
        // 索引文件pathManageFile
        TestManager tm("b_json_entity_ext_manage_0100");
        string root = tm.GetRootDirCurTest();
        string pathManageFile = root + "manage.json";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(pathManageFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();

        // 写入空数据
        map<string, tuple<string, struct stat, bool>> info;
        cache.SetExtManage(info);

        // 读取索引文件信息并做结果判断
        auto fileNames = cache.GetExtManage();
        EXPECT_EQ(fileNames.size(), 0ul);
        auto fileInfo = cache.GetExtManageInfo();
        EXPECT_EQ(fileInfo.size(), 0ul);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0100";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0200
 * @tc.name: b_json_entity_ext_manage_0200
 * @tc.desc: 通过向接口SetExtManage传入包含一条信息的map参数，测试对索引文件的操作是否正确。
 *           0：通过向索引文件写入1条有效数据模拟对索引文件的(有)内容的测试覆盖
 *           1：调用接口SetExtManage，向索引文件写入数据
 *           2：调用接口GetExtManage，从索引文件读出文件名数据
 *           3：调用接口GetExtManageInfo，从索引文件读出文件详细数据(含文件名和对应文件的stat数据)
 *           4：判断读出的文件名集合/文件详细数据记录个数是否和写入时相等
 *           5：判断读出的文件名集合/文件详细数据记录内容是否和写入时相等
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0200, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0200";
    try {
        TestManager tm("b_json_entity_ext_manage_0200");

        // 预置文件数据
        // 索引文件pathManageFile, 测试文件路径pathTestFile, 测试文件名testFileHexName
        string root = tm.GetRootDirCurTest();
        string pathManageFile = root + "manage.json";
        string pathTestFile = root + "test.txt";
        string testFileHexName = "1234567890abcdef";
        SaveStringToFile(pathTestFile, "hello world");
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(pathManageFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();

        // 生成一条有用数据并写入索引文件
        map<string, tuple<string, struct stat, bool>> info;
        struct stat sta = {};
        info.emplace(testFileHexName, make_tuple(pathTestFile, sta = GetFileStat(pathTestFile), true));
        cache.SetExtManage(info);

        // 读取索引文件内容并做结果判断
        auto fileNames = cache.GetExtManage();
        ASSERT_EQ(fileNames.size(), 1ul);
        EXPECT_EQ(*fileNames.begin(), testFileHexName);
        auto fileInfo = cache.GetExtManageInfo();
        ASSERT_EQ(fileInfo.size(), 1ul);
        EXPECT_EQ(fileInfo[0].hashName, testFileHexName);
        EXPECT_EQ(fileInfo[0].fileName, pathTestFile);
        EXPECT_TRUE(IsEqual(fileInfo[0].sta, sta));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0200";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0300
 * @tc.name: b_json_entity_ext_manage_0300
 * @tc.desc: 通过向接口SetExtManage传入包含三条信息的map参数，测试对索引文件的操作是否正确。
 *           0：通过向索引文件写入3条有效数据模拟对索引文件的(无穷)内容的测试覆盖
 *           1：调用接口SetExtManage，向索引文件写入数据
 *           2：调用接口GetExtManage，从索引文件读出文件名数据
 *           3：调用接口GetExtManageInfo，从索引文件读出文件详细数据(含文件名和对应文件的stat数据)
 *           4：判断读出的文件名集合/文件详细数据记录个数是否和写入时相等
 *           5：判断读出的文件名集合/文件详细数据记录内容是否和写入时相等
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0300, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0300";
    try {
        TestManager tm("b_json_entity_ext_manage_0300");

        // 预置文件数据
        // 索引文件pathManageFile1/2/3, 测试文件路径pathTestFile1/2/3, 测试文件名testFile1/2/3HexName
        string root = tm.GetRootDirCurTest();
        string pathManageFile = root + "manage.json";
        string pathTestFile1 = root + "test1.txt";
        string pathTestFile2 = root + "test2.txt";
        string pathTestFile3 = root + "test3.txt";
        string testFile1HexName = "1234567890abcde1";
        string testFile2HexName = "1234567890abcde2";
        string testFile3HexName = "1234567890abcde3";
        SaveStringToFile(pathTestFile1, "h");
        SaveStringToFile(pathTestFile2, "hello");
        SaveStringToFile(pathTestFile3, "hello world");
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(pathManageFile.data(), O_RDONLY, 0)));
        auto newCache = cachedEntity.Structuralize();

        // 生成三条有用数据并写入索引文件
        map<string, tuple<string, struct stat, bool>> info;
        info.emplace(testFile1HexName, make_tuple(pathTestFile1, GetFileStat(pathTestFile1), true));
        info.emplace(testFile2HexName, make_tuple(pathTestFile2, GetFileStat(pathTestFile2), true));
        info.emplace(testFile3HexName, make_tuple(pathTestFile3, GetFileStat(pathTestFile3), true));
        newCache.SetExtManage(info);

        // 预置结果集，用以在读取索引文件后做结果判断
        set<string> resultFileName {testFile1HexName, testFile2HexName, testFile3HexName};

        // 读取索引文件内容并做结果判断
        auto fileNames = newCache.GetExtManage();
        EXPECT_EQ(fileNames.size(), info.size());
        EXPECT_EQ(fileNames, resultFileName);
        auto fileInfo = newCache.GetExtManageInfo();
        EXPECT_EQ(fileInfo.size(), info.size());
        EXPECT_TRUE(IsEqual(fileInfo, info));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0300";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0400
 * @tc.name: b_json_entity_ext_manage_0400
 * @tc.desc: 通过向接口SetExtManage传入包含三条信息的map参数，测试对索引文件的操作是否正确。
 *           0：通过向索引文件写入3条有效数据模拟覆盖对索引文件的(无穷)内容的测试覆盖
 *           0：通过向索引文件的记录一写入0条、记录二写入1条、记录三写入2条有效硬链接数据模拟对索引文件含
 *              有硬链接(空、有、无穷)个的测试覆盖
 *           1：调用接口SetExtManage，向索引文件写入数据
 *           2：调用接口GetExtManage，从索引文件读出文件名数据
 *           3：调用接口GetExtManageInfo，从索引文件读出文件详细数据(含文件名和对应文件的stat数据)
 *           4：判断读出的文件名集合/文件详细数据记录个数是否和写入时相等
 *           5：判断读出的文件名集合/文件详细数据记录内容是否和写入时相等
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0400, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0400";
    try {
        TestManager tm("b_json_entity_ext_manage_0400");

        // 预置文件数据
        // 索引文件pathManageFile1/2/3, 测试文件路径pathTestFile1/2/3, 测试文件名testFile1/2/3HexName
        string root = tm.GetRootDirCurTest();
        string pathManageFile = root + "manage.json";
        string pathTestFile1 = root + "test4.txt";
        string pathTestFile2 = root + "test5.txt";
        string pathTestFile3 = root + "test6.txt";
        string testFile1HexName = "1234567890abcde4";
        string testFile2HexName = "1234567890abcde5";
        string testFile3HexName = "1234567890abcde6";
        SaveStringToFile(pathTestFile1, "h");
        SaveStringToFile(pathTestFile2, "hello");
        SaveStringToFile(pathTestFile3, "hello world");
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(pathManageFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();

        // 生成三条有用数据并写入索引文件
        map<string, tuple<string, struct stat, bool>> info;
        info.emplace(testFile1HexName, make_tuple(pathTestFile1, GetFileStat(pathTestFile1), true));
        info.emplace(testFile2HexName, make_tuple(pathTestFile2, GetFileStat(pathTestFile2), true));
        info.emplace(testFile3HexName, make_tuple(pathTestFile3, GetFileStat(pathTestFile3), true));
        cache.SetExtManage(info);

        // 预置结果集，用以在读取索引文件后做结果判断
        set<string> resultFileName {testFile1HexName, testFile2HexName, testFile3HexName};

        // 读取索引文件内容并做结果判断
        auto fileNames = cache.GetExtManage();
        EXPECT_EQ(fileNames.size(), info.size());
        EXPECT_EQ(fileNames, resultFileName);
        auto fileInfo = cache.GetExtManageInfo();
        EXPECT_EQ(fileInfo.size(), info.size());
        EXPECT_TRUE(IsEqual(fileInfo, info));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0400";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0500
 * @tc.name: b_json_entity_ext_manage_0500
 * @tc.desc: 通过向接口SetExtManage传入包含三条信息的map参数，测试对索引文件的操作是否正确。
 *           0：通过向索引文件写入3条有效数据模拟覆盖对索引文件的(无穷)内容的测试覆盖
 *           0：通过向索引文件的记录一写入0条、记录二写入1条、记录三写入2条有效硬链接数据模拟对索引文件含
 *              有硬链接(空、有、无穷)个的测试覆盖
 *           0：通过传入和源文件相同stat信息向索引文件中对应记录添加硬链接
 *           1：调用接口SetExtManage，向索引文件写入数据
 *           2：调用接口GetExtManage，从索引文件读出文件名数据
 *           3：调用接口GetExtManageInfo，从索引文件读出文件详细数据(含文件名和对应文件的stat数据)
 *           4：判断读出的文件名集合/文件详细数据记录个数是否和写入时相等
 *           5：判断读出的文件名集合/文件详细数据记录内容是否和写入时相等
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0500, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0500";
    try {
        // 预置文件数据
        // 索引文件pathManageFile1/2/3, 测试文件路径pathTestFile1/2/3, 测试文件名testFile1/2/3HexName
        TestManager tm("b_json_entity_ext_manage_0500");
        string root = tm.GetRootDirCurTest();
        string pathManageFile = root + "manage.json";
        string pathTestFile1 = root + "test7.txt";
        string pathTestFile2 = root + "test8.txt";
        string pathTestFile3 = root + "test9.txt";
        string testFile1HexName = "1234567890abcde7";
        string testFile2HexName = "1234567890abcde8";
        string testFile3HexName = "1234567890abcde9";
        SaveStringToFile(pathTestFile1, "h");
        SaveStringToFile(pathTestFile2, "hello");
        SaveStringToFile(pathTestFile3, "hello world");
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(pathManageFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();

        // 生成三条有用数据并写入索引文件
        map<string, tuple<string, struct stat, bool>> info;
        struct stat sta = {};
        info.emplace(testFile1HexName, make_tuple(pathTestFile1, GetFileStat(pathTestFile1), true));
        info.emplace(testFile2HexName, make_tuple(pathTestFile2, sta = GetFileStat(pathTestFile2), true));
        info.emplace(testFile3HexName, make_tuple(pathTestFile3, sta = GetFileStat(pathTestFile3), true));
        cache.SetExtManage(info);

        // 预置结果集，用以在读取索引文件后做结果判断
        set<string> resultFileName {testFile1HexName, testFile2HexName, testFile3HexName};

        // 读取索引文件内容并做结果判断
        auto fileNames = cache.GetExtManage();
        EXPECT_EQ(fileNames.size(), 3ul);
        EXPECT_EQ(fileNames, resultFileName);
        auto fileInfo = cache.GetExtManageInfo();
        EXPECT_EQ(fileInfo.size(), info.size());
        EXPECT_TRUE(IsEqual(fileInfo, info));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0500";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0700
 * @tc.name: b_json_entity_ext_manage_0700
 * @tc.desc: 测试GetExtManageInfo在Json数据不为数组时能否成功返回空map
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0700, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0700";
    try {
        string_view sv = R"({"key":1})";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        auto mp = cache.GetExtManageInfo();
        EXPECT_TRUE(mp.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0700";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0800
 * @tc.name: b_json_entity_ext_manage_0800
 * @tc.desc: 测试GetExtManageInfo在Json数据为数组且仅有一个键不为information的对象时能否成功返回空map
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0800, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0800";
    try {
        string_view sv = R"([{"key":1}])";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        auto mp = cache.GetExtManageInfo();
        EXPECT_TRUE(mp.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0800";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0801
 * @tc.name: b_json_entity_ext_manage_0801
 * @tc.desc: 测试GetExtManageInfo在Json数据为数组且有一个键为information的对象时能否成功返回空map
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0801, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0801";
    try {
        string_view sv = R"([{"path":1}])";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        auto mp = cache.GetExtManageInfo();
        EXPECT_TRUE(mp.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0801";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0802
 * @tc.name: b_json_entity_ext_manage_0802
 * @tc.desc: 测试GetExtManageInfo在Json数据为数组且有两个键为information的对象时能否成功返回空map
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0802, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0802";
    try {
        string_view sv = R"([{"path":1}, {"fileName":1}])";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        auto mp = cache.GetExtManageInfo();
        EXPECT_TRUE(mp.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0802";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0803
 * @tc.name: b_json_entity_ext_manage_0803
 * @tc.desc: 测试GetExtManageInfo在Json数据为数组且键为空时能否成功返回空map
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0803, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0803";
    try {
        string_view sv = R"([{"":1}])";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        auto mp = cache.GetExtManageInfo();
        EXPECT_TRUE(mp.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0803";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0804
 * @tc.name: b_json_entity_ext_manage_0804
 * @tc.desc: 测试GetExtManageInfo
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0804, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0804";
    try {
        string_view sv = R"([{"isBigFile":false}, {"fileName":"test"}])";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        auto mp = cache.GetExtManageInfo();
        EXPECT_TRUE(mp.empty());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0804";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0900
 * @tc.name: b_json_entity_ext_manage_0900
 * @tc.desc: 测试CheckBigFile中st_size的两种情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9JXNH
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0900";
    try {
        struct stat sta;
        sta.st_size = 0;
        bool ret = CheckBigFile(sta);
        EXPECT_FALSE(ret);

        sta.st_size = BConstants::BIG_FILE_BOUNDARY + 1;
        ret = CheckBigFile(sta);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0900";
}

string BExcepUltils::Canonicalize(const string_view &path)
{
    return string(path);
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0901
 * @tc.name: b_json_entity_ext_manage_0901
 * @tc.desc: 测试CheckOwnPackTar各种异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9JXNH
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0901, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0901";
    try {
        string fileName = string(BConstants::PATH_BUNDLE_BACKUP_HOME)
            .append(BConstants::SA_BUNDLE_BACKUP_BACKUP).append("/tar");
        auto ret = CheckOwnPackTar(fileName);
        EXPECT_FALSE(ret);

        fileName = string(BConstants::PATH_BUNDLE_BACKUP_HOME)
            .append(BConstants::SA_BUNDLE_BACKUP_BACKUP).append("/test.tar");
        ret = CheckOwnPackTar(fileName);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0901";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0902
 * @tc.name: b_json_entity_ext_manage_0902
 * @tc.desc: 测试SetExtManageForClone的各种情况
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9JXNH
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0902, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0902";
    try {
        TestManager tm("b_json_entity_ext_manage_0902");
        // 预置文件数据
        // 索引文件pathManageFile, 测试文件路径pathTestFile, 测试文件名testFileHexName
        string root = tm.GetRootDirCurTest();
        string pathManageFile = root + "manage.json";
        string pathTestFile = root + "test.txt";
        string testFileHexName = "1234567890abcdef";
        SaveStringToFile(pathTestFile, "hello world");
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(pathManageFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();

        // 生成一条有用数据并写入索引文件
        map<string, tuple<string, struct stat, bool, bool>> info;
        struct stat sta = {};
        info.emplace(testFileHexName, make_tuple(pathTestFile, sta = GetFileStat(pathTestFile), false, true));
        cache.SetExtManageForClone(info);

        // 读取索引文件内容并做结果判断
        auto fileNames = cache.GetExtManage();
        ASSERT_EQ(fileNames.size(), 1ul);
        EXPECT_EQ(*fileNames.begin(), testFileHexName);
        auto fileInfo = cache.GetExtManageInfo();
        ASSERT_EQ(fileInfo.size(), 1ul);
        EXPECT_EQ(fileInfo[0].hashName, testFileHexName);
        EXPECT_EQ(fileInfo[0].fileName, pathTestFile);
        EXPECT_TRUE(fileInfo[0].isUserTar);
        EXPECT_FALSE(fileInfo[0].isBigFile);
        EXPECT_TRUE(IsEqual(fileInfo[0].sta, sta));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0902";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0903
 * @tc.name: b_json_entity_ext_manage_0903
 * @tc.desc: 测试CheckOwnPackTar各种异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9JXNH
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0903, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0903";
    try {
        TestManager tm("b_json_entity_ext_manage_0903");
        // 预置文件数据
        // 索引文件pathManageFile, 测试文件路径pathTestFile, 测试文件名testFileHexName
        string root = tm.GetRootDirCurTest();
        string pathManageFile = root + "manage.json";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(pathManageFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        Json::Value value;
        value["isUserTar"] = true;
        value["information"]["test"] = "test";

        cache.obj_.clear();
        cache.obj_.append(value);
        // 读取索引文件内容并做结果判断
        auto fileNames = cache.GetExtManage();
        EXPECT_EQ(fileNames.size(), 1ul);
        EXPECT_EQ(*fileNames.begin(), "");
        auto infos = cache.GetExtManageInfo();
        EXPECT_EQ(infos.size(), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0903";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0904
 * @tc.name: b_json_entity_ext_manage_0904
 * @tc.desc: 测试CheckOwnPackTar各种异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9JXNH
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0904, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0904";
    try {
        string fileName = "/home/user/test.tar";
        auto ret = CheckOwnPackTar(fileName);
        EXPECT_FALSE(ret);

        fileName = string(BConstants::PATH_BUNDLE_BACKUP_HOME)
            .append(BConstants::SA_BUNDLE_BACKUP_BACKUP).append("/part1.tar");
        ret = CheckOwnPackTar(fileName);
        EXPECT_TRUE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0904";
}
} // namespace OHOS::FileManagement::Backup