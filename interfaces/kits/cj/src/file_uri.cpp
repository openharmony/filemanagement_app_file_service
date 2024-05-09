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

#include "file_uri.h"

#include <unistd.h>

#include "uri.h"

#include "common_func.h"
#include "log.h"
#include "sandbox_helper.h"

using namespace std;

namespace OHOS {
namespace CJSystemapi {
namespace FileUri {

const std::string FILE_SCHEME_PREFIX = "file://";
const std::string MEDIA_AUTHORITY = "media";
string FileUriImpl::GetName()
{
    string sandboxPath = AppFileService::SandboxHelper::Decode(uri_.GetPath());
    size_t posLast = sandboxPath.find_last_of("/");
    if (posLast == string::npos) {
        return "";
    }

    if (posLast == sandboxPath.size()) {
        return "";
    }

    return sandboxPath.substr(posLast + 1);
}

string FileUriImpl::GetPath()
{
    string sandboxPath = AppFileService::SandboxHelper::Decode(uri_.GetPath());
    string bundleName = uri_.GetAuthority();
    if (bundleName == MEDIA_AUTHORITY && sandboxPath.find(".") != string::npos) {
        size_t pos = sandboxPath.rfind("/");
        return sandboxPath.substr(0, pos);
    }

    return sandboxPath;
}

string FileUriImpl::ToString()
{
    return uri_.ToString();
}

FileUriImpl::FileUriImpl(const string &uriOrPath): uri_(
    (uriOrPath.find(FILE_SCHEME_PREFIX) == 0) ? uriOrPath : AppFileService::CommonFunc::GetUriFromPath(uriOrPath)
)
{}

}
}  // namespace CJSystemapi
}  // namespace OHOS