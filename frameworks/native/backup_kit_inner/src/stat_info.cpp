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

#include "stat_info.h"

#include "filemgmt_libhilog.h"
#include "message_parcel.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
bool StatInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt64(sta.st_size) || !parcel.WriteUint32(sta.st_mode) || !parcel.WriteInt64(sta.st_atim.tv_sec) ||
        !parcel.WriteInt64(sta.st_atim.tv_nsec) || !parcel.WriteInt64(sta.st_mtim.tv_sec) ||
        !parcel.WriteInt64(sta.st_mtim.tv_nsec)) {
        HILOGE("Failed to Marshalling");
        return false;
    }
    return true;
}

bool StatInfo::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadInt64(sta.st_size) || !parcel.ReadUint32(sta.st_mode) || !parcel.ReadInt64(sta.st_atim.tv_sec) ||
        !parcel.ReadInt64(sta.st_atim.tv_nsec) || !parcel.ReadInt64(sta.st_mtim.tv_sec) ||
        !parcel.ReadInt64(sta.st_mtim.tv_nsec)) {
        HILOGE("Failed to ReadFromParcel");
        return false;
    }
    return true;
}

StatInfo *StatInfo::Unmarshalling(Parcel &parcel)
{
    try {
        auto result = make_unique<StatInfo>();
        if (!result->ReadFromParcel(parcel)) {
            return nullptr;
        }
        return result.release();
    } catch (const bad_alloc &e) {
        HILOGE("Failed to unmarshall Unmarshalling because of %{public}s", e.what());
    } catch (...) {
        HILOGE("Failed to unmarshall Unmarshalling because of unknown exception");
    }
    return nullptr;
}
}  // namespace OHOS::FileManagement::Backup