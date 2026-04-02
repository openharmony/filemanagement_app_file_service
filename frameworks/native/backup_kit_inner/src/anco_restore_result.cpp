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

static bool WriteErrorCodeList(Parcel &parcel, const std::vector<ErrCode> &vec)
{
    if (!parcel.WriteUint64(vec.size())) {
        HILOGE("Failed to write error code list size");
        return false;
    }
    for (const auto &v : vec) {
        if (!parcel.WriteInt32(v)) {
            HILOGE("Failed to WriteErrorCodeList");
            return false;
        }
    }
    return true;
}

static bool ReadErrorCodeList(Parcel &parcel, std::vector<ErrCode> &vec)
{
    uint64_t size = 0;
    if (!parcel.ReadUint64(size)) {
        HILOGE("Failed to read error code list size");
        return false;
    }
    while (size > 0) {
        size--;
        int32_t v;
        if (!parcel.ReadInt32(v)) {
            HILOGE("Failed to ReadErrorCodeList");
            return false;
        }
        vec.push_back(static_cast<ErrCode>(v));
    }
    return true;
}

static bool WriteEndFileMap(Parcel &parcel, const std::map<std::string, int64_t> &mp)
{
    if (!parcel.WriteUint64(mp.size())) {
        HILOGE("Failed to write end map size");
        return false;
    }
    for (const auto &[k, v] : mp) {
        if (!parcel.WriteString(k) || !parcel.WriteInt64(v)) {
            HILOGE("Failed to WriteEndFileMap");
            return false;
        }
    }
    return true;
}

static bool ReadEndFileMap(Parcel &parcel, std::map<std::string, int64_t> &mp)
{
    uint64_t size = 0;
    if (!parcel.ReadUint64(size)) {
        HILOGE("Failed to read end map size");
        return false;
    }
    while (size > 0) {
        size--;
        std::string key;
        int64_t value;
        if (!parcel.ReadString(key) || !parcel.ReadInt64(value)) {
            HILOGE("Failed to ReadEndFileMap");
            return false;
        }
        mp.emplace(key, value);
    }
    return true;
}

static bool WriteErrFileMap(Parcel &parcel, const std::map<std::string, std::vector<ErrCode>> &mp)
{
    if (!parcel.WriteUint64(mp.size())) {
        HILOGE("Failed to write err map size");
        return false;
    }
    for (const auto &[k, v] : mp) {
        if (!parcel.WriteString(k) || !WriteErrorCodeList(parcel, v)) {
            HILOGE("Failed to WriteErrFileMap");
            return false;
        }
    }
    return true;
}

static bool ReadErrFileMap(Parcel &parcel, std::map<std::string, std::vector<ErrCode>> &mp)
{
    uint64_t size = 0;
    if (!parcel.ReadUint64(size)) {
        HILOGE("Failed to read err map size");
        return false;
    }
    while (size > 0) {
        size--;
        std::string key;
        std::vector<ErrCode> value;
        if (!parcel.ReadString(key) || !ReadErrorCodeList(parcel, value)) {
            HILOGE("Failed to ReadErrFileMap");
            return false;
        }
        mp.emplace(key, value);
    }
    return true;
}

bool AncoRestoreResult::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt64(successCount) || !parcel.WriteInt64(duplicateCount) || !parcel.WriteInt64(failedCount) ||
        !WriteEndFileMap(parcel, endFileInfos) || !WriteErrFileMap(parcel, errFileInfos)) {
        HILOGE("Failed to Marshalling");
        return false;
    }
    return true;
}

bool AncoRestoreResult::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadInt64(successCount) || !parcel.ReadInt64(duplicateCount) || !parcel.ReadInt64(failedCount) ||
        !ReadEndFileMap(parcel, endFileInfos) || !ReadErrFileMap(parcel, errFileInfos)) {
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