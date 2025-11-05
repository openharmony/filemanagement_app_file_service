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
class FileShare {
private:
    static mutex mapMutex_;

public:
    static int32_t CreateShareFile(const vector<string> &uriList,
                                   uint32_t tokenId,
                                   uint32_t flag,
                                   vector<int32_t> &retList);
    static int32_t DeleteShareFile(uint32_t tokenId, const vector<string> &uriList);
};

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
int32_t CreateShareFile(const vector<string> &uriList, uint32_t tokenId, uint32_t flag, vector<int32_t> &retList);
int32_t DeleteShareFile(uint32_t tokenId, const vector<string> &uriList);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
} // namespace AppFileService
} // namespace OHOS

#endif