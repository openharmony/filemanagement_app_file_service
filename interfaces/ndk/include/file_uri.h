 /*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FILE_URI_H
#define FILE_URI_H

/**
 * @file file_uri.h
 *
 * @brief Handle operations related to URIs.
 * @library libnative_fileuri_ndk.z.so
 * @syscap SystemCapability.FileManagement.AppFileService
 * @since 12
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get Uri From Path.
 *
 * @param asset Pass in the URI to get the path.
 * @param asset The result after processing.
 * @return Returns the execution result.
 * @see fileuri
 * @since 12
 */
int OH_FileUri_GetUriFromPath(const char *path, char *result);

/**
 * @brief Get path From uri.
 *
 * @param asset Pass in the path to get the uri.
 * @param asset The result after processing.
 * @return Returns the execution result.
 * @return Returns .
 * @see fileuri
 * @since 12
 */
int OH_FileUri_GetPathFromUri(const char *uri, char *result);

/**
 * @brief Obtain the path where the URI is located.
 * 
 * @param asset Pass in the URI to get the path.
 * @param asset The result after processing.
 * @return Returns the execution result.
 * @return Returns a uri.
 * @see fileuri
 * @since 12
 */
int OH_FileUri_GetFullDirectoryUri(const char *uri, char *result);

/**
 * @brief Check the correctness of the Uri.
 * 
 * @param asset Pass in the URI to be checked.
 * @param asset The result after processing.
 * @return Returns the execution result.
 * @return Returns true or false.
 * @see fileuri
 * @since 12
 */
int OH_FileUri_CheckUriFormate(const char *uri, bool *result);
#ifdef __cplusplus
};
#endif
#endif //FILE_URI_H
