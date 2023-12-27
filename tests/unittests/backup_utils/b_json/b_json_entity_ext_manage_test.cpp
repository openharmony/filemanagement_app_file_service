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
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0300";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0400
 * @tc.name: b_json_entity_ext_manage_0400
 * @tc.desc: 通过向接口SetExtManage传入包含三条信息的map参数，测试对索引文件的操作是否正确。
 *           0：通过向索引文件写入3条有效数据模拟覆盖对索引文件的(无穷)内容的测试覆盖
 *           0：通过向索引文件的记录一写入0条、记录二写入1条、记录三写入2条有效硬链接数据模拟对索引文件含
 *              有硬链接(空、有、无穷)个的测试覆盖
 *           0：通过调用接口SetHardLinkInfo向索引文件中对应记录添加硬链接
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
        auto cache = cachedEntity.Structuralize();

        // 生成三条有用数据并写入索引文件
        map<string, tuple<string, struct stat, bool>> info;
        info.emplace(testFile1HexName, make_tuple(pathTestFile1, GetFileStat(pathTestFile1), true));
        info.emplace(testFile2HexName, make_tuple(pathTestFile2, GetFileStat(pathTestFile2), true));
        info.emplace(testFile3HexName, make_tuple(pathTestFile3, GetFileStat(pathTestFile3), true));
        cache.SetExtManage(info);

        // 向索引文件中的三条记录分别追加0、1、2条硬链接信息
        set<string> hardLinks1, hardLinks2, hardLinks3;
        cache.SetHardLinkInfo(testFile1HexName, hardLinks1);
        hardLinks2.emplace(root + "testFile2hardlink1");
        cache.SetHardLinkInfo(testFile2HexName, hardLinks2);
        hardLinks3.emplace(root + "testFile3hardlink1");
        hardLinks3.emplace(root + "testFile3hardlink2");
        cache.SetHardLinkInfo(testFile3HexName, hardLinks3);

        // 预置结果集，用以在读取索引文件后做结果判断
        set<string> resultFileName {testFile1HexName, testFile2HexName, testFile3HexName};

        // 读取索引文件内容并做结果判断
        auto fileNames = cache.GetExtManage();
        EXPECT_EQ(fileNames.size(), info.size());
        EXPECT_EQ(fileNames, resultFileName);
        auto fileInfo = cache.GetExtManageInfo();
        EXPECT_EQ(fileInfo.size(), info.size());
        EXPECT_TRUE(IsEqual(fileInfo, info));
        // 传入无效文件名"00000000"，测试读取文件硬链接接口是否正确返回
        auto testFile0HardLinks = cache.GetHardLinkInfo("00000000");
        EXPECT_TRUE(testFile0HardLinks.empty());
        auto testFile1HardLinks = cache.GetHardLinkInfo(testFile1HexName);
        EXPECT_TRUE(testFile1HardLinks.empty());
        auto testFile2HardLinks = cache.GetHardLinkInfo(testFile2HexName);
        EXPECT_EQ(testFile2HardLinks, hardLinks2);
        auto testFile3HardLinks = cache.GetHardLinkInfo(testFile3HexName);
        EXPECT_EQ(testFile3HardLinks, hardLinks3);
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
        auto cache = cachedEntity.Structuralize();

        // 生成三条有用数据并写入索引文件
        // 通过重用原始文件的stat向该记录追加(0/1/2)条硬链接文件信息
        map<string, tuple<string, struct stat, bool>> info;
        struct stat sta = {};
        info.emplace(testFile1HexName, make_tuple(pathTestFile1, GetFileStat(pathTestFile1), true));
        info.emplace(testFile2HexName, make_tuple(pathTestFile2, sta = GetFileStat(pathTestFile2), true));
        info.emplace("testFile2hardlink1", make_tuple(root + "testFile2hardlink1", sta, true));
        info.emplace(testFile3HexName, make_tuple(pathTestFile3, sta = GetFileStat(pathTestFile3), true));
        info.emplace("testFile3hardlink1", make_tuple(root + "testFile3hardlink1", sta, true));
        info.emplace("testFile3hardlink2", make_tuple(root + "testFile3hardlink2", sta, true));
        cache.SetExtManage(info);

        // 预置结果集，用以在读取索引文件后做结果判断
        // 将info中的硬链接信息删除，保留原始文件信息，作为后续结果值判断的比较对象
        info.erase("testFile2hardlink1");
        info.erase("testFile3hardlink1");
        info.erase("testFile3hardlink2");
        set<string> hardLinks2 {root + "testFile2hardlink1"};
        set<string> hardLinks3 {root + "testFile3hardlink1", root + "testFile3hardlink2"};
        set<string> resultFileName {testFile1HexName, testFile2HexName, testFile3HexName};

        // 读取索引文件内容并做结果判断
        auto fileNames = cache.GetExtManage();
        EXPECT_EQ(fileNames.size(), 3ul);
        EXPECT_EQ(fileNames, resultFileName);
        auto fileInfo = cache.GetExtManageInfo();
        EXPECT_EQ(fileInfo.size(), info.size());
        EXPECT_TRUE(IsEqual(fileInfo, info));
        auto testFile1HardLinks = cache.GetHardLinkInfo(testFile1HexName);
        EXPECT_TRUE(testFile1HardLinks.empty());
        auto testFile2HardLinks = cache.GetHardLinkInfo(testFile2HexName);
        EXPECT_EQ(testFile2HardLinks, hardLinks2);
        auto testFile3HardLinks = cache.GetHardLinkInfo(testFile3HexName);
        EXPECT_EQ(testFile3HardLinks, hardLinks3);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0500";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0600
 * @tc.name: b_json_entity_ext_manage_0600
 * @tc.desc: 测试SetExtManage接口中的FindLinks在设备号或INode数目为0时能否成功通过GetExtManage获取相关信息
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0600, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0600";
    try {
        map<string, tuple<string, struct stat, bool>> mp = {{"key", {"first", {}, true}}};
        Json::Value jv;
        BJsonEntityExtManage extMg(jv);

        extMg.SetExtManage(mp);
        set<string> ss = extMg.GetExtManage();
        EXPECT_EQ(ss.size(), 1);

        std::get<INDEX_SECOND>(mp.at("key")).st_dev = 1;
        extMg.SetExtManage(mp);
        ss = extMg.GetExtManage();
        EXPECT_EQ(ss.size(), 1);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0600";
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
 * @tc.number: SUB_backup_bb_json_entity_ext_manage_0900
 * @tc.name: b_json_entity_ext_manage_0900
 * @tc.desc: 测试SetHardLinkInfo接口和GetHardLinkInfo接口在不符合相关条件时能否成功返回false和空set
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0900, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0900";
    try {
        string_view sv = R"({"key":1})";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.SetHardLinkInfo("", {}));

        Json::Value jv;
        BJsonEntityExtManage extMg(jv);
        EXPECT_FALSE(extMg.SetHardLinkInfo("1", {}));

        EXPECT_FALSE(cache.SetHardLinkInfo("1", {}));

        EXPECT_EQ(cache.GetHardLinkInfo(""), set<string>());

        EXPECT_EQ(extMg.GetHardLinkInfo("1"), set<string>());

        EXPECT_EQ(cache.GetHardLinkInfo("1"), set<string>());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0900";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0901
 * @tc.name: b_json_entity_ext_manage_0901
 * @tc.desc: 测试SetHardLinkInfo接口和GetHardLinkInfo接口在不符合相关条件时能否成功返回false和空set
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0901, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0901";
    try {
        string_view sv = R"({"key":1})";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.SetHardLinkInfo("#4$5%", {}));

        Json::Value jv;
        BJsonEntityExtManage extMg(jv);
        EXPECT_FALSE(extMg.SetHardLinkInfo("1", {}));

        EXPECT_FALSE(cache.SetHardLinkInfo("1", {}));

        EXPECT_EQ(cache.GetHardLinkInfo("#4$5%"), set<string>());

        EXPECT_EQ(extMg.GetHardLinkInfo("1"), set<string>());

        EXPECT_EQ(cache.GetHardLinkInfo("1"), set<string>());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0901";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0902
 * @tc.name: b_json_entity_ext_manage_0902
 * @tc.desc: 测试SetHardLinkInfo接口和GetHardLinkInfo接口在不符合相关条件时能否成功返回false和空set
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0902, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0902";
    try {
        string_view sv = R"({"key":1})";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.SetHardLinkInfo("测试代码", {}));

        Json::Value jv;
        BJsonEntityExtManage extMg(jv);
        EXPECT_FALSE(extMg.SetHardLinkInfo("1", {}));

        EXPECT_FALSE(cache.SetHardLinkInfo("1", {}));

        EXPECT_EQ(cache.GetHardLinkInfo("测试代码"), set<string>());

        EXPECT_EQ(extMg.GetHardLinkInfo("1"), set<string>());

        EXPECT_EQ(cache.GetHardLinkInfo("1"), set<string>());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0902";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0903
 * @tc.name: b_json_entity_ext_manage_0903
 * @tc.desc: 测试SetHardLinkInfo接口和GetHardLinkInfo接口在不符合相关条件时能否成功返回false和空set
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0903, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0903";
    try {
        string_view sv = R"({"key":1})";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.SetHardLinkInfo("ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDA\
                                            BCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD", {}));

        Json::Value jv;
        BJsonEntityExtManage extMg(jv);
        EXPECT_FALSE(extMg.SetHardLinkInfo("1", {}));

        EXPECT_FALSE(cache.SetHardLinkInfo("1", {}));

        EXPECT_EQ(cache.GetHardLinkInfo("ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDA\
                                            BCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD"), set<string>());

        EXPECT_EQ(extMg.GetHardLinkInfo("1"), set<string>());

        EXPECT_EQ(cache.GetHardLinkInfo("1"), set<string>());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0903";
}

