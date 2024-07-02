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

/*
 * 本部件处理错误的原则：
 *     原则1：使用异常表示错误，但只有无法处理的问题才算得上是错误，否则只是普通的边界分支；
 *     原则2：仅在模块内部使用异常，而在界面层Catch所有异常，从而防止异常扩散；
 *     原则3：在注释里通过throw关键字注明可能抛出的异常，通报使用风险。
 */
#ifndef OHOS_FILEMGMT_BACKUP_B_ERROR_H
#define OHOS_FILEMGMT_BACKUP_B_ERROR_H

#include <errors.h>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#if __has_builtin(__builtin_FILE) && __has_builtin(__builtin_LINE) && __has_builtin(__builtin_FUNCTION)
#define DEFINE_SOURCE_LOCATION                                              \
    int lineNo = __builtin_LINE(), const char *fileName = __builtin_FILE(), \
        const char *functionName = __builtin_FUNCTION()
#else
#define DEFINE_SOURCE_LOCATION int lineNo = -1, const char *fileName = "NA", const char *functionName = "NA"
#endif

namespace OHOS::FileManagement::Backup {
using ErrCode = int;
using ErrParam = std::function<std::tuple<uint32_t, std::string>()>;

class BError : public std::exception {
public:
    /**
     * @brief 错误码，新增错误码时需要同步补充默认错误信息
     *
     */
    enum class Codes : ErrCode {
        // 0 无错误
        OK = 0x0,

        // 1~999 标准平台错误

        // 0x1000~0x1999 backup_utils错误
        UTILS_INVAL_JSON_ENTITY = 0x1000,
        UTILS_INVAL_FILE_HANDLE = 0x1001,
        UTILS_INVAL_TARBALL_ARG = 0x1002,
        UTILS_INVAL_PROCESS_ARG = 0x1003,
        UTILS_INTERRUPTED_PROCESS = 0x1004,

        // 0x2000~0x2999 backup_tool错误
        TOOL_INVAL_ARG = 0x2000,

        // 0x3000~0x3999 backup_sa错误
        SA_INVAL_ARG = 0x3000,
        SA_BROKEN_IPC = 0x3001,
        SA_REFUSED_ACT = 0x3002,
        SA_BROKEN_ROOT_DIR = 0x3003,
        SA_FORBID_BACKUP_RESTORE = 0x3004,
        SA_BOOT_TIMEOUT = 0x3005,
        SA_BUNDLE_INFO_EMPTY = 0x3006,

        // 0x4000~0x4999 backup_SDK错误
        SDK_INVAL_ARG = 0x4000,
        SDK_BROKEN_IPC = 0x4001,
        SDK_MIXED_SCENARIO = 0x4002,

        // 0x5000~0x5999 backup_ext错误
        EXT_INVAL_ARG = 0x5000,
        EXT_BROKEN_FRAMEWORK = 0x5001,
        EXT_BROKEN_BACKUP_SA = 0x5002,
        EXT_BROKEN_IPC = 0x5003,
        EXT_ABILITY_DIED = 0x5004,
        EXT_ABILITY_TIMEOUT = 0x5005,
        EXT_FORBID_BACKUP_RESTORE = 0x5006,
        EXT_BACKUP_PACKET_ERROR = 0x5007,
        EXT_METHOD_NOT_EXIST = 0x5008,
        EXT_THROW_EXCEPTION = 0x5009,

        // 0x6000~0x6999 sa_ext错误
        SA_EXT_ERR_CALL = 0x6000,
        SA_EXT_ERR_SAMGR = 0x6001,
        SA_EXT_RELOAD_FAIL = 0x6002,
    };

