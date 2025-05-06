#ifndef FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_FILE_SHARE_TAIHE_GRANT_PERMISSONS_H
#define FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_FILE_SHARE_TAIHE_GRANT_PERMISSONS_H

/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos.fileshare.proj.hpp"
#include "ohos.fileshare.impl.hpp"
#include "taihe/runtime.hpp"
#include "file_permission.h"

namespace ANI::fileShare {
using namespace taihe;
using namespace ohos::fileshare;

    PolicyInfo makePolicyInfo(string_view uri, int32_t operationMode);
    void activatePermissionSync(array_view<PolicyInfo> policies);
    void deactivatePermissionSync(array_view<PolicyInfo> policies);

    struct PolicyErrorArgs {
        std::deque<OHOS::AppFileService::PolicyErrorResult> errorResults;
        int32_t errNo = 0;
        ~PolicyErrorArgs() = default;
    };

} //namespace ANI::fileShare

#endif // FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_FILE_SHARE_TAIHE_GRANT_PERMISSONS_H