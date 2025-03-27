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

#include "backup_incremental_data.h"

#include "filemgmt_libhilog.h"
#include "message_parcel.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

bool BIncrementalData::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(bundleName) || !parcel.WriteInt64(lastIncrementalTime) ||
        !parcel.WriteString(backupParameters) || !parcel.WriteInt32(backupPriority)) {
        HILOGE("Failed");
        return false;
    }
    auto msgParcel = static_cast<MessageParcel *>(&parcel);
    msgParcel->WriteFileDescriptor(manifestFd);
    return true;
}

bool BIncrementalData::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(bundleName) || !parcel.ReadInt64(lastIncrementalTime) ||
        !parcel.ReadString(backupParameters) || !parcel.ReadInt32(backupPriority)) {
        HILOGE("Failed");
        return false;
    }
    auto msgParcel = static_cast<MessageParcel *>(&parcel);
    manifestFd = msgParcel->ReadFileDescriptor();
    return true;
}

BIncrementalData *BIncrementalData::Unmarshalling(Parcel &parcel)
{
    try {
        auto result = make_unique<BIncrementalData>();
        if (!result->ReadFromParcel(parcel)) {
            return nullptr;
        }
        return result.release();
    } catch (const bad_alloc &e) {
        HILOGE("Failed to unmarshall BIncrementalData because of %{public}s", e.what());
    }
    return nullptr;
}
} // namespace OHOS::FileManagement::Backup