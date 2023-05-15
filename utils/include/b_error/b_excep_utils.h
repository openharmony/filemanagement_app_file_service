/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_EXCEP_UTILES_H
#define OHOS_FILEMGMT_BACKUP_B_EXCEP_UTILES_H

#include <exception>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
class BExcepUltils {
public:
    /**
     * @brief 异常捕获
     *
     * @param callBack 回调
     * @return ErrCode 错误码
     */
    static ErrCode ExceptionCatcherLocked(std::function<ErrCode(void)> callBack)
    {
        try {
            return callBack();
        } catch (const BError &e) {
            return e.GetCode();
        } catch (const std::exception &e) {
            HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
            return EPERM;
        } catch (...) {
            HILOGE("Unexpected exception");
            return EPERM;
        }
    }

    /**
     * @brief 检查 AbilityInfo 是否有效
     *
     * @param AbilityInfo
     * @param code 错误码
     * @param msg 错误信息
     * @return 无
     */
    template <class T>
    static void BAssert(const T &t, const BError::Codes &code, const std::string_view msg = "")
    {
        if (!t) {
            if (msg.empty()) {
                throw BError(code);
            } else {
                throw BError(code, msg);
            }
        }
    }

    /**
     * @brief 校验路径
     *
     * @param path
     * @param isExtension
     */
    static void VerifyPath(const std::string_view &path, bool isExtension = false);
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_B_EXCEP_UTILES_H