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

#ifndef DATASHARE_HELPER_MOCK_H
#define DATASHARE_HELPER_MOCK_H

#include <gmock/gmock.h>
#include "gmock/gmock.h"
#include "datashare_helper.h"
#include "datashare_helper_mock.h"

namespace OHOS {
namespace FileManageMent::Backup {
using namespace DataShare;
class IDataShareHelper {
public:
    IDataShareHelper() = default;
    virtual ~IDataShareHelper() = default;
    virtual std::shared_ptr<DataShareHelper> Creator(const sptr<IRemoteObject> &token,
        const string &strUri, const string &extUri, const int waitTime, bool isSystem) = 0;
    virtual std::shared_ptr<DataShareResultSet> Query(Uri &uri, const DataSharePredicates &predicates,
        std::vector<string> &colums, DataShareBusinessError *businessError) = 0;;
public:
    static inline std::shared_ptr<IDataShareHelper> idsh = nullptr;
};

class DataShareHelperImpl : public DataShareHelper {
public:
    MOCK_METHOD(std::shared_ptr<DataShareResultSet>, Query, (Uri &uri, const DataSharePredicates& predicates,
        std::vector<string> &colums, DataShareBusinessError *businessError), (override));

    MOCK_METHOD(bool, Release, (), (override));

    MOCK_METHOD(std::vector<string>, GetFileTypes, (Uri &uri, const string &mimeTypeFilter), (override));

    MOCK_METHOD(int, OpenFile, (Uri &uri, const std::string &mode), (override));

    MOCK_METHOD(int, OpenRawFile, (Uri &uri, const std::string &mode), (override));

    MOCK_METHOD(int, Insert, (Uri &uri, const DataShareValuesBucket &value), (override));

    MOCK_METHOD(int, InsertExt, (Uri &uri, const DataShareValuesBucket &value, std::string &result), (override));

    MOCK_METHOD(int, Update, (Uri &uri, const DataSharePredicates &predicates,
        const DataShareValuesBucket &value), (override));

    MOCK_METHOD(int, BatchUpdate, (const UpdateOperations &operations,
        std::vector<BatchUpdateResult> &results), (override));

    MOCK_METHOD(int, Delete, (Uri &uri, const DataSharePredicates &predicates), (override));

    MOCK_METHOD(string, GetType, (Uri &uri), (override));

    MOCK_METHOD(int, BatchInsert, (Uri &uri, const std::vector<DataShareValuesBucket> &values), (override));

    MOCK_METHOD(int, ExecuteBatch, (const std::vector<OperationStatement> &statements,
        ExecResultSet &result), (override));

    MOCK_METHOD(int, RegisterObserver, (const Uri &uri,
        const sptr<AAFwk::IDataAbilityObserver> &dataObserver), (override));

    MOCK_METHOD(int, UnregisterObserver, (const Uri &uri,
        const sptr<AAFwk::IDataAbilityObserver> &dataObserver), (override));

    MOCK_METHOD(void, NotifyChange, (const Uri &uri), (override));

    MOCK_METHOD(int, RegisterObserverExtProvider, (const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver,
        bool isDescendants), (override));

    MOCK_METHOD(int, UnregisterObserverExtProvider, (const Uri &uri,
        std::shared_ptr<DataShareObserver> dataObserver), (override));

    MOCK_METHOD(void, NotifyChangeExtProvider, (const DataShareObserver::ChangeInfo &changeInfo), (override));

    MOCK_METHOD(Uri, NormalizeUri, (Uri &uri), (override));

    MOCK_METHOD(Uri, DenormalizeUri, (Uri &uri), (override));

    MOCK_METHOD(int, AddQueryTemplate, (const std::string &uri, int64_t subscriberId, Template &tpl), (override));

    MOCK_METHOD(int, DelQueryTemplate, (const std::string &uri, int64_t subscriberId), (override));

    MOCK_METHOD(std::vector<OperationResult>, Publish, (const Data &data, const std::string &bundleName), (override));

    MOCK_METHOD(Data, GetPublishedData, (const std::string &bundleName, int &resultCode), (override));

    MOCK_METHOD(std::vector<OperationResult>, SubscribeRdbData, (const std::vector<std::string> &uris,
        const TemplateId &templateId,
        const std::function<void(const RdbChangeNode &changeNode)> &callback), (override));

    MOCK_METHOD(std::vector<OperationResult>, UnsubscribeRdbData, (const std::vector<std::string> &uris,
        const TemplateId &templateId), (override));

    MOCK_METHOD(std::vector<OperationResult>, EnableRdbSubs, (const std::vector<std::string> &uris,
        const TemplateId &templateId), (override));

    MOCK_METHOD(std::vector<OperationResult>, DisableRdbSubs, (const std::vector<std::string> &uris,
        const TemplateId &templateId), (override));

    MOCK_METHOD(std::vector<OperationResult>, SubscribePublishedData,
        (const std::vector<std::string> &uris, int64_t subscriberId,
        const std::function<void(const PublishedDataChangeNode &changeNode)> &callback), (override));

    MOCK_METHOD(std::vector<OperationResult>, UnsubscribePublishedData, (const std::vector<std::string> &uris,
        int64_t subscriberId), (override));

    MOCK_METHOD(std::vector<OperationResult>, EnablePubSubs,
        (const std::vector<std::string> &uris, int64_t subscriberId), (override));

    MOCK_METHOD(std::vector<OperationResult>, DisablePubSubs,
        (const std::vector<std::string> &uris, int64_t subscriberId), (override));

    MOCK_METHOD(std::pair<int32_t, int32_t>, InsertEx,
        (Uri &uri, const DataShareValuesBucket &value), (override));

    MOCK_METHOD(std::pair<int32_t, int32_t>, UpdateEx, (Uri &uri, const DataSharePredicates &predicates,
        const DataShareValuesBucket &value), (override));

    MOCK_METHOD((std::pair<int32_t, int32_t>), DeleteEx,
        (Uri &uri, const DataSharePredicates &predicates), (override));

    MOCK_METHOD(int32_t, UserDefineFunc, (MessageParcel &data, MessageParcel &reply,
        MessageOption &option), (override));
};

class DataShareHelperMock : public IDataShareHelper {
public:
    MOCK_METHOD(std::shared_ptr<DataShareHelper>, Creator, (const sptr<IRemoteObject> &token,
        const string &strUri, const string &extUri, const int waitTime, bool isSystem));

    MOCK_METHOD(std::shared_ptr<DataShareResultSet>, Query, (Uri &uri, const DataSharePredicates &predicates,
        std::vector<string> &colums, DataShareBusinessError *businessError));
}
}
} // namespace OHOS::FileManagement::Backup
#endif
