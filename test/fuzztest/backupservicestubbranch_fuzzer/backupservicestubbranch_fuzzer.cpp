/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "backupservicestubbranch_fuzzer.h"

#include <string>
#include <queue>

#include "b_incremental_data.h"
#include "b_session_backup.h"
#include "b_session_restore.h"
#include "message_parcel.h"
#include "refbase.h"
#include "service.h"
#include "service_reverse.h"

namespace OHOS {
using namespace FileManagement::Backup;
using namespace std;

using FAFVariant = std::variant<bool, int32_t, int64_t, uint32_t, std::string, std::vector<std::string>>;

list<FAFVariant> que;
list<FAFVariant> arg;

void ExpectReturn(list<FAFVariant>&& value)
{
    std::swap(que, value);
}

void ExpectArgReturn(list<FAFVariant>&& value)
{
    std::swap(arg, value);
}

bool GetBoolResult()
{
    bool ret = true;
    if (!que.empty()) {
        ret = std::get<bool>(que.front());
        que.pop_front();
    }
    return ret;
}

Parcelable::Parcelable() : Parcelable(false)
{}

Parcelable::Parcelable(bool asRemote)
{
    asRemote_ = asRemote;
    behavior_ = 0;
}

template <typename T>
bool Parcel::Write(T value)
{
    size_t desireCapacity = sizeof(T);

    if (EnsureWritableCapacity(desireCapacity)) {
        *reinterpret_cast<T *>(data_ + writeCursor_) = value;
        writeCursor_ += desireCapacity;
        dataSize_ += desireCapacity;
        return true;
    }

    return false;
}

bool Parcel::WriteParcelable(const Parcelable *object)
{
    if (object == nullptr) {
        return WriteInt32(0);
    }

    if (!object->asRemote_) {
        bool flag = WriteInt32(1);
        object->Marshalling(*this);
        return flag;
    }

    bool flag = WriteInt32(1);
    WriteRemoteObject(const_cast<Parcelable*>(object));
    return flag;
}

bool Parcel::WriteInt32(int32_t value)
{
    Write<int32_t>(value);
    return GetBoolResult();
}

bool Parcel::WriteUint32(uint32_t value)
{
    Write<uint32_t>(value);
    return GetBoolResult();
}

int32_t Parcel::ReadInt32()
{
    int32_t ret = 0;
    if (!que.empty()) {
        ret = std::get<int32_t>(que.front());
        que.pop_front();
    }
    return ret;
}

bool Parcel::ReadInt32(int32_t &value)
{
    if (!arg.empty()) {
        value = std::get<int32_t>(arg.front());
        arg.pop_front();
    }
    return GetBoolResult();
}

bool Parcel::ReadBool()
{
    return GetBoolResult();
}

bool Parcel::ReadBool(bool &value)
{
    if (!arg.empty()) {
        value = std::get<bool>(arg.front());
        arg.pop_front();
    }
    return GetBoolResult();
}

bool Parcel::WriteBool(bool value)
{
    return GetBoolResult();
}

bool Parcel::WriteString(const std::string &value)
{
    return GetBoolResult();
}

bool Parcel::ReadString(std::string &value)
{
    if (!arg.empty()) {
        value = std::get<string>(arg.front());
        arg.pop_front();
    }
    return GetBoolResult();
}

bool Parcel::ReadStringVector(std::vector<std::string> *value)
{
    if (!arg.empty()) {
        *value = std::get<std::vector<std::string>>(arg.front());
        arg.pop_front();
    }
    return GetBoolResult();
}

bool MessageParcel::WriteFileDescriptor(int fd)
{
    return GetBoolResult();
}

int MessageParcel::ReadFileDescriptor()
{
    int32_t ret = 0;
    if (!que.empty()) {
        ret = std::get<int32_t>(que.front());
        que.pop_front();
    }
    return ret;
}

bool Parcel::ReadUint32(uint32_t &value)
{
    if (!arg.empty()) {
        value = std::get<uint32_t>(arg.front());
        arg.pop_front();
    }
    return GetBoolResult();
}

constexpr int32_t SERVICE_ID = 5203;


template<class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T*>(data));
}

bool OnRemoteRequestFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = TypeCast<uint32_t>(data);

    service->OnRemoteRequest(code, msg, reply, option);

    msg.WriteInterfaceToken(ServiceStub::GetDescriptor());
    service->OnRemoteRequest(code, msg, reply, option);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::sptr service(new OHOS::FileManagement::Backup::Service(OHOS::SERVICE_ID));
    if (service == nullptr) {
        return 0;
    }

    try {
        OHOS::OnRemoteRequestFuzzTest(service, data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return 0;
}