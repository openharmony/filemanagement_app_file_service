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

#ifndef FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_FILE_SHARE_TAIHE_GRANT_PERMISSONS_H
#define FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_FILE_SHARE_TAIHE_GRANT_PERMISSONS_H

#include "ohos.fileshare.fileShare.proj.hpp"
#include "ohos.fileshare.fileShare.impl.hpp"
#include "taihe/runtime.hpp"
#include "file_permission.h"
#include "iremote_broker.h"

namespace ANI::FileShare {

    ohos::fileshare::fileShare::PolicyInfo MakePolicyInfo(taihe::string_view uri, int32_t operationMode);
    ohos::fileshare::fileShare::PathPolicyInfo MakePathPolicyInfo(taihe::string_view path,
        ohos::fileshare::fileShare::OperationMode operationMode);
    void ActivatePermissionSync(taihe::array_view<ohos::fileshare::fileShare::PolicyInfo> policies);
    void DeactivatePermissionSync(taihe::array_view<ohos::fileshare::fileShare::PolicyInfo> policies);
    void GrantUriPermissionSync(taihe::string_view uri, taihe::string_view bundleName, uintptr_t flag);
    taihe::array<bool> CheckPathPermissionSync(int32_t tokenID,
        taihe::array_view<ohos::fileshare::fileShare::PathPolicyInfo> policies,
        ohos::fileshare::fileShare::PolicyType policyType);

    struct PolicyErrorArgs {
        std::deque<OHOS::AppFileService::PolicyErrorResult> errorResults;
        int32_t errNo = 0;
        ~PolicyErrorArgs() = default;
    };

    struct PolicyInfoResultArgs {
        std::vector<bool> resultData;
        int32_t errNo = 0;
        ~PolicyInfoResultArgs() = default;
    };

    struct UriPermissionInfo {
        unsigned int flag;
        std::string mode;
        std::string bundleName;
        std::string uri;
    };

    enum MediaFileTable {
        FILE_TABLE = 0,
        PHOTO_TABLE = 1,
        AUDIO_TABLE = 2,
    };

class FileShareGrantToken : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.fileshare.grantUriPermission");

    FileShareGrantToken() = default;
    virtual ~FileShareGrantToken() noexcept = default;
};
} //namespace ANI::FileShare

#endif // FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_FILE_SHARE_TAIHE_GRANT_PERMISSONS_H
