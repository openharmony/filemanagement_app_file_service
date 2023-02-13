/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef APP_FILE_SERVICE_FILE_SHARE
#define APP_FILE_SERVICE_FILE_SHARE

#include <string>
#include <vector>
#include "want.h"

namespace OHOS {
namespace AppFileService {
using namespace std;

#define DIR_MODE (S_IRWXU | S_IXGRP | S_IXOTH)
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define READ_URI_PERMISSION OHOS::AAFwk::Want::FLAG_AUTH_READ_URI_PERMISSION
#define WRITE_URI_PERMISSION OHOS::AAFwk::Want::FLAG_AUTH_WRITE_URI_PERMISSION

enum SHARE_FILE_TYPE { DIR_TYPE = 0, FILE_TYPE };

namespace {
const string FILE_SCHEME = "file";
const string DATA_STORAGE_PATH = "/data/storage/";
const string PACKAGE_NAME_FLAG = "<PackageName>";
const string CURRENT_USER_ID_FLAG = "<currentUserId>";
const string DATA_APP_EL2_PATH = "/data/service/el2/";
const string SHARE_R_PATH = "/r/";
const string SHARE_RW_PATH = "/rw/";
const string SHARE_PATH = "/share/";

const vector<string> SANDBOX_PATH = {
    "/data/storage/el1/bundle",
    "/data/storage/el2/base",
    "/data/storage/el1/database",
    "/data/storage/el2/database",
    "/data/storage/el1/base",
    "/data/storage/ark-cache",
    "/data/storage/ark-profile"
};

const vector<string> LOWER_PATH = {
    "/data/app/el1/bundle/public/<PackageName>",
    "/data/app/el2/<currentUserId>/base/<PackageName>",
    "/data/app/el1/<currentUserId>/database/<PackageName>",
    "/data/app/el2/<currentUserId>/database/<PackageName>",
    "/data/app/el1/<currentUserId>/base/<PackageName>",
    "/data/local/ark-cache/<PackageName>",
    "/data/local/ark-profile/<currentUserId>/<PackageName>"
};
}

class FileShare {
public:
    FileShare() {}
    static int32_t CreateShareFile(const string &uri, int32_t tokenId, int32_t flag);
    static int32_t DeleteShareFile(int32_t tokenId, vector<string> sharePathList);
    ~FileShare() {}
};
} // namespace AppFileService
} // namespace OHOS

#endif