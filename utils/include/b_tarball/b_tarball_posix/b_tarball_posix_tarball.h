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

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_TARBALL_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_TARBALL_H

#include <string>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixTarball {
public:
    void Emplace(const std::string pathName);
    void Publish();
    void Clear();

public:
    BTarballPosixTarball() = default;
    explicit BTarballPosixTarball(const std::string pathTarball);
    ~BTarballPosixTarball() = default;

private:
    UniqueFd fileTarball_ {-1};
    bool isPublished_ {false};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_TARBALL_H