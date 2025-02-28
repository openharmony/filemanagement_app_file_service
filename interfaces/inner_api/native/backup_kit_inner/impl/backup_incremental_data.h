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

#ifndef OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_DATA_H
#define OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_DATA_H

#include <memory>
#include <string>

#include "parcel.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
struct BIncrementalData : public Parcelable {
    std::string bundleName;
    int64_t lastIncrementalTime;
    int32_t manifestFd;
    std::string backupParameters;
    int32_t backupPriority;

    BIncrementalData() = default;
    BIncrementalData(std::string name, int64_t nTime, int fd = -1, std::string parameters = "", int32_t priority = 0)
        : bundleName(name), lastIncrementalTime(nTime), manifestFd(fd), backupParameters(parameters),
          backupPriority(priority)
    {
    }
    ~BIncrementalData() override = default;

    bool ReadFromParcel(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
    static BIncrementalData *Unmarshalling(Parcel &parcel);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_DATA_H
