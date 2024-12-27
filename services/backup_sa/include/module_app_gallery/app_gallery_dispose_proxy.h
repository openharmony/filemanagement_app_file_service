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

#ifndef OHOS_FILEMGMT_BACKUP_APP_GALLERY_DISPOSE_PROXY_H
#define OHOS_FILEMGMT_BACKUP_APP_GALLERY_DISPOSE_PROXY_H

#include <condition_variable>
#include <map>
#include <mutex>

#include <iremote_stub.h>

#include "app_gallery_service_connection.h"
#include "i_appgallery_service.h"

namespace OHOS::FileManagement::Backup {
enum class DisposeErr {
    OK = 0,
    CONN_FAIL = 1,
    IPC_FAIL = 2,
    REQUEST_FAIL = 3,
};

class AppGalleryDisposeProxy : public IRemoteStub<IAppGalleryService> {
public:
    DISALLOW_COPY_AND_MOVE(AppGalleryDisposeProxy);
    AppGalleryDisposeProxy();
    ~AppGalleryDisposeProxy();

    static sptr<AppGalleryDisposeProxy> GetInstance();

    DisposeErr StartBackup(const std::string &bundleName, const int32_t userId);
    DisposeErr EndBackup(const std::string &bundleName, const int32_t userId);
    DisposeErr StartRestore(const std::string &bundleName, const int32_t userId);
    DisposeErr EndRestore(const std::string &bundleName, const int32_t userId);

    sptr<AppGalleryConnection> GetAppGalleryConnection(const int32_t userId);
    sptr<AppGalleryConnection> ConnectAppGallery(const int32_t userId);

    static std::string abilityName;
    static std::string pkgName;
    static std::map<int32_t, sptr<AppGalleryConnection>> appGalleryConnectionMap_;

private:
    DisposeErr DoDispose(const std::string &bundleName, DisposeOperation disposeOperation, const int32_t userId);

private:
    static std::mutex instanceLock_;
    static std::mutex appGalleryConnectionLock_;
    static sptr<AppGalleryDisposeProxy> appGalleryDisposeProxyInstance_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_APP_GALLERY_DISPOSE_PROXY_H