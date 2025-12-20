/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_DLFCN_MOCK_H
#define OHOS_FILEMGMT_BACKUP_DLFCN_MOCK_H

#include <dlfcn.h>
#include <gmock/gmock.h>

namespace OHOS::FileManagement::Backup {
class Dlfcn {
public:
    virtual ~Dlfcn() = default;

    virtual void *dlopen(const char *file, int mode) = 0;
    virtual void *dlsym(void *handle, const char *symbol) = 0;

public:
    static inline std::shared_ptr<Dlfcn> dlFunc_ = nullptr;
};

class DlfcnMock : public Dlfcn {
public:
    MOCK_METHOD(void *, dlopen, (const char *file, int mode), (override));
    MOCK_METHOD(void *, dlsym, (void *handle, const char *symbol), (override));
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_DLFCN_MOCK_H