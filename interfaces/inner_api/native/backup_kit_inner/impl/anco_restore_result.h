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

#include <string>
#include <vector>
#include <map>

using cJSON = struct cJSON;
namespace OHOS::FileManagement::Backup {
using ErrCode = int;
class AncoRestoreResult {
public:
    uint32_t size{0};
    const void* data{nullptr};
    int64_t successCount{0};
    int64_t duplicateCount{0};
    int64_t failedCount{0};
    std::map<std::string, int64_t> endFileInfos;
    std::map<std::string, std::vector<ErrCode>> errFileInfos;

    AncoRestoreResult() = default;
    AncoRestoreResult(int64_t successCount_, int64_t duplicateCount_, int64_t failedCount_,
        const std::map<std::string, int64_t> &endFileInfos_,
        const std::map<std::string, std::vector<ErrCode>> &errFileInfos_)
        : successCount(successCount_), duplicateCount(duplicateCount_), failedCount(failedCount_),
        endFileInfos(endFileInfos_), errFileInfos(errFileInfos_)
    {}
    ~AncoRestoreResult() = default;

    void Serialize();
    int32_t RawDataCpy(const void* readdata);

private:
    void AddEndFileInfos(cJSON* root);
    void AddErrFileInfos(cJSON* root);
    bool ParseBasicCounts(cJSON* root);
    bool ParseEndFileInfos(cJSON* root);
    bool ParseErrFileInfos(cJSON* root);
};
}  // namespace OHOS::FileManagement::Backup
#endif