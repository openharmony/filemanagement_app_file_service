/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
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

#ifndef OHOS_FILEMGMT_BACKUP_COMPRESS_STRATEGY_MOCK_H
#define OHOS_FILEMGMT_BACKUP_COMPRESS_STRATEGY_MOCK_H

#include <gmock/gmock.h>
#include "tar_file.h"

namespace OHOS::FileManagement::Backup {
class CompressMock : public ICompressStrategy {
public:
    MOCK_METHOD(bool, CompressBuffer, (Buffer& input, Buffer& output));
    MOCK_METHOD(bool, DecompressBuffer, (Buffer& compressed, Buffer& origin));
    MOCK_METHOD(std::string, GetFileSuffix, ());
    MOCK_METHOD(size_t, GetMaxCompressedSizeInner, (size_t inputSize));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_COMPRESS_STRATEGY_MOCK_H
