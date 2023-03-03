/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_IREMOTE_OBJECT_MOCK_H
#define OHOS_FILEMGMT_BACKUP_IREMOTE_OBJECT_MOCK_H

#include <gtest/gtest.h>

#include "iremote_object.h"

namespace OHOS::FileManagement::Backup {
class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject(u"mock_i_remote_object")
    {
        GTEST_LOG_(INFO) << "MockIRemoteObject is ok";
    }

    ~MockIRemoteObject() {}

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return 0;
    }

    int32_t GetObjectRefCount() override
    {
        return 0;
    }

    bool CheckObjectLegality() const override
    {
        return true;
    }

    bool IsProxyObject() const override
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        GTEST_LOG_(INFO) << "AddDeathRecipient is ok";
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface() override
    {
        return nullptr;
    }

    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }

    std::u16string GetObjectDescriptor() const
    {
        std::u16string descriptor = std::u16string();
        return descriptor;
    }

    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        return 0;
    }
};
} // namespace OHOS::FileManagement::Backup
#endif