    enum BackupErrorCode {
        E_IPCSS = 13600001,
        E_PERM = 13900001,
        E_NOTEXIST = 13900002,
        E_IO = 13900005,
        E_NOMEM = 13900011,
        E_INVAL = 13900020,
        E_NOSPC = 13900025,
        E_UKERR = 13900042,
        E_FORBID = 13500001,
        E_BTO = 13500002,
        E_ETO = 13500003,
        E_DIED = 13500004,
        E_EMPTY = 13500005,
        E_PACKET = 13500006,
        E_EXCEPTION = 13500007,
    };

public:
    /**
     * @brief 返回OHOS标准错误码
     *
     * @return int 标注错误码
     */
    int GetCode() const;

    /**
     * @brief 根据系统的errno返回OHOS标准错误码
     *
     * @return int 标注错误码
     */
    static int32_t GetCodeByErrno(int32_t errnoSys);

    /**
     * @brief 返回原始错误码
     *
     * @return Codes 原始错误码
     */
    Codes GetRawCode() const
    {
        return code_;
    }

    /**
     * @brief 返回错误信息
     *
     * @return const char* 错误信息
     */
    const char *what() const noexcept override
    {
        return msg_.c_str();
    }

public:
    /**
     * @brief 重载bool操作符，判断当前错误是否是错误
     *
     * @return true 是错误
     * @return false 不是错误
     */
    explicit operator bool() const
    {
        return code_ != Codes::OK;
    }

    /**
     * @brief 返回OHOS标准错误码
     *
     * @return int 标准错误码
     */
    operator int() const
    {
        return GetCode();
    }

public:
    /**
     * @brief 构造错误对象
     *
     * @param code 备份系统标准错误码，取自本类中的Codes
     * @param lineNo 构造错误对象的行号（不要自己填写）
     * @param fileName 构造错误对象的文件（不要自己填写）
     * @param functionName 构造错误对象的函数（不要自己填写）
     */
    explicit BError(Codes code = Codes::OK, DEFINE_SOURCE_LOCATION) : code_(code)
    {
        msg_ = WrapMessageWithExtraInfos(fileName, lineNo, functionName, code_, {mpErrToMsg_.at(code_)});
    }

    /**
     * @brief 构造错误对象
     *
     * @param code 备份系统标准错误码，取自本类中的Codes
     * @param extraMsg 追加的详细错误信息
     * @param lineNo 构造错误对象的行号（不要自己填写）
     * @param fileName 构造错误对象的文件（不要自己填写）
     * @param functionName 构造错误对象的函数（不要自己填写）
     */
    BError(Codes code, const std::string_view &extraMsg, DEFINE_SOURCE_LOCATION) : code_(code)
    {
        msg_ = WrapMessageWithExtraInfos(fileName, lineNo, functionName, code_, {mpErrToMsg_.at(code_), extraMsg});
    }

