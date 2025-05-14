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

namespace ANI::FileUri {

const std::string FILE_SCHEME_PREFIX_TAIHE = "file://";
const std::string FILE_FILEURI_FAILED = "";

FileUriImpl::FileUriImpl(taihe::string_view name)
    : uri_((std::string(name.c_str()).find(FILE_SCHEME_PREFIX_TAIHE) == 0) ? std::string(name.c_str())
    : OHOS::AppFileService::CommonFunc::GetUriFromPath(std::string(name.c_str())))
{
}

std::string FileUriImpl::getName()
{
    std::string sandboxPath = OHOS::AppFileService::SandboxHelper::Decode(uri_.GetPath());
    size_t posLast = sandboxPath.find_last_of("/");
    if (posLast == std::string::npos) {
        return FILE_FILEURI_FAILED;
    }
    if (posLast == (sandboxPath.size() - 1)) {
        return FILE_FILEURI_FAILED;
    }
    return sandboxPath.substr(posLast + 1);
}

ohos::file::fileuri::FileUri makeFileUri(taihe::string_view name)
{
    return taihe::make_holder<FileUriImpl, ohos::file::fileuri::FileUri>(name);
}

std::string getUriFromPath(taihe::string_view path)
{
    const std::string strPath = path.c_str();
    std::string uri = OHOS::AppFileService::CommonFunc::GetUriFromPath(strPath);
    return uri;
}
}  // namespace ANI::FileUri

TH_EXPORT_CPP_API_makeFileUri(ANI::FileUri::makeFileUri);
TH_EXPORT_CPP_API_getUriFromPath(ANI::FileUri::getUriFromPath);
