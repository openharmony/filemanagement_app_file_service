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

#ifndef OHOS_FILEMGMT_BACKUP_INNERRECEIVER_H
#define OHOS_FILEMGMT_BACKUP_INNERRECEIVER_H

#include <string>

#include <refbase.h>

#include "status_receiver_host.h"

namespace OHOS::FileManagement::Backup {
class SchedScheduler;

class InnerReceiverImpl : public AppExecFwk::StatusReceiverHost {
public:
    virtual void OnStatusNotify(const int progress) override;
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) override;

    std::string GetBundleName();

public:
    InnerReceiverImpl(const std::string &bundleName, wptr<SchedScheduler> sched)
        : bundleName_(bundleName), sched_(sched) {};

private:
    std::string bundleName_;
    wptr<SchedScheduler> sched_;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_INNERRECEIVER_H