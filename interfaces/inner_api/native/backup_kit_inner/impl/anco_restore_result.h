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

#ifndef OHOS_FILEMGMT_BACKUP_ANCO_RESTORE_RESULT_H
#define OHOS_FILEMGMT_BACKUP_ANCO_RESTORE_RESULT_H

#include "parcel.h"
#include <string>
#include <vector>
#include <map>

namespace OHOS::FileManagement::Backup {
using ErrCode = int;
struct AncoRestoreResult : public Parcelable {
    int64_t successCount{0};
    int64_t duplicateCount{0};
    int64_t failedCount{0};
    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<ErrCode>> errFileInfos;
    off_t bigFileSize = 0;
    off_t smallFileSize = 0;
    off_t tarFileSize = 0;
    int tarFileNum = 0;
    int bigFileNum = 0;
    std::vector<int> errCodes;
    AncoRestoreResult() = default;
    AncoRestoreResult(int64_t successCount_, int64_t duplicateCount_, int64_t failedCount_)
        : successCount(successCount_), duplicateCount(duplicateCount_), failedCount(failedCount_)
    {}
    ~AncoRestoreResult() override = default;

    bool ReadFromParcel(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
    static AncoRestoreResult *Unmarshalling(Parcel &parcel);
};
}  // namespace OHOS::FileManagement::Backup
#endif
