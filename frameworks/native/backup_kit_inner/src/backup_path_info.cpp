/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "backup_path_info.h"

#include "filemgmt_libhilog.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

bool BPathInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(srcPath) || !parcel.WriteString(destPath)) {
        HILOGE("Failed to marshall BPathInfo");
        return false;
    }
    return true;
}

bool BPathInfo::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(srcPath) || !parcel.ReadString(destPath)) {
        HILOGE("Failed to read BPathInfo from parcel");
        return false;
    }
    return true;
}

BPathInfo *BPathInfo::Unmarshalling(Parcel &parcel)
{
    try {
        auto result = make_unique<BPathInfo>();
        if (!result->ReadFromParcel(parcel)) {
            return nullptr;
        }
        return result.release();
    } catch (const bad_alloc &e) {
        HILOGE("Failed to unmarshall BPathInfo because of %{public}s", e.what());
    }
    return nullptr;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS
