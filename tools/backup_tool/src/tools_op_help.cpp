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

#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static string GenHelpMsg()
{
    return "\t\tThis operation helps to dump the help messages.";
}

static int Exec(map<string, vector<string>> &mapArgToVal)
{
    stringstream ss;
    auto &&allOps = ToolsOp::GetAllOperations();
    ss << "Usage: backup_tool <SubCommand> [OPTION]... [ARG]..." << endl;
    for (size_t i = 0; i < allOps.size(); ++i) {
        auto desc = allOps[i].GetDescriptor();

        // echo: <op seqs>\n
        ss << allOps[i].GetName();

        // echo: help msgs\n\n
        if (desc.funcGenHelpMsg) {
            ss << desc.funcGenHelpMsg() << endl;
        }
        if (i != allOps.size() - 1) {
            ss << endl;
        }
    }
    printf("%s", ss.str().c_str());
    return 0;
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with side
 * effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp {ToolsOp::Descriptor {
    .opName = {"help"},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
}});
} // namespace OHOS::FileManagement::Backup