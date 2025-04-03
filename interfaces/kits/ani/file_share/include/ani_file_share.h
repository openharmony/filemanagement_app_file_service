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
#ifndef ANI_FILE_SHARE_H
#define ANI_FILE_SHARE_H

#include <string>
#include "iremote_broker.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileShare {

const std::string MEDIA_GRANT_URI_PERMISSION =
    "datashare:///media/bundle_permission_insert_operation/bundle_permission_insert_operation";
const std::string MEDIALIBRARY_DATA_URI = "datashare:///media";
const std::string MEDIA_FILEMODE_READONLY = "r";
const std::string GRANT_URI_NAME = "file_share_grant_uri_permission";
const std::string MEDIA_API_VERSION_10 = "?api_version=10";
const std::string PERMISSION_BUNDLE_NAME = "bundle_name";
const std::string PERMISSION_FILE_ID = "file_id";
const std::string PERMISSION_MODE = "mode";
const std::string MEDIA_AUTHORITY = "media";
const std::string FILE_SCHEME = "file";
const std::string PERMISSION_TABLE_TYPE = "table_type";
const std::string FILE_MANAGER_AUTHORITY = "docs";
const std::string MEDIA_FILE_URI_PHOTO_PREFEX = "file://media/Photo/";
const std::string MEDIA_FILE_URI_AUDIO_PREFEX = "file://media/Audio/";
const std::string MEDIA_FILE_URI_VIDEO_PREFEX = "file://media/video/";
const std::string MEDIA_FILE_URI_IMAGE_PREFEX = "file://media/image/";
const std::string MEDIA_FILE_URI_FILE_PREFEX = "file://media/file/";
const std::string MEDIA_FILE_URI_Audio_PREFEX = "file://media/audio/";

class FileShareGrantToken : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.fileshare.grantUriPermission");

    FileShareGrantToken() = default;
    virtual ~FileShareGrantToken() noexcept = default;
};

enum MediaFileTable {
    FILE_TABLE = 0,
    PHOTO_TABLE = 1,
    AUDIO_TABLE = 2,
};

struct UriPermissionInfo {
    unsigned int flag;
    std::string mode;
    std::string bundleName;
    std::string uri;
};

} // namespace ModuleFileShare
} // namespace AppFileService
} // namespace OHOS
#endif // ANI_FILE_SHARE_H
