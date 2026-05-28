/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "module_strategy/service_strategy.h"
#include "module_ipc/service.h"
#include "b_resources/b_constants.h"
#include "module_external/bms_adapter.h"
#include "module_notify/notify_work_service.h"
#include "b_filesystem/b_dir.h"
#include "b_ohos/startup/backup_para.h"
#include "b_error/b_excep_utils.h"
#include "hitrace_meter.h"

namespace OHOS::FileManagement::Backup {

namespace {
const std::string BROADCAST_TYPE = "broadcast";
const std::string UNICAST_TYPE = "unicast";
const std::string START_TYPE = "start";

class StrategyRegistrar {
public:
    StrategyRegistrar()
    {
        auto &executor = PropertyStrategyExecutor::GetInstance();
        executor.AddStrategy("DefaultPropertyStrategy", std::make_unique<DefaultPropertyStrategy>());
        executor.AddStrategy("DataSizePropertyStrategy", std::make_unique<DataSizePropertyStrategy>());
        executor.AddStrategy("ClearDataFlagPropertyStrategy", std::make_unique<ClearDataFlagPropertyStrategy>());
        executor.AddStrategy("RestoreBasePropertyStrategy", std::make_unique<RestoreBasePropertyStrategy>());
        executor.AddStrategy("BackupExtraPropertyStrategy", std::make_unique<BackupExtraPropertyStrategy>());
        executor.AddStrategy("RestoreExtraPropertyStrategy", std::make_unique<RestoreExtraPropertyStrategy>());
    }
};

StrategyRegistrar &GetRegistrar()
{
    static StrategyRegistrar registrar;
    return registrar;
}
}

// DefaultPropertyStrategy: UserId + ExtensionName + ReadyLaunch
void DefaultPropertyStrategy::Execute(StrategyContext &context)
{
    auto ptr = context.session.promote();
    if (ptr == nullptr) {
        HILOGE("session pointer is null");
        return;
    }
    ptr->SetBundleUserId(context.bundleNameIndexInfo, context.userId);
    ptr->SetBackupExtName(context.bundleNameIndexInfo, context.bundleInfo.extensionName);
    ptr->SetIsReadyLaunch(context.bundleNameIndexInfo);
}

// DataSizePropertyStrategy: 区分增量/全量/恢复
void DataSizePropertyStrategy::Execute(StrategyContext &context)
{
    auto ptr = context.session.promote();
    if (ptr == nullptr) {
        HILOGE("session pointer is null");
        return;
    }
    if (context.restoreType != RestoreTypeEnum::RESTORE_DATA_WAIT_SEND) {
        if (context.isIncBackup) {
            ptr->SetBundleDataSize(context.bundleNameIndexInfo, context.bundleInfo.increSpaceOccupied);
        } else {
            ptr->SetBundleDataSize(context.bundleNameIndexInfo, context.bundleInfo.spaceOccupied);
        }
    } else {
        ptr->SetBundleDataSize(context.bundleNameIndexInfo, context.bundleInfo.spaceOccupied);
    }
}

// ClearDataFlagPropertyStrategy: 清理数据标志
void ClearDataFlagPropertyStrategy::Execute(StrategyContext &context)
{
    if (context.isClearDataFlags == nullptr) {
        return;
    }
    auto ptr = context.session.promote();
    if (ptr == nullptr) {
        HILOGE("session pointer is null");
        return;
    }
    auto iter = context.isClearDataFlags->find(context.bundleNameIndexInfo);
    if (iter != context.isClearDataFlags->end()) {
        ptr->SetClearDataFlag(context.bundleNameIndexInfo, iter->second);
    }
}

// RestoreBasePropertyStrategy: RestoreType + VersionCode + VersionName
void RestoreBasePropertyStrategy::Execute(StrategyContext &context)
{
    auto ptr = context.session.promote();
    if (ptr == nullptr) {
        HILOGE("session pointer is null");
        return;
    }
    ptr->SetBundleRestoreType(context.bundleNameIndexInfo, context.restoreType);
    ptr->SetBundleVersionCode(context.bundleNameIndexInfo, context.bundleInfo.versionCode);
    ptr->SetBundleVersionName(context.bundleNameIndexInfo, context.bundleInfo.versionName);
}

// BackupExtraPropertyStrategy: BackupScene + Unicast
void BackupExtraPropertyStrategy::Execute(StrategyContext &context)
{
    if (context.bundleNameDetailMap == nullptr) {
        return;
    }
    std::string backupScene;
    auto ptr = context.session.promote();
    if (ptr == nullptr) {
        HILOGE("session pointer is null");
        return;
    }
    if (BJsonUtil::FindBackupSceneByName(*context.bundleNameDetailMap, context.bundleNameIndexInfo, backupScene)) {
        ptr->SetBackupScene(context.bundleNameIndexInfo, backupScene);
    }
    BJsonUtil::BundleDetailInfo uniCastInfo;
    if (BJsonUtil::FindBundleInfoByName(*context.bundleNameDetailMap,
        context.bundleNameIndexInfo, UNICAST_TYPE, uniCastInfo)) {
        HILOGI("current bundle:%{public}s, unicast info:%{public}s, unicast info size:%{public}zu",
            context.bundleNameIndexInfo.c_str(), GetAnonyString(uniCastInfo.detail).c_str(), uniCastInfo.detail.size());
        ptr->SetBackupExtInfo(context.bundleNameIndexInfo, uniCastInfo.detail);
    }
}

// RestoreExtraPropertyStrategy: Broadcast + Unicast
void RestoreExtraPropertyStrategy::Execute(StrategyContext &context)
{
    if (context.bundleNameDetailMap == nullptr || context.service == nullptr) {
        return;
    }
    BJsonUtil::BundleDetailInfo broadCastInfo;
    std::map<std::string, std::string> broadCastInfoMap;
    auto ptr = context.session.promote();
    auto serviceptr = context.service.promote();
    if (ptr == nullptr || serviceptr == nullptr) {
        HILOGE("session or service pointer is null");
        return;
    }
    if (BJsonUtil::FindBroadCastInfoByName(*context.bundleNameDetailMap, context.bundleNameIndexInfo,
        BROADCAST_TYPE, broadCastInfoMap)) {
        serviceptr->SetBroadCastInfoMap(context.bundleNameIndexInfo,
            broadCastInfoMap, ptr->GetSessionUserId());
        serviceptr->BroadCastRestore(context.bundleNameIndexInfo, BConstants::BROADCAST_RESTORE_START);
    } else if (BJsonUtil::FindBundleInfoByName(*context.bundleNameDetailMap, context.bundleNameIndexInfo,
        BROADCAST_TYPE, broadCastInfo)) {
        bool notifyRet = DelayedSingleton<NotifyWorkService>::GetInstance()->NotifyBundleDetail(
            broadCastInfo, START_TYPE);
        HILOGI("Publish event end, notify result is:%{public}d", notifyRet);
    }
    BJsonUtil::BundleDetailInfo uniCastInfo;
    if (BJsonUtil::FindBundleInfoByName(*context.bundleNameDetailMap,
        context.bundleNameIndexInfo, UNICAST_TYPE, uniCastInfo)) {
        HILOGI("current bundle, unicast info:%{public}s", GetAnonyString(uniCastInfo.detail).c_str());
        ptr->SetBackupExtInfo(context.bundleNameIndexInfo, uniCastInfo.detail);
    }
}

} // namespace OHOS::FileManagement::Backup
