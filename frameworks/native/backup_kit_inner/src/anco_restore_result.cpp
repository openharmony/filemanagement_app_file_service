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

static bool WriteErrorCodeList(Parcel &parcel, const std::vector<ErrCode> &errList)
{
    if (!parcel.WriteUint64(errList.size())) {
        HILOGE("Failed to write error code list size");
        return false;
    }
    for (const auto &v : errList) {
        if (!parcel.WriteInt32(v)) {
            HILOGE("Failed to WriteErrorCodeList");
            return false;
        }
    }
    return true;
}

static bool ReadErrorCodeList(Parcel &parcel, std::vector<ErrCode> &errList)
{
    uint64_t size = 0;
    if (!parcel.ReadUint64(size)) {
        HILOGE("Failed to read error code list size");
        return false;
    }
    std::vector<ErrCode> tempErrList;
    while (size > 0) {
        size--;
        int32_t v;
        if (!parcel.ReadInt32(v)) {
            HILOGE("Failed to ReadErrorCodeList");
            return false;
        }
        tempErrList.push_back(static_cast<ErrCode>(v));
    }
    errList = std::move(tempErrList);
    return true;
}

static bool WriteEndFileMap(Parcel &parcel, const std::map<std::string, int64_t> &endFileMap)
{
    if (!parcel.WriteUint64(endFileMap.size())) {
        HILOGE("Failed to write end map size");
        return false;
    }
    for (const auto &[k, v] : endFileMap) {
        if (!parcel.WriteString(k) || !parcel.WriteInt64(v)) {
            HILOGE("Failed to WriteEndFileMap");
            return false;
        }
    }
    return true;
}

static bool ReadEndFileMap(Parcel &parcel, std::map<std::string, int64_t> &endFileMap)
{
    uint64_t size = 0;
    if (!parcel.ReadUint64(size)) {
        HILOGE("Failed to read end map size");
        return false;
    }
    std::map<std::string, int64_t> tempEndFileMap;
    while (size > 0) {
        size--;
        std::string key;
        int64_t value;
        if (!parcel.ReadString(key) || !parcel.ReadInt64(value)) {
            HILOGE("Failed to ReadEndFileMap");
            return false;
        }
        tempEndFileMap.emplace(key, value);
    }
    endFileMap = std::move(tempEndFileMap);
    return true;
}

static bool WriteErrFileMap(Parcel &parcel, const std::map<std::string, std::vector<ErrCode>> &errFileMap)
{
    if (!parcel.WriteUint64(errFileMap.size())) {
        HILOGE("Failed to write err map size");
        return false;
    }
    for (const auto &[k, v] : errFileMap) {
        if (!parcel.WriteString(k) || !WriteErrorCodeList(parcel, v)) {
            HILOGE("Failed to WriteErrFileMap");
            return false;
        }
    }
    return true;
}

static bool ReadErrFileMap(Parcel &parcel, std::map<std::string, std::vector<ErrCode>> &errFileMap)
{
    uint64_t size = 0;
    if (!parcel.ReadUint64(size)) {
        HILOGE("Failed to read err map size");
        return false;
    }
    std::map<std::string, std::vector<ErrCode>> tempErrFileMap;
    while (size > 0) {
        size--;
        std::string key;
        std::vector<ErrCode> value;
        if (!parcel.ReadString(key) || !ReadErrorCodeList(parcel, value)) {
            HILOGE("Failed to ReadErrFileMap");
            return false;
        }
        tempErrFileMap.emplace(key, value);
    }
    errFileMap = std::move(tempErrFileMap);
    return true;
}

bool AncoRestoreResult::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt64(successCount) || !parcel.WriteInt64(duplicateCount) || !parcel.WriteInt64(failedCount)) {
        HILOGE("Failed to write count");
        return false;
    }
    if (!WriteEndFileMap(parcel, endFileInfos)) {
        HILOGE("Failed to write EndFileMap");
        return false;
    }
    if (!WriteErrFileMap(parcel, errFileInfos)) {
        HILOGE("Failed to write ErrFileMap");
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
    if (!ReadEndFileMap(parcel, endFileInfos)) {
        HILOGE("Failed to read EndFileMap");
        return false;
    }
    if (!ReadErrFileMap(parcel, errFileInfos)) {
        HILOGE("Failed to read ErrFileMap");
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