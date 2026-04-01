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

static bool WriteVector(Parcel &parcel, const std::vector<ErrCode> &vec)
{
    if (!parcel.WriteUint64(vec.size())) {
        HILOGE("Failed to write vector size");
        return false;
    }
    for (const auto &v : vec) {
        if (!parcel.WriteInt32(v)) {
            HILOGE("Failed to WriteVector");
            return false;
        }
    }
    return true;
}

static bool ReadVector(Parcel &parcel, std::vector<ErrCode> &vec)
{
    uint64_t size = 0;
    if (!parcel.ReadUint64(size)) {
        HILOGE("Failed to ReadVector");
        return false;
    }
    while (size > 0) {
        size--;
        int32_t v;
        if (!parcel.ReadInt32(v)) {
            HILOGE("Failed to ReadVector");
            return false;
        }
        vec.push_back(static_cast<ErrCode>(v));
    }
    return true;
}

static bool WriteMap(Parcel &parcel, const std::map<std::string, int64_t> &mp)
{
    if (!parcel.WriteUint64(mp.size())) {
        HILOGE("Failed to write map size");
        return false;
    }
    for (const auto &[k, v] : mp) {
        if (!parcel.WriteString(k) || !parcel.WriteInt64(v)) {
            HILOGE("Failed to WriteMap");
            return false;
        }
    }
    return true;
}

static bool ReadMap(Parcel &parcel, std::map<std::string, int64_t> &mp)
{
    uint64_t size = 0;
    if (!parcel.ReadUint64(size)) {
        HILOGE("Failed to ReadMap");
        return false;
    }
    while (size > 0) {
        size--;
        std::string key;
        int64_t value;
        if (!parcel.ReadString(key) || !parcel.ReadInt64(value)) {
            HILOGE("Failed to ReadMap");
            return false;
        }
        mp.emplace(key, value);
    }
    return true;
}

static bool WriteMap2(Parcel &parcel, const std::map<std::string, std::vector<ErrCode>> &mp)
{
    if (!parcel.WriteUint64(mp.size())) {
        HILOGE("Failed to WriteMap2");
        return false;
    }
    for (const auto &[k, v] : mp) {
        if (!parcel.WriteString(k) || !WriteVector(parcel, v)) {
            HILOGE("Failed to WriteMap2");
            return false;
        }
    }
    return true;
}

static bool ReadMap2(Parcel &parcel, std::map<std::string, std::vector<ErrCode>> &mp)
{
    uint64_t size = 0;
    if (!parcel.ReadUint64(size)) {
        HILOGE("Failed to ReadMap2");
        return false;
    }
    while (size > 0) {
        size--;
        std::string key;
        std::vector<ErrCode> value;
        if (!parcel.ReadString(key) || !ReadVector(parcel, value)) {
            HILOGE("Failed to ReadMap2");
            return false;
        }
        mp.emplace(key, value);
    }
    return true;
}

bool AncoRestoreResult::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt64(successCount) || !parcel.WriteInt64(duplicateCount) || !parcel.WriteInt64(failedCount) ||
        !WriteMap(parcel, endFileInfos) || !WriteMap2(parcel, errFileInfos)) {
        HILOGE("Failed to Marshalling");
        return false;
    }
    return true;
}

bool AncoRestoreResult::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadInt64(successCount) || !parcel.ReadInt64(duplicateCount) || !parcel.ReadInt64(failedCount) ||
        !ReadMap(parcel, endFileInfos) || !ReadMap2(parcel, errFileInfos)) {
        HILOGE("Failed to ReadFromParcel");
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