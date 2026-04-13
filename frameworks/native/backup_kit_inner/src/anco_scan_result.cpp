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

#include "anco_scan_result.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
bool AncoScanResult::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt64(bigFileSize) || !parcel.WriteInt64(smallFileSize)) {
        HILOGE("Failed to Marshalling");
        return false;
    }
    return true;
}

bool AncoScanResult::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadInt64(bigFileSize) || !parcel.ReadInt64(smallFileSize)) {
        HILOGE("Failed to ReadFromParcel");
        return false;
    }
    return true;
}

AncoScanResult *AncoScanResult::Unmarshalling(Parcel &parcel)
{
    try {
        auto result = make_unique<AncoScanResult>();
        if (!result->ReadFromParcel(parcel)) {
            return nullptr;
        }
        return result.release();
    } catch (const bad_alloc &e) {
        HILOGE("Failed to unmarshall AncoScanResult because of %{public}s", e.what());
    } catch (...) {
        HILOGE("Failed to unmarshall AncoScanResult because of unknown exception");
    }
    return nullptr;
}
}