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

#include "notify_work_service.h"

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
    HILOGI("Start publish event, bundleName is: %{public}s", bundleName.c_str());
    want.SetParam("bundleName", bundleName);
    want.SetParam("type", bundleDetailInfo.type);
    want.SetParam("index", bundleDetailInfo.bundleIndex);
    want.SetParam("detail", bundleDetailInfo.detail);
    want.SetAction(EVENT_NAME);
    EventFwk::CommonEventData commonData {want};
    bool publishRet = EventFwk::CommonEventManager::PublishCommonEvent(commonData);
    HILOGI("End publish event, bundleName is: %{public}s", bundleName.c_str());
    return publishRet;
}
}