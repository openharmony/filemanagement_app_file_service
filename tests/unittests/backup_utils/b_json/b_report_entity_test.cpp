/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>
#include <fcntl.h>
#include <file_ex.h>

#include "b_json/b_report_entity.h"
#include "test_manager.h"

#include "src/b_json/b_report_entity.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BReportEntityTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @brief 创建测试文件
 *
 * @return tuple<bool, string, string> 创建结果、文件路径、文件内容
 */
static tuple<string, string> GetTestFile(const TestManager &tm, const string content)
{
    string path = tm.GetRootDirCurTest();
    string filePath = path + "temp.txt";
    
    if (bool contentCreate = SaveStringToFile(filePath, content, true); !contentCreate) {
        throw system_error(errno, system_category());
    }
    return {filePath, content};
}

/**
 * @tc.number: SUB_backup_b_report_entity_GetReportInfos_0100
 * @tc.name: b_report_entity_GetReportInfos_0100
 * @tc.desc: Test function of GetReportInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(BReportEntityTest, b_report_entity_GetReportInfos_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BReportEntityTest-begin b_report_entity_GetReportInfos_0100";
    try {
        string fileName = "/a.txt";
        string mode = "0644";
        string isDir = "0";
        string size = "1";
        string mtime = "1501927260";
        string hash = "ASDasadSDASDA";
        string isIncremental = "1";

        string content = "version=1.0&attrNum=6\r\npath;mode;dir;size;mtime;hash;isIncremetal\r\n";
        content += fileName + ";" + mode + ";" + isDir + ";" + size + ";" + mtime + ";" + hash + ";" + isIncremental;
        TestManager tm(__func__);
        const auto [filePath, res] = GetTestFile(tm, content);

        BReportEntity cloudRp(UniqueFd(open(filePath.data(), O_RDONLY, 0)));
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        cloudRp.GetReportInfos(cloudFiles);

        bool flag = false;
        fileName = fileName.substr(1, fileName.length() - 1);
        EXPECT_EQ(cloudFiles.size(), 1);
        for (auto &item : cloudFiles) {
            if (item.first == fileName) {
                EXPECT_EQ(item.first, fileName);
                EXPECT_EQ(item.second.mode, mode);
                EXPECT_EQ(to_string(item.second.isDir), isDir);
                EXPECT_EQ(to_string(item.second.size), size);
                EXPECT_EQ(to_string(item.second.mtime), mtime);
                EXPECT_EQ(item.second.hash, hash);
                EXPECT_EQ(to_string(item.second.isIncremental), isIncremental);

                flag = true;
                break;
            }
        }

        EXPECT_TRUE(flag);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BReportEntityTest-an exception occurred by GetReportInfos.";
        e.what();
    }
    GTEST_LOG_(INFO) << "BReportEntityTest-end b_report_entity_GetReportInfos_0100";
}

/**
 * @tc.number: SUB_backup_b_report_entity_SplitStringByChar_0100
 * @tc.name: b_report_entity_SplitStringByChar_0100
 * @tc.desc: Test function of SplitStringByChar interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(BReportEntityTest, b_report_entity_SplitStringByChar_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BReportEntityTest-begin b_report_entity_SplitStringByChar_0100";
    try {
        string str = "";
        char sep = ATTR_SEP;
        vector<string> splits;
        SplitStringByChar(str, sep, splits);
        EXPECT_EQ(splits.size(), 0);

        str = "test;";
        SplitStringByChar(str, sep, splits);
        EXPECT_EQ(splits.size(), 2);

        str = "test";
        splits.clear();
        SplitStringByChar(str, sep, splits);
        EXPECT_EQ(splits.size(), 1);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BReportEntityTest-an exception occurred by SplitStringByChar. " << e.what();
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BReportEntityTest-end b_report_entity_SplitStringByChar_0100";
}

/**
 * @tc.number: SUB_backup_b_report_entity_ParseReportInfo_0100
 * @tc.name: b_report_entity_ParseReportInfo_0100
 * @tc.desc: Test function of ParseReportInfo interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(BReportEntityTest, b_report_entity_ParseReportInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BReportEntityTest-begin b_report_entity_ParseReportInfo_0100";
    try {
        struct ReportFileInfo fileStat;
        vector<string> splits;
        std::vector<std::string> keys;
        int testLen = 1;
        auto err = ParseReportInfo(fileStat, splits, testLen);
        EXPECT_EQ(err, EPERM);

        splits.emplace_back("test");
        testLen = 1;
        err = ParseReportInfo(fileStat, splits, testLen);
        EXPECT_EQ(err, EPERM);

        fileStat = {};
        splits = {"/test", "0", "0", "0", "0", "0", "0"};
        err = ParseReportInfo(fileStat, splits, splits.size());
        EXPECT_EQ(err, ERR_OK);

        splits = {"test", "0", "1", "0", "0", "0", "1", "1"};
        err = ParseReportInfo(fileStat, splits, splits.size());
        EXPECT_EQ(err, ERR_OK);

        splits = {"test", "0", "1", "test", "test", "0", "1", "1"};
        err = ParseReportInfo(fileStat, splits, splits.size());
        EXPECT_EQ(err, ERR_OK);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BReportEntityTest-an exception occurred by ParseReportInfo." << e.what();
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BReportEntityTest-end b_report_entity_ParseReportInfo_0100";
}

/**
 * @tc.number: SUB_backup_b_report_entity_DealLine_0100
 * @tc.name: b_report_entity_DealLine_0100
 * @tc.desc: Test function of DealLine interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(BReportEntityTest, b_report_entity_DealLine_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BReportEntityTest-begin b_report_entity_DealLine_0100";
    try {
        std::vector<string> keys;
        int num = 1;
        string line = "test\r";
        unordered_map<string, struct ReportFileInfo> infos;
        DealLine(keys, num, line, infos);
        EXPECT_EQ(keys.size(), 1);

        num = INFO_ALIGN_NUM;
        keys.clear();
        line = "\r";
        DealLine(keys, num, line, infos);
        EXPECT_EQ(infos.size(), 0);

        line = "/test;0;0;0;0;0;0;0\r";
        keys = Data_Header;
        DealLine(keys, num, line, infos);
        EXPECT_EQ(infos.size(), 1);

        line = "/test;0;0;0;0;0;0\r";
        keys.resize(keys.size() - 1);
        DealLine(keys, num, line, infos);
        EXPECT_EQ(infos.size(), 1);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BReportEntityTest-an exception occurred by DealLine. " << e.what();
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BReportEntityTest-end b_report_entity_DealLine_0100";
}

/**
 * @tc.number: SUB_backup_b_report_entity_DealLine_0101
 * @tc.name: b_report_entity_DealLine_0101
 * @tc.desc: Test function of DealLine interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(BReportEntityTest, b_report_entity_DealLine_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BReportEntityTest-begin b_report_entity_DealLine_0101";
    try {
        std::vector<string> keys;
        int num = 0;
        string line = "";
        unordered_map<string, struct ReportFileInfo> infos;
        DealLine(keys, num, line, infos);
        EXPECT_EQ(infos.size(), 0);
        EXPECT_EQ(keys.size(), 0);

        num = 1;
        DealLine(keys, num, line, infos);
        EXPECT_EQ(infos.size(), 0);
        EXPECT_EQ(keys.size(), 0);

        num = 2;
        line = "";
        DealLine(keys, num, line, infos);
        EXPECT_EQ(infos.size(), 0);
        EXPECT_EQ(keys.size(), 0);

        num = 2;
        line = "test";
        DealLine(keys, num, line, infos);
        EXPECT_EQ(infos.size(), 0);
        EXPECT_EQ(keys.size(), 0);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BReportEntityTest-an exception occurred by DealLine. " << e.what();
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BReportEntityTest-end b_report_entity_DealLine_0101";
}

/**
 * @tc.number: SUB_backup_b_report_entity_GetStorageReportInfos_0100
 * @tc.name: b_report_entity_GetStorageReportInfos_0100
 * @tc.desc: Test function of GetStorageReportInfos interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(BReportEntityTest, b_report_entity_GetStorageReportInfos_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BReportEntityTest-begin b_report_entity_GetStorageReportInfos_0100";
    try {
        string fileName = "/a.txt";
        string mode = "0644";
        string isDir = "0";
        string size = "1";
        string mtime = "1501927260";
        string hash = "ASDasadSDASDA";
        string isIncremental = "1";

        string content = "version=1.0&attrNum=6\r\npath;mode;dir;size;mtime;hash;isIncremental\r\n";
        content += fileName + ";" + mode + ";" + isDir + ";" + size + ";" + mtime + ";" + hash + ";" + isIncremental;
        TestManager tm(__func__);
        const auto [filePath, res] = GetTestFile(tm, content);
        BReportEntity cloudRp(UniqueFd(open(filePath.data(), O_RDONLY, 0)));

        unordered_map<string, struct ReportFileInfo> localFilesInfo;
        bool ret = cloudRp.GetStorageReportInfos(localFilesInfo);
        EXPECT_TRUE(ret);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BReportEntityTest-an exception occurred by GetStorageReportInfos.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BReportEntityTest-end b_report_entity_GetStorageReportInfos_0100";
}

/**
 * @tc.number: SUB_backup_b_report_entity_CheckAndUpdateIfReportLineEncoded_0100
 * @tc.name: b_report_entity_CheckAndUpdateIfReportLineEncoded_0100
 * @tc.desc: Test function of CheckAndUpdateIfReportLineEncoded interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(BReportEntityTest, b_report_entity_CheckAndUpdateIfReportLineEncoded_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BReportEntityTest-begin b_report_entity_CheckAndUpdateIfReportLineEncoded_0100";
    try {
        string fileName = "/a.txt";
        string mode = "0644";
        string isDir = "0";
        string size = "1";
        string mtime = "1501927260";
        string hash = "ASDasadSDASDA";

        string content = "version=1.0&attrNum=6\r\npath;mode;dir;size;mtime;hash\r\n";
        content += fileName + ";" + mode + ";" + isDir + ";" + size + ";" + mtime + ";" + hash;
        TestManager tm(__func__);
        const auto [filePath, res] = GetTestFile(tm, content);
        BReportEntity cloudRp(UniqueFd(open(filePath.data(), O_RDONLY, 0)));

        std::string path;
        cloudRp.CheckAndUpdateIfReportLineEncoded(path);
        path = filePath;
        cloudRp.CheckAndUpdateIfReportLineEncoded(path);
        EXPECT_TRUE(true);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BReportEntityTest-an exception occurred by CheckAndUpdateIfReportLineEncoded.";
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BReportEntityTest-end b_report_entity_CheckAndUpdateIfReportLineEncoded_0100";
}

/**
 * @tc.number: SUB_backup_b_report_entity_EncodeReportItem_0100
 * @tc.name: b_report_entity_EncodeReportItem_0100
 * @tc.desc: Test function of EncodeReportItem interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(BReportEntityTest, b_report_entity_EncodeReportItem_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BReportEntityTest-begin b_report_entity_EncodeReportItem_0100";
    try {
        string fileName = "/a.txt";
        string mode = "0644";
        string isDir = "0";
        string size = "1";
        string mtime = "1501927260";
        string hash = "ASDasadSDASDA";

        string content = "version=1.0&attrNum=6\r\npath;mode;dir;size;mtime;hash\r\n";
        content += fileName + ";" + mode + ";" + isDir + ";" + size + ";" + mtime + ";" + hash;
        TestManager tm(__func__);
        const auto [filePath, res] = GetTestFile(tm, content);
        BReportEntity cloudRp(UniqueFd(open(filePath.data(), O_RDONLY, 0)));

        const std::string reportItem = "test";
        bool enableEncode = false;
        std::string ret = cloudRp.EncodeReportItem(reportItem, enableEncode);
        EXPECT_EQ(ret, reportItem);
        ret = cloudRp.DecodeReportItem(reportItem, enableEncode);
        EXPECT_EQ(ret, reportItem);

        enableEncode = true;
        ret = cloudRp.EncodeReportItem(reportItem, enableEncode);
        EXPECT_EQ(ret, reportItem);
        ret = cloudRp.DecodeReportItem(reportItem, enableEncode);
        EXPECT_EQ(ret, reportItem);
    } catch (const exception &e) {
        GTEST_LOG_(INFO) << "BReportEntityTest-an exception occurred by EncodeReportItem. " << e.what();
        EXPECT_TRUE(false);
    }
    GTEST_LOG_(INFO) << "BReportEntityTest-end b_report_entity_EncodeReportItem_0100";
}
} // namespace OHOS::FileManagement::Backup