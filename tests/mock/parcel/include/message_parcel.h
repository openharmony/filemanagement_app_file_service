/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef MOCK_OHOS_BACKUP_MESSAGE_PARCEL_H
#define MOCK_OHOS_BACKUP_MESSAGE_PARCEL_H

#include "parcel.h"

namespace OHOS {
class MessageParcel : public FileManagement::Backup::Parcel {
public:
    bool WriteFileDescriptor(int fd)
    {
        return WriteInt32(fd);
    }

    int ReadFileDescriptor()
    {
        int32_t fd = -1;
        return ReadInt32(fd) ? fd : -1;
    }
};
} // namespace OHOS

#endif // MOCK_OHOS_BACKUP_MESSAGE_PARCEL_H
