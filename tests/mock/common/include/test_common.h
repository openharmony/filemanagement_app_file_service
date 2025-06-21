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

#ifndef OHOS_FILEMGMT_TEST_COMMON_H
#define OHOS_FILEMGMT_TEST_COMMON_H

#include <string>
#include <thread>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "action_define.h"

namespace OHOS::FileManagement::Backup {

#define SLEEP(second) std::this_thread::sleep_for(std::chrono::seconds(second))

const std::string EMPTY_BUNDLE_NAME = "";
const std::string BUNDLE_NAME = "com.example.app2backup";
const std::string SA_BUNDLE_NAME = "1234";
const std::string BUNDLE_NAME_FALSE = "com.example.app2backup.false";
constexpr uint32_t CLEARED_CLIENT_TOKEN = 99;
constexpr int START_RETURN_FALSE = 100;
const std::string MANAGE_JSON = "manage.json";
const std::string FILE_NAME = "1.tar";
constexpr int32_t SERVICE_ID = 5203;

} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_TEST_COMMON_H