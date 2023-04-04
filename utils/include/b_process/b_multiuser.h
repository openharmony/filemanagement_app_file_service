/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_MULTIUSER_H
#define OHOS_FILEMGMT_BACKUP_B_MULTIUSER_H

#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
class BMultiuser {
public:
    struct UidReadOut {
        int userId;
        int appId;
    };

public:
    static UidReadOut ParseUid(int uid)
    {
        return UidReadOut {
            .userId = uid / BConstants::SPAN_USERID_UID,
            .appId = uid % BConstants::SPAN_USERID_UID,
        };
    }
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_MULTIUSER_H