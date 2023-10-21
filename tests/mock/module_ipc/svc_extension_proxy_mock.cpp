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

#include "module_ipc/svc_extension_proxy.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

UniqueFd SvcExtensionProxy::GetFileHandle(const string &fileName)
{
    return UniqueFd(-1);
}

ErrCode SvcExtensionProxy::HandleClear()
{
    return 0;
}

ErrCode SvcExtensionProxy::HandleBackup()
{
    return 0;
}

ErrCode SvcExtensionProxy::PublishFile(const string &fileName)
{
    return 0;
}

ErrCode SvcExtensionProxy::HandleRestore()
{
    return 0;
}
} // namespace OHOS::FileManagement::Backup