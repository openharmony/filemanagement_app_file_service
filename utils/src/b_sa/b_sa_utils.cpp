/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "b_sa/b_sa_utils.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"

namespace OHOS::FileManagement::Backup {

namespace {
    const std::string BACKUP_PERMISSION = "ohos.permission.BACKUP";
}

bool SAUtils::IsSABundleName(std::string bundleName)
{
    if (bundleName.empty()) {
        return false;
    }
    for (size_t i = 0; i < bundleName.size(); i++) {
        if (!std::isdigit(bundleName[i])) {
            return false;
        }
    }
    return true;
}

bool SAUtils::CheckBackupPermission()
{
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    return Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, BACKUP_PERMISSION) ==
        Security::AccessToken::PermissionState::PERMISSION_GRANTED;
}

bool SAUtils::CheckPermission(const std::string &permission)
{
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    return Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission) ==
        Security::AccessToken::PermissionState::PERMISSION_GRANTED;
}

bool SAUtils::IsSystemApp()
{
    uint64_t fullTokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
}
} // namespace OHOS::FileManagement::Backup