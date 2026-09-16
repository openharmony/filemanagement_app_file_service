/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <filesystem>
#include <fstream>
#include <limits.h>

#include <fcntl.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include "b_filesystem/b_file.h"
#include "b_json/b_report_entity.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
namespace fs = std::filesystem;

class BFileBranchTest : public testing::Test {};

HWTEST_F(BFileBranchTest, b_file_ReadFile_Branches_001, testing::ext::TestSize.Level1)
{
    EXPECT_ANY_THROW(BFile::ReadFile(UniqueFd(-1)));

    TestManager tm(__func__);
    string emptyPath = tm.GetRootDirCurTest() + "empty";
    UniqueFd emptyFd(open(emptyPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600));
    ASSERT_NE(emptyFd.Get(), -1);
    auto content = BFile::ReadFile(emptyFd);
    ASSERT_NE(content, nullptr);
    EXPECT_EQ(content[0], '\0');
}

HWTEST_F(BFileBranchTest, b_file_SendFileAndWrite_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    string root = tm.GetRootDirCurTest();
    string inputPath = root + "input";
    string outputPath = root + "output";
    {
        ofstream input(inputPath);
        input << "content";
    }
    UniqueFd inputFd(open(inputPath.c_str(), O_RDONLY));
    UniqueFd outputFd(open(outputPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600));
    ASSERT_NE(inputFd.Get(), -1);
    ASSERT_NE(outputFd.Get(), -1);
    EXPECT_ANY_THROW(BFile::SendFile(-1, inputFd.Get()));
    EXPECT_ANY_THROW(BFile::SendFile(outputFd.Get(), -1));
    EXPECT_ANY_THROW(BFile::Write(UniqueFd(-1), "data"));

    BFile::SendFile(outputFd.Get(), inputFd.Get());
    EXPECT_EQ(string(BFile::ReadFile(outputFd).get()), "content");
    BFile::Write(outputFd, "short");
    EXPECT_EQ(string(BFile::ReadFile(outputFd).get()), "short");
}

HWTEST_F(BFileBranchTest, b_file_CopyFile_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    fs::path root = tm.GetRootDirCurTest();
    fs::path source = root / "source";
    fs::path target = root / "target";
    fs::path targetDir = root / "target_dir";
    fs::create_directories(targetDir);
    {
        ofstream file(source);
        file << "copy-data";
    }

    EXPECT_TRUE(BFile::CopyFile(source.string(), source.string()));
    EXPECT_FALSE(BFile::CopyFile((root / "missing").string(), target.string()));
    EXPECT_FALSE(BFile::CopyFile(source.string(), (root / "missing_dir/out").string()));
    EXPECT_FALSE(BFile::CopyFile(source.string(), targetDir.string()));
    EXPECT_TRUE(BFile::CopyFile(source.string(), target.string()));
    EXPECT_TRUE(fs::exists(target));
}

HWTEST_F(BFileBranchTest, b_file_MoveFile_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    fs::path root = tm.GetRootDirCurTest();
    fs::path source = root / "source";
    fs::path target = root / "target";
    fs::path targetDir = root / "target_dir";
    fs::create_directories(targetDir);
    {
        ofstream file(source);
        file << "move-data";
    }

    string longPath(PATH_MAX, 'a');
    EXPECT_FALSE(BFile::MoveFile(longPath, target.string()));
    EXPECT_TRUE(BFile::MoveFile(source.string(), source.string()));
    EXPECT_FALSE(BFile::MoveFile((root / "missing").string(), target.string()));
    EXPECT_FALSE(BFile::MoveFile(source.string(), (root / "missing_dir/out").string()));
    EXPECT_FALSE(BFile::MoveFile(source.string(), targetDir.string()));
    EXPECT_TRUE(BFile::MoveFile(source.string(), target.string()));
    EXPECT_FALSE(fs::exists(source));
    EXPECT_TRUE(fs::exists(target));
}

HWTEST_F(BFileBranchTest, b_file_PathAndSize_Branches_001, testing::ext::TestSize.Level1)
{
    EXPECT_FALSE(BFile::EndsWith("abc", "abcd"));
    EXPECT_TRUE(BFile::EndsWith("abc", "bc"));
    EXPECT_TRUE(BFile::EndsWith("abc", ""));
    EXPECT_FALSE(BFile::EndsWith("abc", "ab"));

    TestManager tm(__func__);
    fs::path filePath = fs::path(tm.GetRootDirCurTest()) / "size";
    {
        ofstream file(filePath);
        file << "12345";
    }
    string realPath;
    EXPECT_TRUE(BFile::GetRealPath(filePath.string(), realPath));
    EXPECT_FALSE(realPath.empty());
    EXPECT_FALSE(BFile::GetRealPath((filePath.string() + ".missing"), realPath));
    int32_t error = 0;
    EXPECT_EQ(BFile::GetFileSize(filePath.string(), error), 5);
    EXPECT_EQ(BFile::GetFileSize(filePath.string() + ".missing", error), 0);
    EXPECT_NE(error, 0);
}

HWTEST_F(BFileBranchTest, b_file_WriteFile_Branches_001, testing::ext::TestSize.Level1)
{
    TestManager tm(__func__);
    fs::path root = tm.GetRootDirCurTest();
    BFile::WriteFile((root / "missing/report").string(), {});

    ReportFileInfo normal;
    normal.filePath = "/plain/path";
    normal.mode = "33188";
    normal.size = 10;
    normal.mtime = 20;
    normal.userTar = 1;
    normal.encodeFlag = false;
    ReportFileInfo compatible = normal;
    compatible.filePath = "/source/path";
    compatible.restorePath = "/restore/path";
    compatible.isDir = true;
    compatible.encodeFlag = true;
    fs::path reportPath = root / "report";
    BFile::WriteFile(reportPath.string(), {normal, compatible});

    ifstream report(reportPath);
    ASSERT_TRUE(report.is_open());
    vector<string> lines;
    string line;
    while (getline(report, line)) {
        lines.emplace_back(line);
    }
    ASSERT_EQ(lines.size(), 4);
    EXPECT_NE(lines[2].find("/plain/path"), string::npos);
    EXPECT_EQ(lines[2].back(), '0');
    EXPECT_EQ(lines[3].back(), '1');
}
} // namespace OHOS::FileManagement::Backup
