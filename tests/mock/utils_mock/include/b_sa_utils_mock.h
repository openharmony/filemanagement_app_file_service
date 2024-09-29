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

#ifndef OHOS_FILEMGMT_BACKUP_B_SA_UTILS_MOCK_H
#define OHOS_FILEMGMT_BACKUP_B_SA_UTILS_MOCK_H

#include <gmock/gmock.h>

#include "b_sa/b_sa_utils.h"

namespace OHOS::FileManagement::Backup {
class BSAUtils {
public:
    virtual bool IsSABundleName(std::string bundleName) = 0;
    virtual bool CheckBackupPermission() = 0;
    virtual bool CheckPermission(const std::string &permission) = 0;
    virtual bool IsSystemApp() = 0;
public:
    BSAUtils() = default;
    virtual ~BSAUtils() = default;
public:
    static inline std::shared_ptr<BSAUtils> utils = nullptr;
};

class SAUtilsMock : public BSAUtils {
public:
    MOCK_METHOD(bool, IsSABundleName, (std::string bundleName));
    MOCK_METHOD(bool, CheckBackupPermission, ());
    MOCK_METHOD(bool, CheckPermission, (const std::string &permission));
    MOCK_METHOD(bool, IsSystemApp, ());
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_B_SA_UTILS_MOCK_H
