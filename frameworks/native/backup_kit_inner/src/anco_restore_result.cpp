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

#include "anco_restore_result.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "message_parcel.h"

#include <cstdint>

namespace OHOS::FileManagement::Backup {
using namespace std;

bool AncoRestoreResult::Marshalling(Parcel &parcel) const
{
    auto messageParcel = static_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        HILOGE("Failed to cast parcel to MessageParcel");
        return false;
    }

    if (!messageParcel->WriteInt64(successCount) || !messageParcel->WriteInt64(duplicateCount) ||
        !messageParcel->WriteInt64(failedCount)) {
        HILOGE("Failed to write count");
        return false;
    }

    int isDbFdValid = (dbFd != nullptr && dbFd->Get() >= 0) ? 1 : 0;
    if (!messageParcel->WriteInt32(isDbFdValid)) {
        HILOGE("Failed to write main file descriptor validity");
        return false;
    }
    if (isDbFdValid) {
        if (!messageParcel->WriteFileDescriptor(dbFd->Get())) {
            HILOGE("Failed to write db file descriptor: fd=%d", dbFd->Get());
            return false;
        }
    }

    return true;
}

bool AncoRestoreResult::ReadFromParcel(Parcel &parcel)
{
    auto messageParcel = static_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        HILOGE("Failed to cast parcel to MessageParcel");
        return false;
    }

    if (!messageParcel->ReadInt64(successCount) || !messageParcel->ReadInt64(duplicateCount) ||
        !messageParcel->ReadInt64(failedCount)) {
        HILOGE("Failed to read count");
        return false;
    }

    int dbFdVal = -1;
    int isDbFdValid = 0;
    if (!messageParcel->ReadInt32(isDbFdValid)) {
        HILOGE("Failed to read main file descriptor validity");
        return false;
    }
    if (isDbFdValid) {
        dbFdVal = messageParcel->ReadFileDescriptor();
        if (dbFdVal < 0) {
            HILOGE("Failed to read db file descriptor from parcel");
            return false;
        }
        dbFd = std::make_shared<UniqueFd>(dbFdVal);
    } else {
        dbFd = nullptr;
    }

    return true;
}

AncoRestoreResult *AncoRestoreResult::Unmarshalling(Parcel &parcel)
{
    try {
        auto result = make_unique<AncoRestoreResult>();
        if (!result->ReadFromParcel(parcel)) {
            return nullptr;
        }
        return result.release();
    } catch (const bad_alloc &e) {
        HILOGE("Failed to unmarshall AncoRestoreResult because of %{public}s", e.what());
    } catch (...) {
        HILOGE("Failed to unmarshall AncoRestoreResult because of unknown exception");
    }
    return nullptr;
}
}  // namespace OHOS::FileManagement::Backup