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

#ifndef OHOS_FILEMGMT_BACKUP_I_APP_GALLERY_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_I_APP_GALLERY_SERVICE_H

#include <string>

#include "iremote_proxy.h"

namespace OHOS::FileManagement::Backup {
class IAppGalleryService : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileManagement.Backup.IAppGalleryService");

    enum class DisposeOperation {
        START_RESTORE = 3,
        END_RESTORE = 4,
        START_BACKUP = 5,
        END_BACKUP = 6,
    };

    enum Errcode {
        ERR_BASE = (-99),
        ERR_FAILED = (-1),
        ERR_PERMISSION_DENIED = (-2),
        ERR_OK = 0,
    };
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_APP_GALLERY_SERVICE_H