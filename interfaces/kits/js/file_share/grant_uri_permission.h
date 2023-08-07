/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef GRANT_URI_PERMISSION_H
#define GRANT_URI_PERMISSION_H

#include "filemgmt_libn.h"
#include "iremote_broker.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileShare {

using namespace std;

const string MEDIA_GRANT_URI_PERMISSION =
    "datashare:///media/bundle_permission_insert_operation/bundle_permission_insert_operation";
const string MEDIALIBRARY_DATA_URI = "datashare:///media";
const string MEDIA_FILEMODE_READONLY = "r";
const string GRANT_URI_NAME = "file_share_grant_uri_permission";
const string MEDIA_API_VERSION_10 = "?api_version=10";
const string PERMISSION_BUNDLE_NAME = "bundle_name";
const string PERMISSION_FILE_ID = "file_id";
const string PERMISSION_MODE = "mode";
const string MEDIA_AUTHORITY = "media";
const string FILE_SCHEME = "file";
const string PERMISSION_TABLE_TYPE = "table_type";
const string FILE_MANAGER_AUTHORITY = "docs";
const string MEDIA_FILE_URI_PHOTO_PREFEX = "file://media/Photo/";
const string MEDIA_FILE_URI_AUDIO_PREFEX = "file://media/Audio/";
const string MEDIA_FILE_URI_VIDEO_PREFEX = "file://media/video/";
const string MEDIA_FILE_URI_IMAGE_PREFEX = "file://media/image/";
const string MEDIA_FILE_URI_FILE_PREFEX = "file://media/file/";
const string MEDIA_FILE_URI_Audio_PREFEX = "file://media/audio/";

class FileShareGrantToken : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.fileshare.grantUriPermission");

    FileShareGrantToken() = default;
    virtual ~FileShareGrantToken() noexcept = default;
};

class GrantUriPermission final {
public:
    static napi_value Async(napi_env env, napi_callback_info info);
};
} // namespace ModuleFileShare
} // namespace AppFileService
} // namespace OHOS
#endif // GRANT_URI_PERMISSION_H
