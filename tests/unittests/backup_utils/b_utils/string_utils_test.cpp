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
} // namespace OHOS::FileManagement::Backup