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

#include "module_app_gallery/app_gallery_dispose_proxy.h"

#include <string>
#include <gtest/gtest.h>

#include "message_parcel.h"
#include "want.h"

#include "module_app_gallery/app_gallery_service_connection.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

AppGalleryDisposeProxy::AppGalleryDisposeProxy()
{
    GTEST_LOG_(INFO) << "AppGalleryDisposeProxy is OK";
}

AppGalleryDisposeProxy::~AppGalleryDisposeProxy()
{
    GTEST_LOG_(INFO) << "~AppGalleryDisposeProxy is OK";
}

sptr<AppGalleryDisposeProxy> AppGalleryDisposeProxy::GetInstance()
{
    return new AppGalleryDisposeProxy;
}

DisposeErr AppGalleryDisposeProxy::StartBackup(const std::string &bundleName, const int32_t userId)
{
    return DoDispose(bundleName, DisposeOperation::START_BACKUP, userId);
}

DisposeErr AppGalleryDisposeProxy::EndBackup(const std::string &bundleName, const int32_t userId)
{
    return DoDispose(bundleName, DisposeOperation::END_BACKUP, userId);
}

DisposeErr AppGalleryDisposeProxy::StartRestore(const std::string &bundleName, const int32_t userId)
{
    return DoDispose(bundleName, DisposeOperation::START_RESTORE, userId);
}

DisposeErr AppGalleryDisposeProxy::EndRestore(const std::string &bundleName, const int32_t userId)
{
    return DoDispose(bundleName, DisposeOperation::END_RESTORE, userId);
}

DisposeErr AppGalleryDisposeProxy::DoDispose(const std::string &bundleName, DisposeOperation disposeOperation,
    const int32_t userId)
{
    return DisposeErr::OK;
}

} // namespace OHOS::FileManagement::Backup
