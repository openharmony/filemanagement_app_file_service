/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include "gmock/gmock.h"
#include "datashare_helper.h"
#include "datashare_helper_mock.h"

namespace OHOS {
namespace DataShare {
using namespace std;
using namespace FileManageMent::Backup;

std:shared_ptr<DataShareHelper> DataShareHelper::Creator(const sptr<IRemoteObject> &token,
        const std::string &strUri, const std::string &extUri, const int waitTime, bool isSystem)
{
    GTEST_LOG_(INFO) << "DataShareHelperMock Creator is OK";
    return IDataShareHelper::idsh->Creator(token, strUri, extUri, waitTime, isSystem);
}

std::shared_ptr<DataShareResultSet> Query(Uri &uri, const DataSharePredicates &predicates,
    std::vector<string> &colums, DataShareBusinessError *businessError)
{
    GTEST_LOG_(INFO) << "DataShareHelperMock Query is OK";
    return IDataShareHelper::idsh->Query(uri, predicates, colums, businessError);
}
} 
} // using namespace OHOS::DataShare
