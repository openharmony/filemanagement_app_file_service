/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_BSTRING_RAW_DATA_H
#define OHOS_FILEMGMT_BACKUP_BSTRING_RAW_DATA_H

#include <errno.h>
#include <sstream>

namespace OHOS::FileManagement::Backup {

constexpr size_t MAX_IPC_STRING_SIZE = 16 * 1024 * 1024; // 16M
struct BStringRawData {
    uint32_t size = 0;
    const void *data = nullptr;
    std::string serializedData;

    int32_t Marshalling(const std::string &in)
    {
        if (in.length() > MAX_IPC_STRING_SIZE) {
            return EINVAL;
        }
        std::stringstream ss;
        uint32_t length = in.length();
        if (!ss.write(reinterpret_cast<const char *>(&length), sizeof(length)).good()) {
            return EINVAL;
        }
        if (!ss.write(in.c_str(), length).good()) {
            return EINVAL;
        }
        serializedData = ss.str();
        data = reinterpret_cast<const void *>(serializedData.data());
        size = serializedData.length();
        return 0;
    }

    int32_t Unmarshalling(std::string &out) const
    {
        std::stringstream ss;
        ss.write(reinterpret_cast<const char *>(data), size);
        uint32_t length = 0;
        ss.read(reinterpret_cast<char *>(&length), sizeof(length));
        if (ss.gcount() != sizeof(uint32_t)) {
            return EINVAL;
        }
        if (length <= 0 || length > size || length > MAX_IPC_STRING_SIZE) {
            return EINVAL;
        }
        out.resize(length);
        ss.read(&out[0], length);
        if (ss.gcount() != length) {
            return EINVAL;
        }
        return 0;
    }

    int32_t RawDataCpy(const void *readdata)
    {
        if (readdata == nullptr || size == 0) {
            return EINVAL;
        }
        std::stringstream ss;
        ss.write(reinterpret_cast<const char *>(readdata), size);
        serializedData = ss.str();
        data = reinterpret_cast<const void *>(serializedData.data());
        return 0;
    }
};
} // namespace OHOS::FileManagement::Backup

#endif