    /**
     * @brief 构造错误对象
     *
     * @param stdErrno 失败的LIBC调用通过errno返回的错误码
     * @param lineNo 构造错误对象的行号（不要自己填写）
     * @param fileName 构造错误对象的文件（不要自己填写）
     * @param functionName 构造错误对象的函数（不要自己填写）
     */
    explicit BError(int stdErrno, DEFINE_SOURCE_LOCATION) : code_ {stdErrno}
    {
        std::string rawMsg = std::generic_category().message(stdErrno);
        msg_ = WrapMessageWithExtraInfos(fileName, lineNo, functionName, code_, {rawMsg});
    }

private:
    static inline const std::map<Codes, std::string_view> mpErrToMsg_ = {
        {Codes::OK, "No error"},
        {Codes::UTILS_INVAL_JSON_ENTITY, "Json utils operated on an invalid file"},
        {Codes::UTILS_INVAL_FILE_HANDLE, "File utils received an invalid file handle"},
        {Codes::UTILS_INVAL_TARBALL_ARG, "Tarball utils received an invalid argument"},
        {Codes::UTILS_INVAL_PROCESS_ARG, "Process utils received an invalid argument"},
        {Codes::UTILS_INTERRUPTED_PROCESS, "Can't launch a process or the process was corrupted"},
        {Codes::TOOL_INVAL_ARG, "TOOL received invalid arguments"},
        {Codes::SA_INVAL_ARG, "SA received invalid arguments"},
        {Codes::SA_BROKEN_IPC, "SA failed to issue a IPC"},
        {Codes::SA_REFUSED_ACT, "SA refuse to act"},
        {Codes::SA_BROKEN_ROOT_DIR, "SA failed to operate on the given root dir"},
        {Codes::SA_FORBID_BACKUP_RESTORE, "SA forbid backup or restore"},
        {Codes::SA_BOOT_TIMEOUT, "SA boot application extension time out"},
        {Codes::SA_BUNDLE_INFO_EMPTY, "SA the bundle info for backup/restore is empty"},
        {Codes::SDK_INVAL_ARG, "SDK received invalid arguments"},
        {Codes::SDK_BROKEN_IPC, "SDK failed to do IPC"},
        {Codes::SDK_MIXED_SCENARIO, "SDK involed backup/restore when doing the contrary"},
        {Codes::EXT_INVAL_ARG, "Extension received an invalid argument"},
        {Codes::EXT_BROKEN_FRAMEWORK, "Extension found the appex framework is broken"},
        {Codes::EXT_BROKEN_BACKUP_SA, "Extension found the backup SA died"},
        {Codes::EXT_BROKEN_IPC, "Extension failed to do IPC"},
        {Codes::EXT_ABILITY_TIMEOUT, "Extension process timeout"},
        {Codes::EXT_ABILITY_DIED, "Extension process died"},
        {Codes::EXT_FORBID_BACKUP_RESTORE, "forbid backup or restore"},
        {Codes::EXT_BACKUP_PACKET_ERROR, "Backup packet error"},
        {Codes::EXT_THROW_EXCEPTION, "Extension throw exception"},
    };

