/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License") = 0;
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

#ifndef OHOS_FILEMGMT_BACKUP_APP_GALLERY_DISPOSE_PROXY_MOCK_H
#define OHOS_FILEMGMT_BACKUP_APP_GALLERY_DISPOSE_PROXY_MOCK_H

#include <gmock/gmock.h>

#include "app_gallery_dispose_proxy.h"

namespace OHOS::FileManagement::Backup {
class BAppGalleryDisposeProxy {
public:
    virtual DisposeErr StartBackup(const std::string&) = 0;
    virtual DisposeErr EndBackup(const std::string&) = 0;
    virtual DisposeErr StartRestore(const std::string&) = 0;
    virtual DisposeErr EndRestore(const std::string&) = 0;
public:
    BAppGalleryDisposeProxy() = default;
    virtual ~BAppGalleryDisposeProxy() = default;
public:
    static inline std::shared_ptr<BAppGalleryDisposeProxy> proxy = nullptr;
};

class AppGalleryDisposeProxyMock : public BAppGalleryDisposeProxy {
public:
    MOCK_METHOD(DisposeErr, StartBackup, (const std::string&));
    MOCK_METHOD(DisposeErr, EndBackup, (const std::string&));
    MOCK_METHOD(DisposeErr, StartRestore, (const std::string&));
    MOCK_METHOD(DisposeErr, EndRestore, (const std::string&));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_APP_GALLERY_DISPOSE_PROXY_MOCK_H