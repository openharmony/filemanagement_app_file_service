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
HWTEST_F(BReportEntityTest, b_report_entity_GetReportInfos_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BReportEntityTest-begin b_report_entity_GetReportInfos_0100";
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
        unordered_map<string, struct ReportFileInfo> cloudFiles = cloudRp.GetReportInfos();

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
} // namespace OHOS::FileManagement::Backup