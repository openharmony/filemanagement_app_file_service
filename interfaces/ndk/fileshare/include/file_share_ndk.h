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

#ifndef FILE_SHARE_NDK_H
#define FILE_SHARE_NDK_H
/**
 * @addtogroup fileShare
 *
 * @brief This module provides file sharing capabilities and provides an interface for system applications to authorize
 * the Uniform Resource Identifier (URI) of public directory files with read and write permissions to other applications.
 * @since 12
 */

/**
 * @file file_share_ndk.h
 *
 * @brief Provides fileshare APIS.
 * @library libfile_share_ndk.z.so
 * @syscap SystemCapability.FileManagement.AppFileService.FolderAuthorization
 * @since 12
 */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Enumerates the uri operate mode types.
 *
 * @since 12
 */
typedef enum OH_OperationMode {
    /**
     * @brief Indicates read permissions.
     */
    READ_MODE = 0b1,

    /**
     * @brief Indicates write permissions.
     */
    WRITE_MODE = 0b10
} OH_OperationMode;

/**
 * @brief Enumerates the error code of the permission policy for the URI operation.
 *
 * @since 12
 */
typedef enum OH_PolicyErrorCode  {
    /**
     * @brief Indicates that the policy is not allowed to be persisted.
     */
    PERSISTENCE_FORBIDDEN = 1,

    /**
     * @brief Indicates that the mode of this policy is invalid.
     */
    INVALID_MODE = 2,

    /**
     * @brief Indicates that the path of this policy is invalid.
     */
    INVALID_PATH = 3
} OH_PolicyErrorCode;

/**
 * @brief Define the OH_PolicyErrorResult structure type.
 *
 * Failed policy result on URI.
 *
 * @since 12
 */
typedef struct OH_PolicyErrorResult {
    /**
     * Indicates the failed uri of the policy information.
     */
    char* uri;

    /**
     * Indicates the error code of the failure in the policy information.
     */
    OH_PolicyErrorCode code;

    /**
     * Indicates the reason of the failure in the policy information.
     */
    char* message;
} OH_PolicyErrorResult;

/**
 * @brief Define the OH_PolicyInfo structure type.
 *
 * Policy information to manager permissions on a URI.
 *
 * @since 12
 */
typedef struct OH_PolicyInfo {
    /**
     * Indicates the uri of the policy information.
     */
    char* uri;

    /**
     * Indicates the mode of operation for the URI.
     * example { OH_OperationMode.READ_MODE } or { OH_OperationMode.READ_MODE | OH_OperationMode.WRITE_MODE }.
     */
    int operationMode;
} OH_PolicyInfo;

/**
 * @brief Set persistence permissions for the URI.
 *
 * @param policies Represents a pointer to an {@link OH_PolicyInfo} instance.
 * @param policyNum Indicates the size of the policies array.
 * @param result Represents a pointer to an {@link OH_PolicyErrorResult} instance.
 * @param resultNum Indicates the size of the result array.
 * @return Returns the status code of the execution.
 * @see OH_PolicyInfo, OH_PolicyErrorResult
 * @since 12
 */
int OH_FileShare_PersistPermission(const OH_PolicyInfo *policies, int policyNum, OH_PolicyErrorResult **result, int *resultNum);

/**
 * @brief Revoke persistence permissions for the URI.
 *
 * @param policies Represents a pointer to an {@link OH_PolicyInfo} instance.
 * @param policyNum Indicates the size of the policies array.
 * @param result Represents a pointer to an {@link OH_PolicyErrorResult} instance.
 * @param resultNum Indicates the size of the result array.
 * @return Returns the status code of the execution.
 * @see OH_PolicyInfo, OH_PolicyErrorResult
 * @since 12
 */
int OH_FileShare_RevokePermission(const OH_PolicyInfo *policies, int policyNum, OH_PolicyErrorResult **result, int *resultNum);

/**
 * @brief Enable the URI that have been permanently authorized.
 *
 * @param policies Represents a pointer to an {@link OH_PolicyInfo} instance.
 * @param policyNum Indicates the size of the policies array.
 * @param result Represents a pointer to an {@link OH_PolicyErrorResult} instance.
 * @param resultNum Indicates the size of the result array.
 * @return Returns the status code of the execution.
 * @see OH_PolicyInfo, OH_PolicyErrorResult
 * @since 12
 */
int OH_FileShare_ActivatePermission(const OH_PolicyInfo *policies, int policyNum, OH_PolicyErrorResult **result, int *resultNum);

/**
 * @brief Stop the authorized URI that has been enabled.
 *
 * @param policies Represents a pointer to an {@link OH_PolicyInfo} instance.
 * @param policyNum Indicates the size of the policies array.
 * @param result Represents a pointer to an {@link OH_PolicyErrorResult} instance.
 * @param resultNum Indicates the size of the result array.
 * @return Returns the status code of the execution.
 * @see OH_PolicyInfo, OH_PolicyErrorResult
 * @since 12
 */
int OH_FileShare_DeactivatePermission(const OH_PolicyInfo *policies, int policyNum, OH_PolicyErrorResult **result, int *resultNum);

/**
 * @brief Check persistence permissions for the URI..
 *
 * @param policies Represents a pointer to an {@link OH_PolicyInfo} instance.
 * @param policyNum Indicates the size of the policies array.
 * @param result Represents a pointer to an bool instance.
 * @return Returns the status code of the execution.
 * @see OH_PolicyInfo, OH_PolicyErrorResult
 * @since 12
 */
int OH_FileShare_CheckPersistentPermission(const OH_PolicyInfo *policies, int policyNum, bool **result);
#ifdef __cplusplus
};
#endif
#endif //FILE_SHARE_NDK_H
