/*
<<<<<<< HEAD
<<<<<<< HEAD
 * Copyright (c) 2025 Huawei Device Co., Ltd.
=======
 * Copyright (c) 2024 Huawei Device Co., Ltd.
>>>>>>> df18e2da (add ut for storage manager service)
=======
 * Copyright (c) 2025 Huawei Device Co., Ltd.
>>>>>>> 0cc1da75 (add ut)
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

<<<<<<< HEAD
<<<<<<< HEAD
=======
#ifndef DATASHARE_HELPER_MOCK_H
#define DATASHARE_HELPER_MOCK_H

#include <gmock/gmock.h>
#include "gmock/gmock.h"
>>>>>>> df18e2da (add ut for storage manager service)
=======
>>>>>>> 0cc1da75 (add ut)
#include "datashare_helper.h"
#include "datashare_helper_mock.h"

namespace OHOS{
<<<<<<< HEAD
<<<<<<< HEAD
namespace DataShare {
using namespace std;
using namespace FileManageMent::Backup 

std:shared_ptr<DataShareHelper> DataShareHelper::Creator(const sptr<IRemoteObject> &token,
        const std::string &strUri, const std::string &extUri, const int waitTime, bool isSystem)
{
    GTEST_LOG(INFO) << "DataShareHelperMock Creator is OK";
    return IDataShareHelper::idsh->Creator(token, strUri, extUri, waitTime, isSystem);
}

std::shared_ptr<DataShareResultSet> Query(Uri &uri,const DataSharePredicates &predicates,
        std::vector<string> &colums, DataShareBusinessError *businessError)
{
    GTEST_LOG(INFO) << "DataShareHelperMock Query is OK";
    return IDataShareHelper::idsh->Query(uri, predicates, colums, businessError);
}

=======
using namespace DataShare;
using string = std::string;
namespace FileManageMent::Backup {
class IDataShareHelper {
public:
    IDataShareHelper() = default;
    virtual ~IDataShareHelper() = default;
    virtual std::share_ptr<DataShareHelper> Creator(const sptr<IRemoteObject> &token,
        const string &strUri, const string &extUri, const int waitTime, bool isSystem) = 0;
    virtual std::shared_ptr<DataShareResultSet> Query(Uri &uri,const DataSharePredicates &predicates,
        std::vector<string> &colums, DataShareBusinessError *businessError) = 0;;
public:
    static inline std::shared_ptr<IDataShareHelper> idsh = nullptr;
};
=======
namespace DataShare {
using namespace std;
using namespace FileManageMent::Backup 
>>>>>>> 0cc1da75 (add ut)

std:shared_ptr<DataShareHelper> DataShareHelper::Creator(const sptr<IRemoteObject> &token,
        const std::string &strUri, const std::string &extUri, const int waitTime, bool isSystem)
{
    GTEST_LOG(INFO) << "DataShareHelperMock Creator is OK";
    return IDataShareHelper::idsh->Creator(token, strUri, extUri, waitTime, isSystem);
}
<<<<<<< HEAD
>>>>>>> df18e2da (add ut for storage manager service)
=======

std::shared_ptr<DataShareResultSet> Query(Uri &uri,const DataSharePredicates &predicates,
        std::vector<string> &colums, DataShareBusinessError *businessError)
{
    GTEST_LOG(INFO) << "DataShareHelperMock Query is OK";
    return IDataShareHelper::idsh->Query(uri, predicates, colums, businessError);
}

>>>>>>> 0cc1da75 (add ut)
} // using namespace OHOS::FileManageMent::Backup 
} // namespace OHOS
#endif
