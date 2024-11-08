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

#include <iostream>

#include "directory_ex.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

TestManager::TestManager(std::string functionName, MakeDirType dirType)
{
    switch (dirType) {
        case MakeDirType::CURRENTUSER :
            rootDirCurTest_ = PATH_CURRENT_USER;
            break;
        default :
            rootDirCurTest_ = "/data/test/backup/" + functionName + "/";
    }
    //  REM：先删后创建
    if (bool created = ForceCreateDirectory(rootDirCurTest_); !created) {
        throw std::system_error(errno, std::system_category());
    }
}

TestManager::~TestManager()
{
    ForceRemoveDirectory(rootDirCurTest_);
}

string TestManager::GetRootDirCurTest() const
{
    return rootDirCurTest_;
}
} // namespace OHOS::FileManagement::Backup