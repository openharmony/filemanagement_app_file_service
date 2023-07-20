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

#ifndef OHOS_FILEMGMT_BACKUP_BUNDLE_MGR_ADAPTER_H
#define OHOS_FILEMGMT_BACKUP_BUNDLE_MGR_ADAPTER_H

#include <string>
#include <vector>

#include "b_json/b_json_entity_caps.h"
#include "bundlemgr/bundle_mgr_interface.h"

namespace OHOS::FileManagement::Backup {
class InnerReceiverImpl;

class BundleMgrAdapter {
public:
    /**
     * @brief Get the Bundle Infos object
     *
     * @param userId User ID
     * @return std::vector<BJsonEntityCaps::BundleInfo>
     */
    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfos(int32_t userId);

    /**
     * @brief Get the bundle infos object
     *
     * @param bundleNames bundle names
     * @param userId User ID
     * @return std::vector<BJsonEntityCaps::BundleInfo>
     */
    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfos(
                                    const std::vector<std::string> &bundleNames, int32_t userId);

    /**
     * @brief Install bundle
     *
     * @param bundleName bundle name
     * @param bundleFilePath bundle file path
     * @param userId User ID
     */
    static ErrCode Install(wptr<InnerReceiverImpl> statusReceiver, const std::string &bundleFilePath, int32_t userId);
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_BUNDLE_MGR_ADAPTER_H
