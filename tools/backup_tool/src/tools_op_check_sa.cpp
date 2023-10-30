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

#include <iostream>
#include <sstream>

#include "errors.h"
#include "service_proxy.h"
#include "tools_op.h"
#include "tools_op_check_sa.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static string GenHelpMsg()
{
    return "\tThis operation helps to check if the backup sa is available.";
}

static int Exec(map<string, vector<string>> &mapArgToVal)
{
    auto proxy = ServiceProxy::GetInstance();
    if (!proxy) {
        fprintf(stderr, "Get an empty backup sa proxy\n");
        return -EFAULT;
    }

    printf("successful\n");
    return 0;
}


bool CheckSaRegister()
{
    return ToolsOp::Register(ToolsOp{ ToolsOp::Descriptor {
        .opName = {"check", "sa"},
        .funcGenHelpMsg = GenHelpMsg,
        .funcExec = Exec,
    } });
}
} // namespace OHOS::FileManagement::Backup