/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef INTERFACES_KITS_JS_FILE_URI_PROP_N_EXPORTER_H
#define INTERFACES_KITS_JS_FILE_URI_PROP_N_EXPORTER_H

#include <string>

#include "filemgmt_libn.h"

namespace OHOS::AppFileService::ModuleFileUri {
class PropNExporter final : public FileManagement::LibN::NExporter {
public:
    inline static const std::string className = "GetUriFromPath";

    bool Export() override;
    std::string GetClassName() override;

    PropNExporter(napi_env env, napi_value exports);
    ~PropNExporter() override;
};
} // namespace OHOS::AppFileService::ModuleFileUri
#endif // INTERFACES_KITS_JS_FILE_URI_PROP_N_EXPORTER_H