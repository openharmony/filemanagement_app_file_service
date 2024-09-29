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

#include "b_sa_utils_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
bool SAUtils::IsSABundleName(string bundleName)
{
    return BSAUtils::utils->IsSABundleName(bundleName);
}

bool SAUtils::CheckBackupPermission()
{
    return BSAUtils::utils->CheckBackupPermission();
}

bool SAUtils::CheckPermission(const string &permission)
{
    return BSAUtils::utils->CheckPermission(permission);
}

bool SAUtils::IsSystemApp()
{
    return BSAUtils::utils->IsSystemApp();
}
} // namespace OHOS::FileManagement::Backup