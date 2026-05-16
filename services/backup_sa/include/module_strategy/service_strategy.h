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

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_STRATEGY_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_STRATEGY_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <shared_mutex>

#include "b_jsonutil/b_jsonutil.h"
#include "b_json/b_json_entity_caps.h"
#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {

class Service;

// 策略上下文：封装执行策略所需的所有数据
struct StrategyContext {
    std::string bundleName;
    std::string bundleNameIndexInfo;
    BJsonEntityCaps::BundleInfo bundleInfo;
    int32_t userId = 0;
    bool isIncBackup = false;
    RestoreTypeEnum restoreType = RestoreTypeEnum::RESTORE_DATA_WAIT_SEND;
    
    // 指针可能为空，调用方需判空
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>> *bundleNameDetailMap = nullptr;
    std::map<std::string, bool> *isClearDataFlags = nullptr;
    
    sptr<SvcSessionManager> session = nullptr;
    Service *service = nullptr;
};

// --- 1. 策略模式：抽象策略类 ---
class BundlePropertyStrategy {
public:
    virtual ~BundlePropertyStrategy() = default;
    virtual void Execute(StrategyContext &context) = 0;
};

// 策略1：默认基础属性（UserId + ExtensionName + ReadyLaunch）
class DefaultPropertyStrategy : public BundlePropertyStrategy {
public:
    void Execute(StrategyContext &context) override;
};

// 策略2：数据大小设置（区分增量/全量/恢复）
class DataSizePropertyStrategy : public BundlePropertyStrategy {
public:
    void Execute(StrategyContext &context) override;
};

// 策略3：清理数据标志（detail 接口共用）
class ClearDataFlagPropertyStrategy : public BundlePropertyStrategy {
public:
    void Execute(StrategyContext &context) override;
};

// 策略4：恢复基础属性（RestoreType + VersionCode + VersionName）
class RestoreBasePropertyStrategy : public BundlePropertyStrategy {
public:
    void Execute(StrategyContext &context) override;
};

// 策略5：备份扩展属性（BackupScene + Unicast）
class BackupExtraPropertyStrategy : public BundlePropertyStrategy {
public:
    void Execute(StrategyContext &context) override;
};

// 策略6：恢复扩展属性（Broadcast + Unicast）
class RestoreExtraPropertyStrategy : public BundlePropertyStrategy {
public:
    void Execute(StrategyContext &context) override;
};

// --- 2. 策略执行器：管理策略链 ---
class PropertyStrategyExecutor {
public:
    static PropertyStrategyExecutor& GetInstance()
    {
        static PropertyStrategyExecutor instance;
        return instance;
    }

    // 线程安全的策略注册
    void AddStrategy(const std::string &name, std::unique_ptr<BundlePropertyStrategy> strategy)
    {
        std::lock_guard<std::shared_mutex> lock(mutex_);
        strategyMap_[name] = std::move(strategy);
    }
    
    // 按顺序执行指定策略
    void ExecuteStrategies(StrategyContext &context, const std::vector<std::string> &strategyNames)
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        for (const auto &name : strategyNames) {
            auto it = strategyMap_.find(name);
            if (it != strategyMap_.end()) {
                it->second->Execute(context);
            }
        }
    }

private:
    PropertyStrategyExecutor() = default;
    std::shared_mutex mutex_;
    std::map<std::string, std::unique_ptr<BundlePropertyStrategy>> strategyMap_;
};

// 自动注册辅助类
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

// 全局单例注册器，确保策略在程序启动时注册
inline StrategyRegistrar &GetStrategyRegistrar()
{
    static StrategyRegistrar registrar;
    return registrar;
}

} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_STRATEGY_H
