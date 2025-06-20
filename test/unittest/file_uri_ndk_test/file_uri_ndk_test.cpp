/*A
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
#include "common_func.h"
#include "error_code.h"
#include "oh_file_uri.h"
#include "parameter.h"

#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <string>

using namespace testing::ext;
using namespace OHOS::AppFileService;
namespace {
const std::string BUNDLE_A = "com.example.filesharea";
const std::string FILE_MANAGER_FULL_MOUNT_ENABLE_PARAMETER_STR = "const.filemanager.full_mount.enable";
} // namespace

std::string CommonFunc::GetSelfBundleName()
{
    return BUNDLE_A;
}

namespace OHOS::AppFileService::ModuleFileUri {

static void FreeResult(char **freeResult)
{
    if ((*freeResult) != nullptr) {
        free(*freeResult);
        *freeResult = nullptr;
    }
}

static bool CheckFileManagerFullMountEnable()
{
    char value[] = "false";
    const char *fileManagerFullMountEnableParameter = FILE_MANAGER_FULL_MOUNT_ENABLE_PARAMETER_STR.c_str();
    int retSystem = GetParameter(fileManagerFullMountEnableParameter, "false", value, sizeof(value));
    if (retSystem > 0 && !std::strcmp(value, "true")) {
        return true;
    }
    GTEST_LOG_(INFO) << "Not supporting all mounts";
    return false;
}

class NDKFileUriTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: get_path_from_uri_test_001
 * @tc.name: Test function of OH_FileUri_GetPathFromUri() interface for sandbox uri
 * @tc.desc: Set uri and get path
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_path_from_uri_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_path_from_uri_test_001 start";
    std::string fileUriStr = "file://" + BUNDLE_A + "/data/storage/el2/base/files/GetPathFromUri001.txt";
    const char *fileUri = fileUriStr.c_str();
    const char filePath[] = "/data/storage/el2/base/files/GetPathFromUri001.txt";
    char *result = nullptr;
    unsigned int length = fileUriStr.size();
    FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, filePath), 0);
        FreeResult(&result);
    }
    GTEST_LOG_(INFO) << "get_path_from_uri_test_001 end";
}

/**
 * @tc.number: get_path_from_uri_test_002
 * @tc.name: Test function of OH_FileUri_GetPathFromUri() interface for document uri
 * @tc.desc: Set uri and get path
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_path_from_uri_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_path_from_uri_test_002 start";
    const char fileUri[] = "file://docs/storage/Users/currentUser/Documents/GetPathFromUri002.txt";
    char *result = nullptr;
    unsigned int length = strlen(fileUri);
    FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    GTEST_LOG_(INFO) << result;
    if (!CheckFileManagerFullMountEnable()) {
        const char filePath[] =
            "/data/storage/el2/share/r/docs/storage/Users/currentUser/Documents/GetPathFromUri002.txt";
        EXPECT_EQ(strcmp(result, filePath), 0);
    } else {
        const char filePath[] = "/storage/Users/currentUser/Documents/GetPathFromUri002.txt";
        EXPECT_EQ(strcmp(result, filePath), 0);
    }
    FreeResult(&result);
    GTEST_LOG_(INFO) << "get_path_from_uri_test_002 end";
}

/**
 * @tc.number: get_path_from_uri_test_003
 * @tc.name: Test function of OH_FileUri_GetPathFromUri() interface for different applications uri
 * @tc.desc: Set uri and get path
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_path_from_uri_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_path_from_uri_test_003 start";
    const char fileUri[] = "file://demoa/data/storage/el2/base/files/GetPathFromUri003.txt";
    std::string filePathStr = "/data/storage/el2/share/r/demoa/data/storage/el2/base/files/GetPathFromUri003.txt";
    const char *filePath = filePathStr.c_str();
    char *result = nullptr;
    unsigned int length = strlen(fileUri);
    FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, filePath), 0);
        FreeResult(&result);
    }
    GTEST_LOG_(INFO) << "get_path_from_uri_test_003 end";
}

/**
 * @tc.number: get_path_from_uri_test_004
 * @tc.name: Test function of OH_FileUri_GetPathFromUri() interface for distributed files uri
 * @tc.desc: Set uri and get path
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_path_from_uri_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_path_from_uri_test_004 start";
    std::string fileUriStr = "file://" + BUNDLE_A + "/data/storage/el2/distributedfiles/.remote_share/";
    fileUriStr += "data/storage/el2/base/haps/entry/files/GetPathFromUri004.txt";
    fileUriStr += "?networkid=64799ecdf70788e396f454ff4a6e6ae4b09e20227c39c21f6e67a2aacbcef7b9";
    const char *fileUri = fileUriStr.c_str();
    std::string filePathStr =
        "/data/storage/el2/share/r/64799ecdf70788e396f454ff4a6e6ae4b09e20227c39c21f6e67a2aacbcef7b9/" + BUNDLE_A +
        "/data/storage/el2/distributedfiles/.remote_share/";
    filePathStr += "data/storage/el2/base/haps/entry/files/GetPathFromUri004.txt";
    const char *filePath = filePathStr.c_str();
    char *result = nullptr;
    unsigned int length = fileUriStr.size();
    FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, filePath), 0);
        FreeResult(&result);
    }
    GTEST_LOG_(INFO) << "get_path_from_uri_test_004 end";
}

/**
 * @tc.number: get_path_from_uri_test_005
 * @tc.name: Test function of OH_FileUri_GetPathFromUri() interface for distributed files uri
 * @tc.desc: Set uri and get path
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_path_from_uri_test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_path_from_uri_test_005 start";
    std::string fileUriStr = "file://docs/storage/Users/currentUser/Documents/GetPathFromUri005.txt";
    fileUriStr += "?networkid=64799ecdf70788e396f454ff4a6e6ae4b09e20227c39c21f6e67a2aacbcef7b9";
    const char *fileUri = fileUriStr.c_str();
    const char filePath[] =
        "/data/storage/el2/share/r/64799ecdf70788e396f454ff4a6e6ae4b09e20227c39c21f6e67a2aacbcef7b9/docs/storage/Users/"
        "currentUser/Documents/GetPathFromUri005.txt";
    char *result = nullptr;
    unsigned int length = fileUriStr.size();
    FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, filePath), 0);
        FreeResult(&result);
    }
    GTEST_LOG_(INFO) << "get_path_from_uri_test_005 end";
}

/**
 * @tc.number: get_path_from_uri_test_006
 * @tc.name: Test function of OH_FileUri_GetPathFromUri() interface for distributed files uri
 * @tc.desc: Set uri and get path
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_path_from_uri_test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_path_from_uri_test_006 start";
    std::string bundleB = "com.example.fileshareb";
    std::string fileUriStr = "file://" + bundleB + "/data/storage/el2/distributedfiles/.remote_share/";
    fileUriStr += "data/storage/el2/base/haps/entry/files/GetPathFromUri006.txt";
    fileUriStr += "?networkid=64799ecdf70788e396f454ff4a6e6ae4b09e20227c39c21f6e67a2aacbcef7b9";
    const char *fileUri = fileUriStr.c_str();
    std::string filePathUri =
        "/data/storage/el2/share/r/64799ecdf70788e396f454ff4a6e6ae4b09e20227c39c21f6e67a2aacbcef7b9/" + bundleB;
    filePathUri += "/data/storage/el2/distributedfiles/.remote_share/";
    filePathUri += "data/storage/el2/base/haps/entry/files/GetPathFromUri006.txt";
    const char *filePath = filePathUri.c_str();
    char *result = nullptr;
    unsigned int length = fileUriStr.size();
    FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, filePath), 0);
        FreeResult(&result);
    }
    GTEST_LOG_(INFO) << "get_path_from_uri_test_006 end";
}

/**
 * @tc.number: get_path_from_uri_test_007
 * @tc.name: Test function of OH_FileUri_GetPathFromUri() interface for distributed files uri
 * @tc.desc: Set uri and get path
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_path_from_uri_test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_path_from_uri_test_007 start";
    std::string bundleB = "com.example.fileshareb";
    std::string fileUriStr = "file://" + bundleB + "/data/storage/el2/distributedfiles/.remote_share/";
    fileUriStr += "data/storage/el2/base/haps/entry/files/GetPathFromUri006.txt";
    fileUriStr += "?networkid=64799ecdf70788e396f454ff4a6e6ae4b09e20227c39c21f6e67a2aacbcef7b9";
    const char *fileUri = fileUriStr.c_str();
    char *result = nullptr;
    unsigned int length = fileUriStr.size();
    FileManagement_ErrCode ret = OH_FileUri_GetPathFromUri(nullptr, length, &result);
    EXPECT_EQ(ret, ERR_PARAMS);

    ret = OH_FileUri_GetPathFromUri(fileUri, length - 1, &result);
    EXPECT_EQ(ret, ERR_PARAMS);

    ret = OH_FileUri_GetPathFromUri(fileUri, length, nullptr);
    EXPECT_EQ(ret, ERR_PARAMS);
    GTEST_LOG_(INFO) << "get_path_from_uri_test_007 end";
}

/**
 * @tc.number: get_uri_from_path_test_001
 * @tc.name: Test function of OH_FileUri_GetUriFromPath() interface for document uri
 * @tc.desc: Set path and get uri
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_uri_from_path_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_uri_from_path_test_001 start";
    const char filePath[] = "storage/Users/currentUser/Documents/GetPathFromUri001.txt";
    const char fileUri[] = "file://docs/storage/Users/currentUser/Documents/GetPathFromUri001.txt";
    char *result = nullptr;
    unsigned int length = strlen(filePath);
    FileManagement_ErrCode ret = OH_FileUri_GetUriFromPath(filePath, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, fileUri), 0);
        FreeResult(&result);
    }
    GTEST_LOG_(INFO) << "get_uri_from_path_test_001 end";
}

/**
 * @tc.number: get_uri_from_path_test_002
 * @tc.name: Test function of OH_FileUri_GetUriFromPath() interface for document uri
 * @tc.desc: Set path and get uri
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_uri_from_path_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_uri_from_path_test_002 start";
    const char filePath[] = "storage/Users/currentUser/Documents/GetPathFromUri001.txt";
    char *result = nullptr;
    unsigned int length = strlen(filePath);
    FileManagement_ErrCode ret = OH_FileUri_GetUriFromPath(nullptr, length, &result);
    EXPECT_EQ(ret, ERR_PARAMS);

    ret = OH_FileUri_GetUriFromPath(filePath, length - 1, &result);
    EXPECT_EQ(ret, ERR_PARAMS);

    ret = OH_FileUri_GetUriFromPath(filePath, length, nullptr);
    EXPECT_EQ(ret, ERR_PARAMS);
    GTEST_LOG_(INFO) << "get_uri_from_path_test_002 end";
}

/**
 * @tc.number: get_uri_from_path_test_003
 * @tc.name: Test function of OH_FileUri_GetUriFromPath() interface for document uri
 * @tc.desc: Set path and get uri
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_uri_from_path_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_uri_from_path_test_003 start";
    const char filePath[] = "data/storage/el2/media/Photo/12/IMG_12345_999999/test.jpg";
    const char fileUri[] = "file://media/Photo/12/IMG_12345_999999/test.jpg";
    char *result = nullptr;
    unsigned int length = strlen(filePath);
    FileManagement_ErrCode ret = OH_FileUri_GetUriFromPath(filePath, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, fileUri), 0);
        FreeResult(&result);
    }
    GTEST_LOG_(INFO) << "get_uri_from_path_test_003 end";
}

/**
 * @tc.number: get_uri_from_path_test_004
 * @tc.name: Test function of OH_FileUri_GetUriFromPath() interface for document uri
 * @tc.desc: Set path and get uri
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_uri_from_path_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_uri_from_path_test_004 start";
    const char filePath[] = "data/storage/el2/base/Photo/12/IMG_12345_999999/test.jpg";
    const char fileUri[] = "file://com.example.filesharea/data/storage/el2/base/Photo/12/IMG_12345_999999/test.jpg";
    char *result = nullptr;
    unsigned int length = strlen(filePath);
    FileManagement_ErrCode ret = OH_FileUri_GetUriFromPath(filePath, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, fileUri), 0);
        FreeResult(&result);
    }
    GTEST_LOG_(INFO) << "get_uri_from_path_test_003 end";
}

/**
 * @tc.number: get_uri_from_path_test_005
 * @tc.name: Test function of OH_FileUri_GetUriFromPath() interface for document uri
 * @tc.desc: Set path and get uri
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_uri_from_path_test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_uri_from_path_test_005 start";
    const char filePath[] = "data/storage/el1/media/Photo/12/IMG_12345_999999/test.jpg";
    const char fileUri[] = "file://com.example.filesharea/data/storage/el1/media/Photo/12/IMG_12345_999999/test.jpg";
    char *result = nullptr;
    unsigned int length = strlen(filePath);
    FileManagement_ErrCode ret = OH_FileUri_GetUriFromPath(filePath, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, fileUri), 0);
        FreeResult(&result);
    }
    GTEST_LOG_(INFO) << "get_uri_from_path_test_005 end";
}

/**
 * @tc.number: get_full_directory_uri_test_001
 * @tc.name: Test function of OH_FileUri_GetFullDirectoryUri() interface for unknown path
 * @tc.desc: Set uri and get full directory uri
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_full_directory_uri_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_full_directory_uri_test_001 start";
    const char fileUri[] = "file://docs/storage/Users/currentUser/Documents/GetFullDirectoryUri001.txt";
    char *result = nullptr;
    unsigned int length = strlen(fileUri);
    FileManagement_ErrCode ret = OH_FileUri_GetFullDirectoryUri(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_ENOENT);
    FreeResult(&result);
    GTEST_LOG_(INFO) << "get_full_directory_uri_test_001 end";
}

/**
 * @tc.number: get_full_directory_uri_test_002
 * @tc.name: Test function of OH_FileUri_GetFullDirectoryUri() interface for success
 * @tc.desc: Set uri and get full directory uri
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_full_directory_uri_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_full_directory_uri_test_002 start";
    const char fileUri[] = "file://com.example.filesharea/data/test/file_uri_test.txt";
    const char resultUri[] = "file://com.example.filesharea/data/test";
    char *result = nullptr;
    unsigned int length = strlen(fileUri);
    FileManagement_ErrCode ret = OH_FileUri_GetFullDirectoryUri(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, resultUri), 0);
    }
    FreeResult(&result);
    GTEST_LOG_(INFO) << "get_full_directory_uri_test_002 end";
}

/**
 * @tc.number: get_full_directory_uri_test_003
 * @tc.name: Test function of OH_FileUri_GetFullDirectoryUri() interface for success
 * @tc.desc: Set uri and get full directory uri
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_full_directory_uri_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_full_directory_uri_test_003 start";
    const char fileUri[] = "file://com.example.filesharea/data/test/file_uri_test.txt";
    char *result = nullptr;
    unsigned int length = strlen(fileUri);
    FileManagement_ErrCode ret = OH_FileUri_GetFullDirectoryUri(nullptr, length, &result);
    EXPECT_EQ(ret, ERR_PARAMS);

    ret = OH_FileUri_GetFullDirectoryUri(fileUri, length - 1, &result);
    EXPECT_EQ(ret, ERR_PARAMS);

    ret = OH_FileUri_GetFullDirectoryUri(fileUri, length, nullptr);
    EXPECT_EQ(ret, ERR_PARAMS);
    FreeResult(&result);
    GTEST_LOG_(INFO) << "get_full_directory_uri_test_003 end";
}

/**
 * @tc.number: is_valid_uri_test_001
 * @tc.name: Test function of OH_FileUri_IsValidUri() interface for real uri
 * @tc.desc: Set URI and make a judgment
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, is_valid_uri_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "is_valid_uri_test_001";
    const char fileUri[] = "file://com.demo.a/data/storage/el2/base/files/IsValidUriTest001.txt";
    unsigned int length = strlen(fileUri);
    bool flags = OH_FileUri_IsValidUri(fileUri, length);
    EXPECT_EQ(flags, true);
    GTEST_LOG_(INFO) << "is_valid_uri_test_001";
}

/**
 * @tc.number: is_valid_uri_test_002
 * @tc.name: Test function of OH_FileUri_IsValidUri() interface for unreal uri
 * @tc.desc: Set URI and make a judgment
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, is_valid_uri_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "is_valid_uri_test_002";
    const char fileUri[] = "com.demo.a/data/storage/el2/base/files/IsValidUriTest002.txt";
    unsigned int length = strlen(fileUri);
    bool flags = OH_FileUri_IsValidUri(fileUri, length);
    EXPECT_EQ(flags, false);
    GTEST_LOG_(INFO) << "is_valid_uri_test_002";
}

/**
 * @tc.number: is_valid_uri_test_003
 * @tc.name: Test function of OH_FileUri_IsValidUri() interface for unreal uri
 * @tc.desc: Set URI and make a judgment
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, is_valid_uri_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "is_valid_uri_test_003";
    const char fileUri[] = "com.demo.a/data/storage/el2/base/files/IsValidUriTest002.txt";
    unsigned int length = strlen(fileUri);
    bool flags = OH_FileUri_IsValidUri(nullptr, length);
    EXPECT_EQ(flags, false);

    flags = OH_FileUri_IsValidUri(fileUri, length - 1);
    EXPECT_EQ(flags, false);
    GTEST_LOG_(INFO) << "is_valid_uri_test_003";
}

/**
 * @tc.number: get_filename_test_001
 * @tc.name: Test function of OH_FileUri_GetFileName() interface for unknown path
 * @tc.desc: Set uri and get filename
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_fileUri_filename_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_fileUri_filename_test_001 start";
    const char fileUri[] = "file://docs/storage/Users/currentUser/Documents/GetFullDirectoryUri001.txt";
    const char resultUri[] = "GetFullDirectoryUri001.txt";
    char *result = nullptr;
    unsigned int length = strlen(fileUri);
    FileManagement_ErrCode ret = OH_FileUri_GetFileName(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, resultUri), 0);
    }
    FreeResult(&result);
    GTEST_LOG_(INFO) << "get_fileUri_filename_test_001 end";
}

/**
 * @tc.number: get_filename_test_002
 * @tc.name: Test function of OH_FileUri_GetFileName() interface for unknown path
 * @tc.desc: Set uri and get filename
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_fileUri_filename_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_fileUri_filename_test_002 start";
    const char fileUri[] = "file://docs/storage/Users/currentUser/Documents/dirName";
    const char resultUri[] = "dirName";
    char *result = nullptr;
    unsigned int length = strlen(fileUri);
    FileManagement_ErrCode ret = OH_FileUri_GetFileName(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_OK);
    if (result != nullptr) {
        GTEST_LOG_(INFO) << result;
        EXPECT_EQ(strcmp(result, resultUri), 0);
    }
    FreeResult(&result);
    GTEST_LOG_(INFO) << "get_fileUri_filename_test_002 end";
}

/**
 * @tc.number: get_filename_test_003
 * @tc.name: Test function of OH_FileUri_GetFileName() interface for unknown path
 * @tc.desc: Set uri and get filename
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_fileUri_filename_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_fileUri_filename_test_003 start";
    const char fileUri[] = "file://com.example.filesharea/data/test/file_uri_test.txt";
    char *result = nullptr;
    unsigned int length = strlen(fileUri);
    FileManagement_ErrCode ret = OH_FileUri_GetFileName(nullptr, length, &result);
    EXPECT_EQ(ret, ERR_PARAMS);

    ret = OH_FileUri_GetFileName(fileUri, length - 1, &result);
    EXPECT_EQ(ret, ERR_PARAMS);

    ret = OH_FileUri_GetFileName(fileUri, length, nullptr);
    EXPECT_EQ(ret, ERR_PARAMS);
    FreeResult(&result);
    GTEST_LOG_(INFO) << "get_fileUri_filename_test_003 end";
}

/**
 * @tc.number: get_filename_test_004
 * @tc.name: Test function of OH_FileUri_GetFileName() interface for unknown path
 * @tc.desc: Set uri and get filename
 * @tc.type: FUNC
 */
HWTEST_F(NDKFileUriTest, get_fileUri_filename_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "get_fileUri_filename_test_004 start";
    const char fileUri[] = "file://docs/storage/Users/currentUser/Documents/dirName/";
    char *result = nullptr;
    unsigned int length = strlen(fileUri);
    FileManagement_ErrCode ret = OH_FileUri_GetFileName(fileUri, length, &result);
    EXPECT_EQ(ret, ERR_PARAMS);
    GTEST_LOG_(INFO) << "get_fileUri_filename_test_004 end";
}

} // namespace OHOS::AppFileService::ModuleFileUri