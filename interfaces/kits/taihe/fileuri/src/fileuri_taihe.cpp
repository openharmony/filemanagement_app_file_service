/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fileuri_taihe.h"

#include "common_func.h"
#include "sandbox_helper.h"

using namespace ANI::fileUri;

namespace ANI::fileUri {

const std::string FILE_SCHEME_PREFIX_TAIHE = "file://";

FileUriImpl::FileUriImpl(string_view name)
    : uri_((std::string(name.c_str()).find(FILE_SCHEME_PREFIX_TAIHE) == 0) ? std::string(name.c_str())
    : OHOS::AppFileService::CommonFunc::GetUriFromPath(std::string(name.c_str())))
{
}

string FileUriImpl::getName()
{
std::string sandboxPath = OHOS::AppFileService::SandboxHelper::Decode(uri_.GetPath());
    size_t posLast = sandboxPath.find_last_of("/");
    if (posLast == std::string::npos) {
        return "";
    }
    if (posLast == (sandboxPath.size() - 1)) {
        return "";
    }
    return sandboxPath.substr(posLast + 1);
}


FileUri makeFileUri(string_view name)
{
    return make_holder<FileUriImpl, FileUri>(name);
}

string getUriFromPath(string_view path)
{
    const std::string strPath = path.c_str();
    std::string uri = OHOS::AppFileService::CommonFunc::GetUriFromPath(strPath);

    return uri;
}
}  // namespace ANI::fileUri

TH_EXPORT_CPP_API_makeFileUri(makeFileUri);
TH_EXPORT_CPP_API_getUriFromPath(getUriFromPath);