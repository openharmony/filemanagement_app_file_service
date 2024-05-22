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
#include "oh_file_uri.h"

#include <cstring>

#include "file_uri.h"
#include "log.h"
#include "securec.h"

using namespace std;
static FileManagement_ErrCode GetValue(std::string_view resultStr, char **result)
{
    size_t count = resultStr.length();
    if (count == 0) {
        return ERR_UNKNOWN;
    }
    *result = static_cast<char *>(malloc(sizeof(char) * (count + 1)));
    if (*result == nullptr) {
        LOGE("malloc is feiled!");
        return ERR_ENOMEM;
    }
    int ret = strncpy_s(*result, count + 1, resultStr.data(), count);
    if (ret != 0) {
        LOGE("strncpy_s is feiled!");
        free(*result);
        *result = nullptr;
        return ERR_ENOMEM;
    }
    return ERR_OK;
}

FileManagement_ErrCode OH_FileUri_GetUriFromPath(const char *path, unsigned int length, char **result)
{
    if (path == nullptr || strlen(path) != length || result == nullptr) {
        return ERR_PARAMS;
    }
    std::string pathStr(path, length);
    OHOS::AppFileService::ModuleFileUri::FileUri fileUri(pathStr);
    return GetValue(fileUri.ToString(), result);
}

FileManagement_ErrCode OH_FileUri_GetPathFromUri(const char *uri, unsigned int length, char **result)
{
    if (uri == nullptr || strlen(uri) != length || result == nullptr) {
        return ERR_PARAMS;
    }
    std::string uriStr(uri, length);
    OHOS::AppFileService::ModuleFileUri::FileUri fileUri(uriStr);
    return GetValue(fileUri.GetRealPath(), result);
}

FileManagement_ErrCode OH_FileUri_GetFullDirectoryUri(const char *uri, unsigned int length, char **result)
{
    if (uri == nullptr || strlen(uri) != length || result == nullptr) {
        return ERR_PARAMS;
    }
    std::string uriStr(uri, length);
    OHOS::AppFileService::ModuleFileUri::FileUri fileUri(uriStr);
    std::string resultStr = fileUri.GetFullDirectoryUri();
    if (resultStr.length() == 0) {
        return ERR_ENOENT;
    }
    return GetValue(resultStr, result);
}

bool OH_FileUri_IsValidUri(const char *uri, unsigned int length)
{
    if (uri == nullptr || strlen(uri) != length) {
        return ERR_PARAMS;
    }
    std::string uriStr(uri, length);
    OHOS::AppFileService::ModuleFileUri::FileUri fileUri(uriStr);
    return fileUri.CheckUriFormat(uriStr);
}