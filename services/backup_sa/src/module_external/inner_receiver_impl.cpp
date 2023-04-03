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

#include "module_external/inner_receiver_impl.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/svc_session_manager.h"
#include "module_sched/sched_scheduler.h"
#include "module_ipc/service.h"

namespace OHOS::FileManagement::Backup {
void InnerReceiverImpl::OnStatusNotify(const int progress)
{
    HILOGI("OnStatusNotify %{public}d", progress);
}
void InnerReceiverImpl::OnFinished(const int32_t resultCode, const std::string &resultMsg)
{
    HILOGI("Finished to install %{public}s, resultCode = %{public}d resultMsg = %{public}s", bundleName_.c_str(),
           resultCode, resultMsg.c_str());

    auto sched = sched_.promote();
    if (sched == nullptr) {
        throw BError(BError::Codes::SA_INVAL_ARG, std::string("Failed to promote bundleContext"));
    }
    sched->InstallSuccess(bundleName_, resultCode);
}
std::string InnerReceiverImpl::GetBundleName()
{
    return bundleName_;
}
} // namespace OHOS::FileManagement::Backup