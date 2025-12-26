/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#ifndef INTERFACES_KITS_TAIHE_SRC_MOD_BACKUP_PROPERTIES_GENERAL_CALLBACKS_H
#define INTERFACES_KITS_TAIHE_SRC_MOD_BACKUP_PROPERTIES_GENERAL_CALLBACKS_H

#include <memory>
#include <node_api.h>
#include <string>
#include "backup_file_info.h"
#include "b_error/b_error.h"
#include "filemgmt_libn.h"
#include "taihe/runtime.hpp"
#include "unique_fd.h"

using  BFileInfo = OHOS::FileManagement::Backup::BFileInfo;

namespace OHOS::FileManagement::Backup::TAIHE {
class TaiheGeneralCallbacks {
public:
    TaiheGeneralCallbacks(ani_ref* ref, ani_vm* vm):ref_(ref), vm_(vm) {};
    ~TaiheGeneralCallbacks() {};
    void onFileReady(const BFileInfo &, UniqueFd, ErrCode);  // 当备份服务有文件待发送时执行的回调
    void onBundleStarted(ErrCode, const BundleName);  // 当启动某个应用的备份流程结束时执行的回调函数
    void onBundleFinished(ErrCode, const BundleName); // 当某个应用的备份流程结束或意外中止时执行的回调函数
    void onAllBundlesFinished(ErrCode);           // 当整个备份流程结束或意外中止时执行的回调函数
    void onResultReport(const std::string bundleName, const std::string result); // 某个应用备份流程中自定义错误信息的上报的回调函数
    void onBackupServiceDied();                       // 当备份服务意外死亡时执行的回调函数
    void onProcess(const std::string bundleName, const std::string process); // 上报备份恢复过程中的进度和异常
    void onBackupSizeReport(const std::string); // 返回已获取待备份数据量的信息
    ani_ref* getRef(){return ref_;}
private:
    ani_ref* ref_ = nullptr;
    ani_vm* vm_ = nullptr;
};
} // namespace OHOS::FileManagement::Backup
#endif // INTERFACES_KITS_JS_SRC_MOD_BACKUP_PROPERTIES_GENERAL_CALLBACKS_H