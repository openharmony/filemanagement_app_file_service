/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANI_UTILS_H
#define ANI_UTILS_H

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <ani.h>

namespace OHOS::FileManagement::Backup {
class AniUtils {
public:
    static std::string AniStringToStdString(ani_env* env, ani_string aniString);
    static bool AniObjectToStdString(ani_env* env, ani_object obj, std::string& str);
    static std::optional<ani_string> StdStringToAniString(ani_env* env, const std::string& str);
    static bool AniArrayToStrVector(ani_env* env, ani_array arr, std::vector<std::string>& result);
    static ani_class GetAniClsByName(ani_env* env, std::string className);
private:
    static std::unordered_map<std::string, ani_class> clsMap_;
    static std::mutex mapMutex_;
};
} // namespace OHOS::FileManagement::Backup
#endif // ANI_UTILS_H