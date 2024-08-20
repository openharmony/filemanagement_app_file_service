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

#ifndef FILEMANAGEMENT_APP_FILE_SERVICE_LIBRARY_FUNC_MOCK_H
#define FILEMANAGEMENT_APP_FILE_SERVICE_LIBRARY_FUNC_MOCK_H

#include <gmock/gmock.h>

#include <fcntl.h>
#include <memory>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace OHOS {
namespace AppFileService {
class LibraryFunc {
public:
    virtual ~LibraryFunc() = default;
    virtual int fseeko(FILE *stream, off_t offset, int whence) = 0;
    virtual off_t ftello(FILE *stream) = 0;
    virtual int access(const char *pathname, int mode) = 0;
    virtual int mkdir(const char *pathname, mode_t mode) = 0;
public:
    static inline std::shared_ptr<LibraryFunc> libraryFunc_ = nullptr;
};

class LibraryFuncMock : public LibraryFunc {
public:
    MOCK_METHOD3(fseeko, int(FILE *stream, off_t offset, int whence));
    MOCK_METHOD1(ftello, off_t(FILE *stream));
    MOCK_METHOD2(access, int(const char *pathname, int mode));
    MOCK_METHOD2(mkdir, int(const char *pathname, mode_t mode));
};
} // namespace AppFileService
} // namespace OHOS

#endif // FILEMANAGEMENT_APP_FILE_SERVICE_LIBRARY_FUNC_MOCK_H
