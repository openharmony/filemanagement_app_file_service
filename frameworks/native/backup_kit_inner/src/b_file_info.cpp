/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "b_file_info.h"

#include "filemgmt_libhilog.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

bool BFileInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(owner) || !parcel.WriteString(fileName) || !parcel.WriteUint32(sn)) {
        HILOGE("Failed");
        return false;
    }
    return true;
}

bool BFileInfo::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(owner) || !parcel.ReadString(fileName) || !parcel.ReadUint32(sn)) {
        HILOGE("Failed");
        return false;
    }
    return true;
}

BFileInfo *BFileInfo::Unmarshalling(Parcel &parcel)
{
    try {
        auto result = make_unique<BFileInfo>();
        if (!result->ReadFromParcel(parcel)) {
            return nullptr;
        }
        return result.release();
    } catch (const bad_alloc &e) {
        HILOGE("Failed to unmarshall BFileInfo because of %{public}s", e.what());
    }
    return nullptr;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS