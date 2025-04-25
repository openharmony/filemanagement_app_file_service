/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef EXT_ANI_ERROR_H
#define EXT_ANI_ERROR_H

#include <ani.h>
#include <string>

namespace OHOS {

class AniError {
public:
    enum errorCode : int { EXT_BROKEN_FRAMEWORK = 0x5001 };

public:
    AniError() = default;

    static void ThrowBusinessError(ani_env *env, errorCode errCode, const std::string &errMsg);

    template <class T>
    static void Assert(const T &t, ani_env *env, errorCode code, const std::string &errMsg)
    {
        if (!t) {
            AniError::ThrowBusinessError(env, code, errMsg);
        }
    }
}; // AniError

} // namespace OHOS
#endif // EXT_ANI_ERROR_H