/**
 * @tc.number: SUB_backup_b_json_entity_ext_manage_0904
 * @tc.name: b_json_entity_ext_manage_0904
 * @tc.desc: 测试SetHardLinkInfo接口和GetHardLinkInfo接口在不符合相关条件时能否成功返回false和空set
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: I6F3GV
 */
HWTEST_F(BJsonEntityExtManageTest, b_json_entity_ext_manage_0904, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-begin b_json_entity_ext_manage_0904";
    try {
        string_view sv = R"({"key":1})";
        BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(sv);
        auto cache = cachedEntity.Structuralize();
        EXPECT_FALSE(cache.SetHardLinkInfo("", {""""""""""""""""""""""""""""""""""""""""""""""""""}));

        Json::Value jv;
        BJsonEntityExtManage extMg(jv);
        EXPECT_FALSE(extMg.SetHardLinkInfo("测试代码", {}));

        EXPECT_FALSE(cache.SetHardLinkInfo("#4$5%", {}));

        EXPECT_EQ(cache.GetHardLinkInfo(""), set<string>());

        EXPECT_EQ(extMg.GetHardLinkInfo("测试代码"), set<string>());

        EXPECT_EQ(cache.GetHardLinkInfo("#4$5%"), set<string>());
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityExtManageTest-end b_json_entity_ext_manage_0904";
}

} // namespace OHOS::FileManagement::Backup