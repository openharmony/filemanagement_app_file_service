/*
* Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "library_func_mock.h"

using namespace OHOS::AppFileService;
int fseeko(FILE *stream, off_t offset, int whence)
{
    if (LibraryFunc::libraryFunc_ == nullptr) {
        return -1;
    }
    return LibraryFunc::libraryFunc_->fseeko(stream, offset, whence);
}

off_t ftello(FILE *stream)
{
    if (LibraryFunc::libraryFunc_ == nullptr) {
        return -1;
    }
    return LibraryFunc::libraryFunc_->ftello(stream);
}

int access(const char *pathname, int mode)
{
    if (LibraryFunc::libraryFunc_ == nullptr) {
        return -1;
    }
    return LibraryFunc::libraryFunc_->access(pathname, mode);
}

int mkdir(const char *pathname, mode_t mode)
{
    if (LibraryFunc::libraryFunc_ == nullptr) {
        return -1;
    }
    return LibraryFunc::libraryFunc_->mkdir(pathname, mode);
}