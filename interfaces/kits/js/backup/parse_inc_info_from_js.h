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
#ifndef INTERFACES_KITS_JS_SRC_MOD_BACKUP_PARSE_INCREMENT_INFO_FROM_JS_H
#define INTERFACES_KITS_JS_SRC_MOD_BACKUP_PARSE_INCREMENT_INFO_FROM_JS_H

#include <node_api.h>
#include <tuple>

#include "b_incremental_data.h"

namespace OHOS::FileManagement::Backup {
class Parse {
public:
    static bool CheckDataList(const LibN::NVal &data);
    static std::tuple<bool, std::vector<BIncrementalData>> ParseDataList(napi_env env, const napi_value& value);
    static bool VerifyAndParseParams(napi_env env, LibN::NFuncArg &funcArg,
                              bool &isPreciseScan, std::vector<BIncrementalData> &bundleNames);
};
} // namespace OHOS::FileManagement::Backup
#endif //INTERFACES_KITS_JS_SRC_MOD_BACKUP_PARSE_INCREMENT_INFO_FROM_JS_H