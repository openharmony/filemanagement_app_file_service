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

#include <tuple>

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
    int64_t stSize = 0;
    uint32_t stMode = 0;
    int64_t stAtimTvSec = 0;
    int64_t stAtimTvNsec = 0;
    int64_t stMtimTvSec = 0;
    int64_t stMtimTvNsec = 0;
    if (!parcel.ReadInt64(stSize) || !parcel.ReadUint32(stMode) || !parcel.ReadInt64(stAtimTvSec) ||
        !parcel.ReadInt64(stAtimTvNsec) || !parcel.ReadInt64(stMtimTvSec) ||
        !parcel.ReadInt64(stMtimTvNsec)) {
        HILOGE("Failed to ReadFromParcel");
        return false;
    }
    sta.st_size = stSize;
    sta.st_mode = stMode;
    sta.st_atim.tv_sec = stAtimTvSec;
    sta.st_atim.tv_nsec = stAtimTvNsec;
    sta.st_mtim.tv_sec = stMtimTvSec;
    sta.st_mtim.tv_nsec = stMtimTvNsec;
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