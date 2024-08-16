/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_I_EXTENSION_IPC_INTERFACE_CODE_H
#define OHOS_FILEMGMT_BACKUP_I_EXTENSION_IPC_INTERFACE_CODE_H

/*SAID: 5203*/
namespace OHOS::FileManagement::Backup {
enum class IExtensionInterfaceCode {
    CMD_GET_FILE_HANDLE = 1,
    CMD_HANDLE_CLAER,
    CMD_PUBLISH_FILE,
    CMD_HANDLE_BACKUP,
    CMD_HANDLE_RESTORE,
    CMD_GET_INCREMENTAL_FILE_HANDLE,
    CMD_PUBLISH_INCREMENTAL_FILE,
    CMD_HANDLE_INCREMENTAL_BACKUP,
    CMD_GET_INCREMENTAL_BACKUP_FILE_HANDLE,
    CMD_GET_BACKUP_INFO,
    CMD_INCREMENTAL_ON_BACKUP,
    CMD_UPDATE_FD_SENDRATE,
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_EXTENSION_IPC_INTERFACE_CODE_H