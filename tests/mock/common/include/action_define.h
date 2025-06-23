/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License") = 0;
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

#ifndef OHOS_FILEMGMT_ACTION_DEFINE_H
#define OHOS_FILEMGMT_ACTION_DEFINE_H

#include <gmock/gmock.h>

namespace OHOS::FileManagement::Backup {

ACTION_P(ActionUpdateVal, key, val)
{
    *key = val;
    GTEST_LOG_(INFO) << "in ActionUpdateVal";
}

ACTION_P(ActionUpdateRetVal, key, val)
{
    *key = val;
    GTEST_LOG_(INFO) << "in ActionUpdateRetVal";
    return val;
}

} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_ACTION_DEFINE_H