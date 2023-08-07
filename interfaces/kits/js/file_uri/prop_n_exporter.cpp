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
#include "prop_n_exporter.h"

#include "get_uri_from_path.h"

namespace OHOS::AppFileService::ModuleFileUri {
using namespace std;
using namespace FileManagement::LibN;

bool PropNExporter::Export()
{
    return exports_.AddProp({
        NVal::DeclareNapiFunction("getUriFromPath", GetUriFromPath::Sync),
    });
}

string PropNExporter::GetClassName()
{
    return PropNExporter::className;
}

PropNExporter::PropNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

PropNExporter::~PropNExporter() {}
} // namespace OHOS::AppFileService::ModuleFileUri