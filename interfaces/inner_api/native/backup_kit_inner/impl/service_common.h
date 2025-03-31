<<<<<<<< HEAD:services/backup_sa/ExtensionType.idl
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

package OHOS.FileManagement.Backup.ExtensionType;

struct UniqueFdGroup {
    FileDescriptor fd;
    FileDescriptor reportFd;
    int errCode;
};
========
/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_COMMON_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_COMMON_H

namespace OHOS::FileManagement::Backup {
const int DEFAULT_INVAL_VALUE = -1;

typedef enum TypeRestoreTypeEnum {
    RESTORE_DATA_WAIT_SEND = 0,
    RESTORE_DATA_READDY = 1,
} RestoreTypeEnum;

} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_COMMON_H
>>>>>>>> idl_extension_modify:interfaces/inner_api/native/backup_kit_inner/impl/service_common.h
