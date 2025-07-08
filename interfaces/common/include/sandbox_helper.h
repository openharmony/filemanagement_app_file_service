/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_INNERKITS_NATIVE_COMMON_INCLUDE_SANDBOX_HELPER_H
#define FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_INNERKITS_NATIVE_COMMON_INCLUDE_SANDBOX_HELPER_H

#include <mutex>
#include <string>
#include <unordered_map>

namespace OHOS {
namespace AppFileService {
class SandboxHelper {
private:
    static std::mutex mapMutex_;
    static std::unordered_map<std::string, std::string> sandboxPathMap_;
    static std::unordered_map<std::string, std::string> backupSandboxPathMap_;
    static bool GetSandboxPathMap();
    static bool GetBackupSandboxPathMap();
    static void* libMediaHandle_;
public:
    static std::string Encode(const std::string &uri);
    static std::string Decode(const std::string &uri);
    static bool CheckValidPath(const std::string &filePath);
    static int32_t GetMediaSharePath(const std::vector<std::string> &fileUris, std::vector<std::string> &physicalPaths);
    static int32_t GetPhysicalPath(const std::string &fileUri, const std::string &userId,
                                   std::string &physicalPath);
    static int32_t GetPhysicalDir(const std::string &fileUri, const std::string &userId, std::string &physicalDir);
    static int32_t GetBackupPhysicalPath(const std::string &fileUri, const std::string &userId,
                                         std::string &physicalPath);
    static bool IsValidPath(const std::string &path);
    static void GetNetworkIdFromUri(const std::string &fileUri, std::string &networkId);
    static std::string GetLowerDir(std::string &lowerPathHead, const std::string &userId, const std::string &bundleName,
                                   const std::string &networkId);
    static void ClearBackupSandboxPathMap();
};
} // namespace AppFileService
} // namespace OHOS

#endif // FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_INNERKITS_NATIVE_COMMON_INCLUDE_SANDBOX_HELPER_H