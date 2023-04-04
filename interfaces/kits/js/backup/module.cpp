/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <memory>
#include <vector>

#include "filemgmt_libhilog.h"
#include "filemgmt_libn.h"
#include "prop_n_exporter.h"
#include "session_backup_n_exporter.h"
#include "session_restore_n_exporter.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace LibN;

static napi_value Export(napi_env env, napi_value exports)
{
    std::vector<unique_ptr<NExporter>> products;
    products.emplace_back(make_unique<PropNExporter>(env, exports));
    products.emplace_back(make_unique<SessionBackupNExporter>(env, exports));
    products.emplace_back(make_unique<SessionRestoreNExporter>(env, exports));
    for (auto &&product : products) {
        if (!product->Export()) {
            HILOGE("INNER BUG. Failed to export class %{public}s for module backup", product->GetClassName().c_str());
            return nullptr;
        } else {
            HILOGI("Class %{public}s for module fileio has been exported", product->GetClassName().c_str());
        }
    }
    return exports;
}

NAPI_MODULE(backup, Export)
} // namespace OHOS::FileManagement::Backup