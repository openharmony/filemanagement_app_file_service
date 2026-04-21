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
#include <cstdint>

namespace OHOS::FileManagement::Backup {
using namespace std;

bool AncoRestoreResult::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt64(successCount) || !parcel.WriteInt64(duplicateCount) || !parcel.WriteInt64(failedCount)) {
        HILOGE("Failed to write count");
        return false;
    }

    return true;
}

bool AncoRestoreResult::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadInt64(successCount) || !parcel.ReadInt64(duplicateCount) || !parcel.ReadInt64(failedCount)) {
        HILOGE("Failed to read count");
        return false;
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