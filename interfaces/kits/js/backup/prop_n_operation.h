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
#ifndef INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_LOCAL_CAPABILITIES_H
#define INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_LOCAL_CAPABILITIES_H

#include <node_api.h>
#include <string>

namespace OHOS::FileManagement::Backup {
class PropNOperation final {
public:
    static napi_value Async(napi_env env, napi_callback_info info);
    static napi_value DoGetBackupInfo(napi_env env, napi_callback_info info);
};

const std::string PROCEDURE_LOCALCAPABILITIES_NAME = "getLocalCapalities";
} // namespace OHOS::FileManagement::Backup
#endif // INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_LOCAL_CAPABILITIES_H