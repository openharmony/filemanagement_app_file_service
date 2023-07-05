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

#ifndef APP_FILE_SERVICE_COMMON_FUNC
#define APP_FILE_SERVICE_COMMON_FUNC

#include <string>
#include <unordered_map>

namespace OHOS {
namespace AppFileService {
class CommonFunc {
    static std::unordered_map<std::string, std::string> sandboxPathMap_;
public:
    static bool CheckValidPath(const std::string &filePath);
    static int32_t GetPhysicalPath(const std::string &fileUri, const std::string &userId,
                                   std::string &physicalPath);
};
} // namespace AppFileService
} // namespace OHOS

#endif // APP_FILE_SERVICE_COMMON_FUNC