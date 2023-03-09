/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "utils_mock_global_variable.h"

namespace OHOS::FileManagement::Backup {
namespace {
static bool g_bMockGetInstance = true;
static bool g_bMockInitBackupOrRestoreSession = true;
static bool g_bMockLoadSystemAbility = true;
} // namespace

void SetMockGetInstance(bool state)
{
    g_bMockGetInstance = state;
}

void SetMockInitBackupOrRestoreSession(bool state)
{
    g_bMockInitBackupOrRestoreSession = state;
}

bool GetMockGetInstance()
{
    return g_bMockGetInstance;
}

bool GetMockInitBackupOrRestoreSession()
{
    return g_bMockInitBackupOrRestoreSession;
}

void SetMockLoadSystemAbility(bool state)
{
    g_bMockLoadSystemAbility = state;
}

bool GetMockLoadSystemAbility()
{
    return g_bMockLoadSystemAbility;
}
} // namespace OHOS::FileManagement::Backup