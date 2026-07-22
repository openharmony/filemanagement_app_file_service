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

#include <gtest/gtest.h>
#include "b_resources/b_constants.h"
#include "b_utils/string_utils.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class StringUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: ENDS_WITH_TEST_001
 * @tc.name: ENDS_WITH_TEST_001
 * @tc.desc: Test function of EndsWith
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(StringUtilsTest, ENDS_WITH_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin ENDS_WITH_TEST_001";
    GTEST_LOG_(INFO) << "1. test suffix invalid";
    std::string str = "/tmp/test1";
    std::string suffix1 = "/abc/tmp/test1";
    EXPECT_FALSE(StringUtils::EndsWith(str, suffix1));

    GTEST_LOG_(INFO) << "2. test suffix valid";
    std::string suffix2 = "test1";
    EXPECT_TRUE(StringUtils::EndsWith(str, suffix2));
    GTEST_LOG_(INFO) << "StringUtilsTest-end ENDS_WITH_TEST_001";
}

/**
 * @tc.number: SPLIT_TEST_001
 * @tc.name: SPLIT_TEST_001
 * @tc.desc: Test function of Split
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(StringUtilsTest, SPLIT_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin SPLIT_TEST_001";
    GTEST_LOG_(INFO) << "1. test delimiter invalid";
    std::string str = "abc:123:";
    std::string delimiter1 = "";
    EXPECT_EQ(StringUtils::Split(str, delimiter1).size(), 1);

    GTEST_LOG_(INFO) << "2. test delimiter valid";
    std::string delimiter2 = ":";
    EXPECT_EQ(StringUtils::Split(str, delimiter2).size(), 3);
    std::string delimiter3 = "123";
    EXPECT_EQ(StringUtils::Split(str, delimiter3).size(), 2);
    GTEST_LOG_(INFO) << "StringUtilsTest-end SPLIT_TEST_001";
}

/**
 * @tc.number: CONCAT_TEST_001
 * @tc.name: CONCAT_TEST_001
 * @tc.desc: Test function of Concat
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(StringUtilsTest, CONCAT_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin CONCAT_TEST_001";
    std::vector<std::string> strs = {"/tmp/test1", "test2"};
    std::string connector = "||";
    EXPECT_EQ(StringUtils::Concat(strs, connector), "/tmp/test1||test2||");
    GTEST_LOG_(INFO) << "StringUtilsTest-end CONCAT_TEST_001";
}

/**
 * @tc.number: PATH_ADD_DELIMITER_TEST_001
 * @tc.name: PATH_ADD_DELIMITER_TEST_001
 * @tc.desc: Test function of PathAddDelimiter
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(StringUtilsTest, PATH_ADD_DELIMITER_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin PATH_ADD_DELIMITER_TEST_001";
    GTEST_LOG_(INFO) << "1. test path empty";
    std::string path1 = "";
    EXPECT_EQ(StringUtils::PathAddDelimiter(path1), "/");

    GTEST_LOG_(INFO) << "2. test path valid";
    std::string path2 = "test1";
    EXPECT_EQ(StringUtils::PathAddDelimiter(path2), "test1/");
    std::string path3 = "test1/";
    EXPECT_EQ(StringUtils::PathAddDelimiter(path3), "test1/");
    GTEST_LOG_(INFO) << "StringUtilsTest-end PATH_ADD_DELIMITER_TEST_001";
}

/**
 * @tc.number: GEN_MAPPING_DIR_TEST_001
 * @tc.name: GEN_MAPPING_DIR_TEST_001
 * @tc.desc: Test function of GenMappingDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(StringUtilsTest, GEN_MAPPING_DIR_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin GEN_MAPPING_DIR_TEST_001";
    std::string back = "/tmp/test1";
    std::string restore = "test2";
    EXPECT_EQ(StringUtils::GenMappingDir(back, restore), "/tmp/test1||||test2||||");
    GTEST_LOG_(INFO) << "StringUtilsTest-end GEN_MAPPING_DIR_TEST_001";
}

/**
 * @tc.number: PARSE_MAPPING_DIR_TEST_001
 * @tc.name: PARSE_MAPPING_DIR_TEST_001
 * @tc.desc: Test function of ParseMappingDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(StringUtilsTest, PARSE_MAPPING_DIR_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin PARSE_MAPPING_DIR_TEST_001";
    GTEST_LOG_(INFO) << "1. test path not endswith separator";
    std::string path1 = "abc";
    auto [p1, p2] = StringUtils::ParseMappingDir(path1);
    EXPECT_EQ(p1, "abc");
    EXPECT_EQ(p2, "");
    GTEST_LOG_(INFO) << "StringUtilsTest-end PARSE_MAPPING_DIR_TEST_001";
}

/**
 * @tc.number: PARSE_MAPPING_DIR_TEST_002
 * @tc.name: PARSE_MAPPING_DIR_TEST_002
 * @tc.desc: Test function of ParseMappingDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(StringUtilsTest, PARSE_MAPPING_DIR_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin PARSE_MAPPING_DIR_TEST_002";
    GTEST_LOG_(INFO) << "2. test splits size not 3";
    std::string path2 = "abc||||123||||5678||||";
    auto [p1, p2] = StringUtils::ParseMappingDir(path2);
    EXPECT_EQ(p1, "abc||||123||||5678||||");
    EXPECT_EQ(p2, "");
    GTEST_LOG_(INFO) << "StringUtilsTest-end PARSE_MAPPING_DIR_TEST_002";
}

/**
 * @tc.number: PARSE_MAPPING_DIR_TEST_003
 * @tc.name: PARSE_MAPPING_DIR_TEST_003
 * @tc.desc: Test function of ParseMappingDir
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(StringUtilsTest, PARSE_MAPPING_DIR_TEST_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin PARSE_MAPPING_DIR_TEST_003";
    GTEST_LOG_(INFO) << "2. test splits size equal 3";
    std::string path3 = "abc||||123||||";
    auto [p1, p2] = StringUtils::ParseMappingDir(path3);
    EXPECT_EQ(p1, "abc");
    EXPECT_EQ(p2, "123");
    GTEST_LOG_(INFO) << "StringUtilsTest-end PARSE_MAPPING_DIR_TEST_003";
}

/**
 * @tc.number: GEN_HASH_NAME_TEST_001
 * @tc.name: GEN_HASH_NAME_TEST_001
 * @tc.desc: Test function of GenHashName
 * @tc.size: SMALL
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(StringUtilsTest, GEN_HASH_NAME_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin GEN_HASH_NAME_TEST_001";
    std::string str = "abcdef1234";
    auto hash = StringUtils::GenHashName(str);
    GTEST_LOG_(INFO) << hash;
    EXPECT_EQ(hash.length(), 16);
    GTEST_LOG_(INFO) << "StringUtilsTest-end GEN_HASH_NAME_TEST_001";
}

/**
* @tc.number: IS_SANBOX_ANCO_PATH_TEST_001
* @tc.name: IS_SANBOX_ANCO_PATH_TEST_001
* @tc.desc: Test function of IsSandboxAncoPath
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, IS_SANBOX_ANCO_PATH_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin IS_SANBOX_ANCO_PATH_TEST_001";
    // 完全匹配
    EXPECT_TRUE(StringUtils::IsSandboxAncoPath("/storage/Users/currentUser/HO_DATA_EXT_MISC/"));
    // 长度不足
    EXPECT_TRUE(StringUtils::IsSandboxAncoPath("/storage/Users/currentUser/HO_DATA_EXT_MISC"));
    // 长度超过但后缀不是斜杠
    EXPECT_TRUE(StringUtils::IsSandboxAncoPath("/storage/Users/currentUser/HO_DATA_EXT_MISC/test"));
    // 长度超过且后缀是斜杠，但内容不匹配
    EXPECT_FALSE(StringUtils::IsSandboxAncoPath("/storage/Users/currentUser/other_dir/"));
    // 长度超过且后缀是斜杠，但内容不匹配
    EXPECT_FALSE(StringUtils::IsSandboxAncoPath("/storage/Users/currentUser/HO_DATA_EXT_MIS/"));
    // 长度超过且后缀是斜杠，但内容不匹配
    EXPECT_TRUE(StringUtils::IsSandboxAncoPath("/storage/Users/currentUser/HO_DATA_EXT_MISC/test/"));
    GTEST_LOG_(INFO) << "StringUtilsTest-end IS_SANBOX_ANCO_PATH_TEST_001";
}

/**
* @tc.number: IS_PATH_PREFIX_TEST_001
* @tc.name: IS_PATH_PREFIX_TEST_001
* @tc.desc: Test function of IsPathPrefix
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, IS_PATH_PREFIX_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin IS_PATH_PREFIX_TEST_001";
    // 传入path为空
    EXPECT_FALSE(StringUtils::IsPathPrefix("", "/path/"));
    // 传入prefix为空
    EXPECT_FALSE(StringUtils::IsPathPrefix("/path/", ""));
    // 传入字符串不以/开头
    EXPECT_TRUE(StringUtils::IsPathPrefix("storage/Users/currentUser/", "/storage/Users/currentUser/"));
    // path传入字符串不以/结尾
    EXPECT_TRUE(StringUtils::IsPathPrefix("/storage/Users/currentUser", "/storage/Users/currentUser/"));
    // 传入字符串都不以/结尾
    EXPECT_TRUE(StringUtils::IsPathPrefix("/storage/Users/currentUser", "/storage/Users/currentUser"));
    // 长度小于前缀
    EXPECT_FALSE(StringUtils::IsPathPrefix("/storage/Users/", "/storage/Users/currentUser"));
    // 前缀内容匹配
    EXPECT_TRUE(StringUtils::IsPathPrefix("/storage/Users/currentUser/test/", "/storage/Users/currentUser"));
    GTEST_LOG_(INFO) << "StringUtilsTest-end IS_PATH_PREFIX_TEST_001";
}
 
/**
* @tc.number: CONVERT_MEDIA_SANDBOX_TEST_001
* @tc.name: CONVERT_MEDIA_SANDBOX_TEST_001
* @tc.desc: Test function of ConvertMediaSandboxToPublic
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, CONVERT_MEDIA_SANDBOX_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin CONVERT_MEDIA_SANDBOX_TEST_001";
    // 传入path为空
    EXPECT_EQ(StringUtils::ConvertMediaSandboxToPublic(""), "");
    // 传入path不为媒体沙箱
    EXPECT_EQ(StringUtils::ConvertMediaSandboxToPublic("/storage/Users/currentUser"), "/storage/Users/currentUser");
    // 传入path为媒体沙箱
    EXPECT_EQ(StringUtils::ConvertMediaSandboxToPublic("/storage/media/local/files/Docs/test.txt"),
        "/storage/Users/currentUser/test.txt");
    // 传入path为媒体沙箱
    EXPECT_EQ(StringUtils::ConvertMediaSandboxToPublic("/storage/media/local/files/Docs/test/test.txt"),
        "/storage/Users/currentUser/test/test.txt");
    GTEST_LOG_(INFO) << "StringUtilsTest-end CONVERT_MEDIA_SANDBOX_TEST_001";
}

/**
* @tc.number: FILTER_ANCO_PATHS_TEST_001
* @tc.name: FILTER_ANCO_PATHS_TEST_001
* @tc.desc: Test function of FilterAncoPaths with mixed paths
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, FILTER_ANCO_PATHS_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin FILTER_ANCO_PATHS_TEST_001";

    std::set<std::string> inputPaths = {
        "/storage/Users/currentUser/HO_DATA_EXT_MISC/",
        "/data/app/com.example.app/",
        "/mnt/data/100/HO_MEDIA/",
        "/data/user/0/com.example.app/",
        "/storage/Users/currentUser/HO_DATA/"
    };

    std::set<std::string> expectedAncoPaths = {
        "/storage/Users/currentUser/HO_DATA_EXT_MISC/"
    };

    std::set<std::string> resultAncoPaths = StringUtils::FilterAncoPaths(inputPaths);
    EXPECT_EQ(resultAncoPaths, expectedAncoPaths);

    GTEST_LOG_(INFO) << "StringUtilsTest-end FILTER_ANCO_PATHS_TEST_001";
}

/**
* @tc.number: FILTER_ANCO_PATHS_TEST_002
* @tc.name: FILTER_ANCO_PATHS_TEST_002
* @tc.desc: Test function of FilterAncoPaths with no anco paths
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, FILTER_ANCO_PATHS_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin FILTER_ANCO_PATHS_TEST_002";

    std::set<std::string> inputPaths = {
        "/data/app/com.example.app/",
        "/data/user/0/com.example.app/",
        "/storage/Users/currentUser/HO_DATA/"
    };

    std::set<std::string> expectedAncoPaths;

    std::set<std::string> resultAncoPaths = StringUtils::FilterAncoPaths(inputPaths);

    EXPECT_EQ(resultAncoPaths, expectedAncoPaths);
    GTEST_LOG_(INFO) << "StringUtilsTest-end FILTER_ANCO_PATHS_TEST_002";
}

/**
* @tc.number: FILTER_ANCO_PATHS_TEST_003
* @tc.name: FILTER_ANCO_PATHS_TEST_003
* @tc.desc: Test function of FilterAncoPaths with all anco paths
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, FILTER_ANCO_PATHS_TEST_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin FILTER_ANCO_PATHS_TEST_003";

    std::set<std::string> inputPaths = {
        "/storage/Users/currentUser/HO_DATA_EXT_MISC/",
        "/storage/Users/currentUser/HO_DATA/"
    };

    std::set<std::string> expectedAncoPaths = {
        "/storage/Users/currentUser/HO_DATA_EXT_MISC/"
    };

    std::set<std::string> resultAncoPaths = StringUtils::FilterAncoPaths(inputPaths);

    EXPECT_EQ(resultAncoPaths, expectedAncoPaths);
    GTEST_LOG_(INFO) << "StringUtilsTest-end FILTER_ANCO_PATHS_TEST_003";
}

/**
* @tc.number: FILTER_ANCO_PATHS_TEST_004
* @tc.name: FILTER_ANCO_PATHS_TEST_004
* @tc.desc: Test function of FilterAncoPaths with empty input
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, FILTER_ANCO_PATHS_TEST_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin FILTER_ANCO_PATHS_TEST_004";

    std::set<std::string> inputPaths;
    std::set<std::string> expectedAncoPaths;

    std::set<std::string> resultAncoPaths = StringUtils::FilterAncoPaths(inputPaths);
    EXPECT_EQ(resultAncoPaths, expectedAncoPaths);

    GTEST_LOG_(INFO) << "StringUtilsTest-end FILTER_ANCO_PATHS_TEST_004";
}

/**
* @tc.number: STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_001
* @tc.name: CheckOverLongPath_NormalPath
* @tc.desc: Test normal path length check
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_001";

    std::string normalPath = "/storage/Users/currentUser/test.txt";
    uint32_t result = StringUtils::CheckOverLongPath(normalPath);
    EXPECT_EQ(result, normalPath.length());

    GTEST_LOG_(INFO) << "StringUtilsTest-end STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_001";
}

/**
* @tc.number: STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_002
* @tc.name: CheckOverLongPath_LongPath
* @tc.desc: Test over long path with file name
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_002";

    std::string longPath = "/storage/Users/currentUser/very_long_directory_name_that_exceeds_max_path_length/test.txt";
    uint32_t result = StringUtils::CheckOverLongPath(longPath);
    EXPECT_EQ(result, longPath.length());

    GTEST_LOG_(INFO) << "StringUtilsTest-end STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_002";
}

/**
* @tc.number: STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_003
* @tc.name: CheckOverLongPath_LongPathWithoutFileName
* @tc.desc: Test over long path without file name
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_CHECK_OVER_LONG_PATH__TEST003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_003";

    std::string longPathWithoutFile = "/storage/Users/current/very_long_directory_name_that_exceeds_max_path_length/";
    uint32_t result = StringUtils::CheckOverLongPath(longPathWithoutFile);
    EXPECT_EQ(result, longPathWithoutFile.length());

    GTEST_LOG_(INFO) << "StringUtilsTest-end STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_003";
}

/**
* @tc.number: STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_004
* @tc.name: CheckOverLongPath_ExactMaxPath
* @tc.desc: Test path length exactly at max limit
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_004";

    // 创建一个刚好等于MAX_PATH_LEN的路径
    std::string exactMaxPath(4068, 'a');
    exactMaxPath += "/test.txt";
    uint32_t result = StringUtils::CheckOverLongPath(exactMaxPath);
    EXPECT_EQ(result, exactMaxPath.length());

    GTEST_LOG_(INFO) << "StringUtilsTest-end STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_004";
}

/**
* @tc.number: STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_005
* @tc.name: CheckOverLongPath_ExactMaxPath
* @tc.desc: Test path length exactly at max limit
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_005";

    // 创建一个刚好等于MAX_PATH_LEN的路径
    std::string exactMaxPath(4096, 'a');
    exactMaxPath += "/";
    uint32_t result = StringUtils::CheckOverLongPath(exactMaxPath);
    EXPECT_EQ(result, exactMaxPath.length());

    GTEST_LOG_(INFO) << "StringUtilsTest-end STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_005";
}

/**
* @tc.number: STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_006
* @tc.name: CheckOverLongPath_ExactMaxPath
* @tc.desc: Test path length exactly at max limit
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_006";

    // 创建一个刚好等于MAX_PATH_LEN的路径
    std::string exactMaxPath(4097, 'a');
    uint32_t result = StringUtils::CheckOverLongPath(exactMaxPath);
    EXPECT_EQ(result, exactMaxPath.length());

    GTEST_LOG_(INFO) << "StringUtilsTest-end STRINGUTILS_CHECK_OVER_LONG_PATH_TEST_006";
}

/**
* @tc.number: STRINGUTILS_ADD_TRAILING_SLASH_TEST_001
* @tc.name: AddTrailingSlash_MultipleScenarios
* @tc.desc: Test multiple scenarios for AddTrailingSlash function
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_ADD_TRAILING_SLASH_TEST_001, testing::ext::TestSize.Level1) {
    EXPECT_EQ(StringUtils::AddTrailingSlash(""), BConstants::BACKSLASH);
    EXPECT_EQ(StringUtils::AddTrailingSlash("////"), BConstants::BACKSLASH);
    EXPECT_EQ(StringUtils::AddTrailingSlash("/a/b/c"), "/a/b/c/");
    EXPECT_EQ(StringUtils::AddTrailingSlash("/a/b/c/"), "/a/b/c/");
    EXPECT_EQ(StringUtils::AddTrailingSlash("/a/b/c///"), "/a/b/c/");
}

/**
* @tc.number: STRINGUTILS_REMOVE_TRAILING_SLASH_TEST_001
* @tc.name: RemoveTrailingSlash_MultipleScenarios
* @tc.desc: Test multiple scenarios for RemoveTrailingSlash function
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_REMOVE_TRAILING_SLASH_TEST_001, testing::ext::TestSize.Level1) {
    EXPECT_EQ(StringUtils::RemoveTrailingSlash(""), "");
    EXPECT_EQ(StringUtils::RemoveTrailingSlash("////"), "");
    EXPECT_EQ(StringUtils::RemoveTrailingSlash("/a/b/c"), "/a/b/c");
    EXPECT_EQ(StringUtils::RemoveTrailingSlash("/a/b/c/"), "/a/b/c");
    EXPECT_EQ(StringUtils::RemoveTrailingSlash("/a/b/c///"), "/a/b/c");
}

/**
* @tc.number: STRINGUTILS_IS_SUBDIRECTORY_TEST_001
* @tc.name: IsSubdirectory_MultipleScenarios
* @tc.desc: Test multiple scenarios for IsSubdirectory function
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_IS_SUBDIRECTORY_TEST_001, testing::ext::TestSize.Level1) {
    EXPECT_TRUE(StringUtils::IsSubdirectory("/a/b", "/a/b"));
    EXPECT_TRUE(StringUtils::IsSubdirectory("/a/b", "/a/b/"));
    EXPECT_TRUE(StringUtils::IsSubdirectory("/a/b", "/a/b////"));
    EXPECT_TRUE(StringUtils::IsSubdirectory("/a/b/", "/a/b"));
    EXPECT_TRUE(StringUtils::IsSubdirectory("/a/b/", "/a/b/"));
    EXPECT_TRUE(StringUtils::IsSubdirectory("/a/b", "/a/b/c"));
    EXPECT_FALSE(StringUtils::IsSubdirectory("/a/b/c", "/a/b"));
    EXPECT_TRUE(StringUtils::IsSubdirectory("", "/a/b"));
    EXPECT_FALSE(StringUtils::IsSubdirectory("/a/b", ""));
    EXPECT_TRUE(StringUtils::IsSubdirectory("/a/b/", "/a/b/c//"));
}


/**
* @tc.number: STRINGUTILS_GET_FILE_NAME_TEST_001
* @tc.name: GetFileName_MultipleScenarios
* @tc.desc: Test multiple scenarios for GetFileName function
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_GET_FILE_NAME_TEST_001, testing::ext::TestSize.Level1) {
    // 1. 基础场景：包含路径分隔符的标准路径
    EXPECT_EQ(StringUtils::GetFileName("/a/b/c.txt"), "c.txt");
    EXPECT_EQ(StringUtils::GetFileName("/usr/local/bin"), "bin");

    // 2. 基础场景：包含反斜杠的路径
    EXPECT_EQ(StringUtils::GetFileName("C:\\Program Files\\app.exe"), "app.exe");
    EXPECT_EQ(StringUtils::GetFileName("D:\\Data\\file"), "file");

    // 3. 混合场景：路径末尾包含分隔符
    // 注意：根据实现逻辑，find_last_of 找到最后一个分隔符，substr 取其后内容。
    // 如果分隔符是最后一个字符，substr 会返回空字符串。
    EXPECT_EQ(StringUtils::GetFileName("/a/b/c/"), "");
    EXPECT_EQ(StringUtils::GetFileName("C:\\a\\b\\"), "");

    // 4. 边缘场景：只有文件名，不包含路径分隔符
    EXPECT_EQ(StringUtils::GetFileName("filename.txt"), "filename.txt");
    EXPECT_EQ(StringUtils::GetFileName("config"), "config");

    // 5. 边缘场景：空字符串
    EXPECT_EQ(StringUtils::GetFileName(""), "");

    // 6. 边缘场景：只有分隔符
    EXPECT_EQ(StringUtils::GetFileName("/"), "");
    EXPECT_EQ(StringUtils::GetFileName("\\"), "");
    EXPECT_EQ(StringUtils::GetFileName("////"), "");

    // 7. 边缘场景：文件名中包含点（测试是否只取最后一段）
    EXPECT_EQ(StringUtils::GetFileName("/a/b/archive.tar.gz"), "archive.tar.gz");
}

/**
* @tc.number: STRINGUTILS_IS_PATH_WITH_DIRECTORY_TEST_001
* @tc.name: IsPathWithDirectory_MultipleScenarios
* @tc.desc: Test multiple scenarios for IsPathWithDirectory function
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_IS_PATH_WITH_DIRECTORY_TEST_001, testing::ext::TestSize.Level1) {
    // 1. 基础场景：包含路径分隔符的路径
    EXPECT_TRUE(StringUtils::IsPathWithDirectory("/a/b/c.txt"));
    EXPECT_TRUE(StringUtils::IsPathWithDirectory("/usr/local/bin"));
    EXPECT_TRUE(StringUtils::IsPathWithDirectory("C:\\Program Files\\app.exe"));
    EXPECT_TRUE(StringUtils::IsPathWithDirectory("D:\\Data\\file"));

    // 2. 基础场景：相对路径
    EXPECT_TRUE(StringUtils::IsPathWithDirectory("./config.ini"));
    EXPECT_TRUE(StringUtils::IsPathWithDirectory("../test/file"));

    // 3. 边缘场景：单纯文件名，不包含路径分隔符
    EXPECT_FALSE(StringUtils::IsPathWithDirectory("filename.txt"));
    EXPECT_FALSE(StringUtils::IsPathWithDirectory("config"));
    EXPECT_FALSE(StringUtils::IsPathWithDirectory("Makefile"));

    // 4. 边缘场景：空字符串
    EXPECT_FALSE(StringUtils::IsPathWithDirectory(""));

    // 5. 边缘场景：只有分隔符（根据实现逻辑，这会被视为包含路径）
    EXPECT_TRUE(StringUtils::IsPathWithDirectory("/"));
    EXPECT_TRUE(StringUtils::IsPathWithDirectory("\\"));
    EXPECT_TRUE(StringUtils::IsPathWithDirectory("////"));
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_001
* @tc.name: StringVectorSerialize_Deserialize_Normal
* @tc.desc: Test normal serialize and deserialize roundtrip
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_001";
    std::vector<std::string> input = {"hello", "world", "/storage/Users/currentUser"};
    std::string serialized = StringUtils::StringVectorSerialize(input);
    EXPECT_FALSE(serialized.empty());
    std::vector<std::string> output = StringUtils::StringVectorDeserialize(serialized);
    EXPECT_EQ(output.size(), input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        EXPECT_EQ(output[i], input[i]);
    }
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_001";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_002
* @tc.name: StringVectorSerialize_Deserialize_EmptyVector
* @tc.desc: Test serialize and deserialize with empty vector
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_002";
    std::vector<std::string> input;
    std::string serialized = StringUtils::StringVectorSerialize(input);
    EXPECT_FALSE(serialized.empty());
    EXPECT_EQ(serialized.size(), sizeof(uint64_t));
    std::vector<std::string> output = StringUtils::StringVectorDeserialize(serialized);
    EXPECT_TRUE(output.empty());
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_002";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_003
* @tc.name: StringVectorSerialize_Deserialize_SingleEmptyString
* @tc.desc: Test serialize and deserialize with vector containing empty string
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_003";
    std::vector<std::string> input = {""};
    std::string serialized = StringUtils::StringVectorSerialize(input);
    EXPECT_FALSE(serialized.empty());
    std::vector<std::string> output = StringUtils::StringVectorDeserialize(serialized);
    EXPECT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], "");
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_003";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_004
* @tc.name: StringVectorSerialize_Deserialize_SpecialChars
* @tc.desc: Test serialize and deserialize with strings containing special characters
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_004";
    std::vector<std::string> input = {"hello\nworld", "tab\there", "with\"quote", "back\\slash"};
    std::string serialized = StringUtils::StringVectorSerialize(input);
    EXPECT_FALSE(serialized.empty());
    std::vector<std::string> output = StringUtils::StringVectorDeserialize(serialized);
    EXPECT_EQ(output.size(), input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        EXPECT_EQ(output[i], input[i]);
    }
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_004";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_005
* @tc.name: StringVectorDeserialize_InvalidData
* @tc.desc: Test deserialize with invalid data (too short, truncated)
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_DESERIALIZE_TEST_005";
    EXPECT_TRUE(StringUtils::StringVectorDeserialize("").empty());
    EXPECT_TRUE(StringUtils::StringVectorDeserialize("abc").empty());
    std::string sevenBytes(7, 'X');
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(sevenBytes).empty());
    std::string truncated;
    uint64_t count = 1;
    truncated.append(reinterpret_cast<const char*>(&count), sizeof(uint64_t));
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(truncated).empty());
    truncated.clear();
    truncated.append(reinterpret_cast<const char*>(&count), sizeof(uint64_t));
    uint64_t len = 5;
    truncated.append(reinterpret_cast<const char*>(&len), sizeof(uint64_t));
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(truncated).empty());
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_DESERIALIZE_TEST_005";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_006
* @tc.name: StringVectorDeserialize_TrailingGarbageData
* @tc.desc: Test deserialize rejects data with trailing garbage after valid content
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_DESERIALIZE_TEST_006";
    std::vector<std::string> input = {"hello"};
    std::string serialized = StringUtils::StringVectorSerialize(input);
    // append trailing garbage data
    serialized.append("GARBAGE");
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(serialized).empty());
    // append single trailing byte
    serialized = StringUtils::StringVectorSerialize(input);
    serialized.append(1, '\0');
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(serialized).empty());
    // count=0 with trailing garbage
    std::string zeroCount;
    uint64_t zero = 0;
    zeroCount.append(reinterpret_cast<const char*>(&zero), sizeof(uint64_t));
    zeroCount.append("trash");
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(zeroCount).empty());
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_DESERIALIZE_TEST_006";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_007
* @tc.name: StringVectorDeserialize_MaliciousLargeCount
* @tc.desc: Test deserialize rejects data with extremely large count value
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_DESERIALIZE_TEST_007";
    std::string malicious;
    uint64_t hugeCount = UINT64_MAX;
    malicious.append(reinterpret_cast<const char*>(&hugeCount), sizeof(uint64_t));
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(malicious).empty());
    malicious.append(16, 'X');
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(malicious).empty());
    uint64_t boundaryCount = 2;
    uint64_t bigLen = 100;
    std::string boundaryData;
    boundaryData.append(reinterpret_cast<const char*>(&boundaryCount), sizeof(uint64_t));
    boundaryData.append(reinterpret_cast<const char*>(&bigLen), sizeof(uint64_t));
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(boundaryData).empty());
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_DESERIALIZE_TEST_007";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_008
* @tc.name: StringVectorDeserialize_MaliciousLargeLen
* @tc.desc: Test deserialize rejects data with extremely large string length
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_DESERIALIZE_TEST_008";
    std::string malicious;
    uint64_t count = 1;
    uint64_t hugeLen = UINT64_MAX;
    malicious.append(reinterpret_cast<const char*>(&count), sizeof(uint64_t));
    malicious.append(reinterpret_cast<const char*>(&hugeLen), sizeof(uint64_t));
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(malicious).empty());
    malicious.append(4, 'X');
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(malicious).empty());
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_DESERIALIZE_TEST_008";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_013
* @tc.name: StringVectorSerialize_ExceedsMaxSize
* @tc.desc: Test serialize rejects data when total size exceeds 16MB limit and accepts at boundary
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_013, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_013";
    std::string bigStr(16 * 1024 * 1024, 'A');
    EXPECT_TRUE(StringUtils::StringVectorSerialize({bigStr}).empty());
    EXPECT_TRUE(StringUtils::StringVectorSerialize({"hello", bigStr}).empty());
    std::string nearLimit(16 * 1024 * 1024 - 2 * sizeof(uint64_t), 'A');
    std::string serialized = StringUtils::StringVectorSerialize({nearLimit});
    EXPECT_FALSE(serialized.empty());
    EXPECT_EQ(serialized.size(), static_cast<size_t>(16 * 1024 * 1024));
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_013";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_014
* @tc.name: StringVectorDeserialize_ExceedsMaxSize
* @tc.desc: Test deserialize rejects data exceeding 16MB limit and accepts at boundary
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_014, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_DESERIALIZE_TEST_014";
    std::string hugeData(16 * 1024 * 1024 + 1, 'X');
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(hugeData).empty());
    std::string nearLimit(16 * 1024 * 1024 - 2 * sizeof(uint64_t), 'A');
    std::string serialized = StringUtils::StringVectorSerialize({nearLimit});
    EXPECT_EQ(serialized.size(), static_cast<size_t>(16 * 1024 * 1024));
    std::vector<std::string> output = StringUtils::StringVectorDeserialize(serialized);
    EXPECT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], nearLimit);
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_DESERIALIZE_TEST_014";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_009
* @tc.name: StringVectorDeserialize_PartialValidThenTruncated
* @tc.desc: Test deserialize rejects data where first string valid but second truncated
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_DESERIALIZE_TEST_009";
    std::vector<std::string> input = {"first", "second"};
    std::string serialized = StringUtils::StringVectorSerialize(input);
    // truncate: remove last few bytes so second string is incomplete
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(serialized.substr(0, serialized.size() - 3)).empty());
    // truncate: remove len field of second string
    size_t firstStrEnd = sizeof(uint64_t) + sizeof(uint64_t) + 5;
    EXPECT_TRUE(StringUtils::StringVectorDeserialize(serialized.substr(0, firstStrEnd)).empty());
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_DESERIALIZE_TEST_009";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_010
* @tc.name: StringVectorSerialize_Deserialize_EmptyStringsMixed
* @tc.desc: Test serialize and deserialize with mixed empty and non-empty strings
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_010";
    std::vector<std::string> input = {"", "hello", "", "world", ""};
    std::string serialized = StringUtils::StringVectorSerialize(input);
    EXPECT_FALSE(serialized.empty());
    std::vector<std::string> output = StringUtils::StringVectorDeserialize(serialized);
    EXPECT_EQ(output.size(), input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        EXPECT_EQ(output[i], input[i]);
    }
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_010";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_011
* @tc.name: StringVectorSerialize_Deserialize_BinaryContentWithNull
* @tc.desc: Test serialize and deserialize with strings containing embedded null bytes
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_011, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_011";
    std::string withNull = "hello\0world";
    withNull.resize(11);
    std::vector<std::string> input = {withNull, "normal"};
    std::string serialized = StringUtils::StringVectorSerialize(input);
    EXPECT_FALSE(serialized.empty());
    std::vector<std::string> output = StringUtils::StringVectorDeserialize(serialized);
    EXPECT_EQ(output.size(), input.size());
    EXPECT_EQ(output[0].size(), 11);
    EXPECT_EQ(output[0], withNull);
    EXPECT_EQ(output[1], "normal");
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_SERIALIZE_DESERIALIZE_TEST_011";
}

/**
* @tc.number: STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_012
* @tc.name: StringVectorDeserialize_CountZeroExactSize
* @tc.desc: Test deserialize with count=0 and exact data size (no trailing data)
* @tc.size: SMALL
* @tc.type: FUNC
* @tc.level: Level 1
* @tc.require: NA
*/
HWTEST_F(StringUtilsTest, STRINGUTILS_STRING_VECTOR_DESERIALIZE_TEST_012, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StringUtilsTest-begin STRING_VECTOR_DESERIALIZE_TEST_012";
    std::string zeroData;
    uint64_t zero = 0;
    zeroData.append(reinterpret_cast<const char*>(&zero), sizeof(uint64_t));
    std::vector<std::string> output = StringUtils::StringVectorDeserialize(zeroData);
    EXPECT_TRUE(output.empty());
    GTEST_LOG_(INFO) << "StringUtilsTest-end STRING_VECTOR_DESERIALIZE_TEST_012";
}
} // namespace OHOS::FileManagement::Backup