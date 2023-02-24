/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_TOOLS_OP_H
#define OHOS_FILEMGMT_BACKUP_TOOLS_OP_H

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace OHOS::FileManagement::Backup {
class ToolsOp {
public:
    using CRefVStrView = const std::vector<std::string_view> &;
    struct CmdInfo {
        std::string paramName;
        bool repeatable = false;
    };

    struct Descriptor {
        // 命令名，必填
        std::vector<std::string_view> opName;
        // 参数，选填
        std::vector<CmdInfo> argList;
        // 命令帮助语句，选填
        std::function<std::string()> funcGenHelpMsg;
        // 命令执行主体，必填
        std::function<int(std::map<std::string, std::vector<std::string>> &args)> funcExec;
    };

    /**
     * @brief 构造一个操作
     *
     * @param desc 操作具体信息
     */
    explicit ToolsOp(Descriptor &&desc) : desc_(std::move(desc)) {}

    /**
     * @brief 获取当前操作的名称。操作由多条字符串构成时，之间由空格隔开
     *
     * @return const std::string 当前操作的名称
     */
    const std::string GetName() const;

    /**
     * @brief 获取当前操作的参数
     *
     * @return std::vector<CmdInfo> 当前参数的向量
     */
    const std::vector<CmdInfo> GetParams() const
    {
        return desc_.argList;
    }

    /**
     * @brief 获取当前操作的原始具体信息
     *
     * @return const Descriptor& 当前操作的原始具体信息
     */
    const Descriptor &GetDescriptor() const
    {
        return desc_;
    }

    /**
     * @brief 获取所有操作
     *
     * @return const std::vector<ToolsOp>& 所有操作
     */
    static const std::vector<ToolsOp> &GetAllOperations()
    {
        return ToolsOp::opsAvailable_;
    }

    /**
     * @brief 注册一个操作
     *
     * @param op 操作
     * @return true 注册成功
     * @return false 注册失败
     */
    static bool Register(ToolsOp &&op);

    /**
     * @brief 将当前操作与主函数给定的操作相匹配（大小写敏感）
     *
     * @param op 给定操作
     * @return true 匹配成功
     * @return false 匹配失败
     */
    bool TryMatch(CRefVStrView op) const;

    /**
     * @brief 使用主函数给定的参数表执行当前操作
     *
     * @param args 给定参数表
     * @return int 错误码（0 表示成功，非零表示失败）
     */
    int Execute(std::map<std::string, std::vector<std::string>> mapArg) const;

private:
    Descriptor desc_;
    static inline std::vector<ToolsOp> opsAvailable_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_TOOLS_OP_H