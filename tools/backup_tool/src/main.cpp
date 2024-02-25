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

#ifndef BACKUP_TOOL_ENABLE
int main()
{
    return 0;
}
#else

#include "errors.h"
#include "tools_op.h"
#include "tools_op_backup.h"
#include "tools_op_check_sa.h"
#include "tools_op_help.h"
#include "tools_op_restore.h"
#include "tools_op_restore_async.h"
#include "tools_op_incremental_backup.h"
#include "tools_op_incremental_restore.h"
#include "tools_op_incremental_restore_async.h"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <sstream>

namespace OHOS::FileManagement::Backup {
using namespace std;

optional<map<string, vector<string>>> GetArgsMap(int argc, char *const argv[], const vector<ToolsOp::CmdInfo> &argList)
{
    int i = 0;
    map<int, string> mapOptToName;
    vector<struct option> vecLongOptions;
    for (auto &&arg : argList) {
        mapOptToName[i] = arg.paramName;
        vecLongOptions.emplace_back(option {
            .name = arg.paramName.c_str(),
            .has_arg = required_argument,
            .flag = nullptr,
            .val = i++,
        });
    }
    vecLongOptions.emplace_back(option {nullptr, 0, nullptr, 0});

    int opt = 0;
    int options_index = 0;
    map<string, vector<string>> mapArgToVals;
    while ((opt = getopt_long(argc, argv, "", vecLongOptions.data(), &options_index)) != -1) {
        if (opt == '?') {
            // "我们匹配到了一个奇怪的命令 返回 nullopt,getopt_long 在opterr 未被赋值0时 会自动打印未被定义参数到终端"
            return nullopt;
        }
        string argName = mapOptToName[opt];
        if (mapArgToVals.find(argName) != mapArgToVals.end() && argList[opt].repeatable == true) {
            mapArgToVals[argName].emplace_back(optarg);
        } else if (mapArgToVals.find(argName) != mapArgToVals.end()) {
            fprintf(stderr, "%s can only be entered once, but you repeat it.\n", argName.c_str());
            return nullopt;
        } else {
            mapArgToVals.emplace(argName, vector<string> {optarg});
        }
    }
    return mapArgToVals;
}

void ToolRegister()
{
    OHOS::FileManagement::Backup::BackUpRegister();
    OHOS::FileManagement::Backup::HelpRegister();
    OHOS::FileManagement::Backup::CheckSaRegister();
    OHOS::FileManagement::Backup::RestoreRegister();
    OHOS::FileManagement::Backup::RestoreAsyncRegister();
    OHOS::FileManagement::Backup::IncrementalBackUpRegister();
    OHOS::FileManagement::Backup::IncrementalRestoreRegister();
    OHOS::FileManagement::Backup::IncrementalRestoreAsyncRegister();
}

int ParseOpAndExecute(const int argc, char *const argv[])
{
    // 注册下命令
    ToolRegister();
    int flag = -1;
    for (int i = 1; i < argc; i++) {
        // 暂存 {argv[1]...argv[i]};
        vector<string_view> curOp;
        for (int j = 1; j <= i; ++j) {
            curOp.emplace_back(argv[j]);
        }

        // 尝试匹配当前命令，成功后执行
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            vector<ToolsOp::CmdInfo> argList = matchedOp->GetParams();
            optional<map<string, vector<string>>> mapNameToArgs = GetArgsMap(argc, argv, argList);
            if (mapNameToArgs.has_value()) {
                flag = matchedOp->Execute(mapNameToArgs.value());
            }
        }
    }
    if (flag != 0) {
        printf("backup_tool: missing operand\nTry 'backup_tool help' for more information.\n");
    }
    return flag;
}
} // namespace OHOS::FileManagement::Backup

int main(int argc, char *const argv[])
{
    return OHOS::FileManagement::Backup::ParseOpAndExecute(argc, argv);
}

#endif