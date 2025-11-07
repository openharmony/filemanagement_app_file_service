/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "runninglock_mock.h"

namespace OHOS::PowerMgr {
    
std::shared_ptr<RunningLock> PowerMgrClient::CreateRunningLock(const std::string &name, RunningLockType type)
{
    return PPowerMgrClient::powerMgrClient_->CreateRunningLock(name, type);
}

Errcode RunningLock::Lock(int32_t timeOutMs)
{
    return RRunningLock::runninglock_->lock(timeOutMs);
}

Errcode RunningLock::UnLock()
{
    return RRunningLock::runninglock_->Unlock();
}
};