/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "module_notify/notify_work_service.h"

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "filemgmt_libhilog.h"
#include "int_wrapper.h"
#include "string_wrapper.h"
#include "want.h"
#include "want_params.h"

namespace OHOS::FileManagement::Backup {
namespace {
    const static std::string EVENT_NAME = "COMMON_EVENT_RESTORE_START";
}
NotifyWorkService::NotifyWorkService() {}
NotifyWorkService::~NotifyWorkService() {}

bool NotifyWorkService::NotifyBundleDetail(BJsonUtil::BundleDetailInfo bundleDetailInfo)
{
    AAFwk::Want want;
    std::string bundleName = bundleDetailInfo.bundleName;
    std::string bundleDetail = bundleDetailInfo.detail;
    HILOGI("Start publish event, bundleName is: %{public}s, detail:%{public}s", bundleName.c_str(),
        bundleDetail.c_str());
    want.SetBundle(bundleName);
    want.SetParam("bundleName", bundleName);
    want.SetParam("userId", bundleDetailInfo.userId);
    want.SetParam("index", bundleDetailInfo.bundleIndex);
    want.SetParam("detail", bundleDetail);
    want.SetAction(EVENT_NAME);
    EventFwk::CommonEventData commonData {want};
    HILOGI("End publish event, bundleName is: %{public}s", bundleName.c_str());
    return EventFwk::CommonEventManager::PublishCommonEvent(commonData);
}
}