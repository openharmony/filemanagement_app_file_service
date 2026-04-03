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

#ifndef OHOS_FILEMGMT_BACKUP_STAT_INFO_H
#define OHOS_FILEMGMT_BACKUP_STAT_INFO_H

#include "parcel.h"
#include <sys/stat.h>

namespace OHOS::FileManagement::Backup {
struct StatInfo : public Parcelable {
    struct stat sta;

    StatInfo() = default;
    StatInfo(const struct stat &sta_) : sta(sta_)
    {}
    ~StatInfo() override = default;

    bool ReadFromParcel(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
    static StatInfo *Unmarshalling(Parcel &parcel);
};
}  // namespace OHOS::FileManagement::Backup
#endif