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
#ifndef OHOS_FILEMGMT_BACKUP_RUNNINGLOCK_MOCK_H
#define OHOS_FILEMGMT_BACKUP_RUNNINGLOCK_MOCK_H

#include <gmock/gmock.h>
#include "power_mgr_client.h"
#include "running_lock.h"

namespace OHOS::PowerMgr {
class PPowerMgrClient {
public:
    virtual ~PPowerMgrClient() = default;
    virtual std::shared_ptr<RunningLock> CreateRunningLock(const std::string &name, RunningLockType type) = 0;

    static inline std::shared_ptr<PPowerMgrClient> powerMgrClient_ = nullptr;
};

class PowerMgrClientMock : public PPowerMgrClient {
public:
    MOCK_METHOD(std::shared_ptr<RunningLock>, CreateRunningLock,
        (const std::string &name, RunningLockType type));
};

class RRunningLock {
public:
    virtual ~RRunningLock() = default;
    virtual ErrCode Lock(int32_t timesOutMs) = 0;
    virtual ErrCode UnLock() = 0;

    static inline std::shared_ptr<RRunningLockj> runninglock_ = nullptr;
};

class RunningLockMock : public RRunningLock {
public:
    MOCK_METHOD(ErrCode, Lock, (int32_t timeOurMs));
    MOCK_METHOD(ErrCode, UnLock, ());
};
};

#endif