    static inline const std::map<int, int> errCodeTable_ {
        {static_cast<int>(Codes::OK), static_cast<int>(Codes::OK)},
        {static_cast<int>(Codes::UTILS_INVAL_JSON_ENTITY), BackupErrorCode::E_INVAL},
        {static_cast<int>(Codes::UTILS_INVAL_FILE_HANDLE), BackupErrorCode::E_INVAL},
        {static_cast<int>(Codes::UTILS_INVAL_TARBALL_ARG), BackupErrorCode::E_UKERR},
        {static_cast<int>(Codes::UTILS_INVAL_PROCESS_ARG), BackupErrorCode::E_UKERR},
        {static_cast<int>(Codes::UTILS_INTERRUPTED_PROCESS), BackupErrorCode::E_UKERR},
        {static_cast<int>(Codes::TOOL_INVAL_ARG), BackupErrorCode::E_UKERR},
        {static_cast<int>(Codes::SA_INVAL_ARG), BackupErrorCode::E_INVAL},
        {static_cast<int>(Codes::SA_BROKEN_IPC), BackupErrorCode::E_IPCSS},
        {static_cast<int>(Codes::SA_REFUSED_ACT), BackupErrorCode::E_PERM},
        {static_cast<int>(Codes::SA_BROKEN_ROOT_DIR), BackupErrorCode::E_UKERR},
        {static_cast<int>(Codes::SA_FORBID_BACKUP_RESTORE), BackupErrorCode::E_FORBID},
        {static_cast<int>(Codes::SA_BOOT_TIMEOUT), BackupErrorCode::E_BTO},
        {static_cast<int>(Codes::SA_BUNDLE_INFO_EMPTY), BackupErrorCode::E_EMPTY},
        {static_cast<int>(Codes::SDK_INVAL_ARG), BackupErrorCode::E_INVAL},
        {static_cast<int>(Codes::SDK_BROKEN_IPC), BackupErrorCode::E_IPCSS},
        {static_cast<int>(Codes::SDK_MIXED_SCENARIO), BackupErrorCode::E_INVAL},
        {static_cast<int>(Codes::EXT_INVAL_ARG), BackupErrorCode::E_INVAL},
        {static_cast<int>(Codes::EXT_BROKEN_FRAMEWORK), BackupErrorCode::E_UKERR},
        {static_cast<int>(Codes::EXT_BROKEN_BACKUP_SA), BackupErrorCode::E_IPCSS},
        {static_cast<int>(Codes::EXT_BROKEN_IPC), BackupErrorCode::E_IPCSS},
        {static_cast<int>(Codes::EXT_ABILITY_DIED), BackupErrorCode::E_DIED},
        {static_cast<int>(Codes::EXT_ABILITY_TIMEOUT), BackupErrorCode::E_ETO},
        {static_cast<int>(Codes::EXT_FORBID_BACKUP_RESTORE), BackupErrorCode::E_FORBID},
        {static_cast<int>(Codes::EXT_BACKUP_PACKET_ERROR), BackupErrorCode::E_PACKET},
        {static_cast<int>(Codes::EXT_THROW_EXCEPTION), BackupErrorCode::E_EXCEPTION},
        {BackupErrorCode::E_IPCSS, BackupErrorCode::E_IPCSS},
        {BackupErrorCode::E_INVAL, BackupErrorCode::E_INVAL},
        {BackupErrorCode::E_NOTEXIST, BackupErrorCode::E_NOTEXIST},
        {BackupErrorCode::E_UKERR, BackupErrorCode::E_UKERR},
        {BackupErrorCode::E_PERM, BackupErrorCode::E_PERM},
        {BackupErrorCode::E_NOMEM, BackupErrorCode::E_NOMEM},
        {BackupErrorCode::E_NOSPC, BackupErrorCode::E_NOSPC},
        {BackupErrorCode::E_IO, BackupErrorCode::E_IO},
        {BackupErrorCode::E_FORBID, BackupErrorCode::E_FORBID},
        {BackupErrorCode::E_BTO, BackupErrorCode::E_BTO},
        {BackupErrorCode::E_ETO, BackupErrorCode::E_ETO},
        {BackupErrorCode::E_DIED, BackupErrorCode::E_DIED},
        {BackupErrorCode::E_EMPTY, BackupErrorCode::E_EMPTY},
        {BackupErrorCode::E_PACKET, BackupErrorCode::E_PACKET},
        {BackupErrorCode::E_EXCEPTION, BackupErrorCode::E_EXCEPTION},
    };

    static inline const std::map<int, int> sysErrnoCodeTable_ {
        {EPERM, BackupErrorCode::E_IPCSS},
        {EIO, BackupErrorCode::E_IO},
        {EBADF, BackupErrorCode::E_IO},
        {EACCES, BackupErrorCode::E_IO},
        {EFBIG, BackupErrorCode::E_IO},
        {ENOMEM, BackupErrorCode::E_NOMEM},
        {EMFILE, BackupErrorCode::E_NOMEM},
        {ENOENT, BackupErrorCode::E_INVAL},
        {ENOTDIR, BackupErrorCode::E_INVAL},
        {EISDIR, BackupErrorCode::E_INVAL},
        {ENAMETOOLONG, BackupErrorCode::E_INVAL},
        {ENOSPC, BackupErrorCode::E_NOSPC},
    };

private:
    Codes code_ {Codes::OK};
    std::string msg_;

private:
    /**
     * @brief 生成如下格式的打印信息 → [文件名:行号->函数名] 默认错误信息. 补充错误信息
     *
     * @param fileName 构造错误对象的文件
     * @param lineNo 构造错误对象的行号
     * @param functionName 构造错误对象的函数
     * @param msgs 所有待追加的错误信息
     * @return std::string 打印信息
     */
    std::string WrapMessageWithExtraInfos(const char *fileName,
                                          int lineNo,
                                          const char *functionName,
                                          Codes code,
                                          const std::vector<std::string_view> &msgs) const;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_ERROR_H