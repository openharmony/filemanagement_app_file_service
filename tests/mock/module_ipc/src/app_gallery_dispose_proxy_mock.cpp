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

#include "app_gallery_dispose_proxy_mock.h"

#include <string>

namespace OHOS::FileManagement::Backup {
using namespace std;

mutex AppGalleryDisposeProxy::instanceLock_;
sptr<AppGalleryDisposeProxy> AppGalleryDisposeProxy::appGalleryDisposeProxyInstance_;

AppGalleryDisposeProxy::AppGalleryDisposeProxy() {}

AppGalleryDisposeProxy::~AppGalleryDisposeProxy()
{
    appGalleryDisposeProxyInstance_ = nullptr;
}

sptr<AppGalleryDisposeProxy> AppGalleryDisposeProxy::GetInstance()
{
    if (appGalleryDisposeProxyInstance_ == nullptr) {
        lock_guard<mutex> autoLock(instanceLock_);
        if (appGalleryDisposeProxyInstance_ == nullptr) {
            appGalleryDisposeProxyInstance_ = sptr(new AppGalleryDisposeProxy);
        }
    }

    return appGalleryDisposeProxyInstance_;
}

DisposeErr AppGalleryDisposeProxy::StartBackup(const string &bundleName)
{
    return BAppGalleryDisposeProxy::proxy->StartBackup(bundleName);
}

DisposeErr AppGalleryDisposeProxy::EndBackup(const string &bundleName)
{
    return BAppGalleryDisposeProxy::proxy->EndBackup(bundleName);
}

DisposeErr AppGalleryDisposeProxy::StartRestore(const string &bundleName)
{
    return BAppGalleryDisposeProxy::proxy->StartRestore(bundleName);
}

DisposeErr AppGalleryDisposeProxy::EndRestore(const string &bundleName)
{
    return BAppGalleryDisposeProxy::proxy->EndRestore(bundleName);
}
} // namespace OHOS::FileManagement::Backup
