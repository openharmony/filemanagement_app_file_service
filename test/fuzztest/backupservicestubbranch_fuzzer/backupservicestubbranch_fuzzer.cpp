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

bool CmdInitRestoreSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        msg.WriteBuffer(data, size);
        service->CmdInitRestoreSession(msg, reply);

        BSessionRestore::Callbacks callbacks;
        msg.WriteRemoteObject(new ServiceReverse(callbacks));
        ExpectReturn({false});
        service->CmdInitRestoreSession(msg, reply);

        msg.FlushBuffer();
        msg.WriteRemoteObject(new ServiceReverse(callbacks));
        ExpectReturn({true});
        service->CmdInitRestoreSession(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdInitBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        msg.WriteBuffer(data, size);
        service->CmdInitBackupSession(msg, reply);

        BSessionBackup::Callbacks callbacks;
        msg.WriteRemoteObject(new ServiceReverse(callbacks));
        ExpectReturn({false});
        service->CmdInitBackupSession(msg, reply);

        msg.FlushBuffer();
        msg.WriteRemoteObject(new ServiceReverse(callbacks));
        ExpectReturn({true});
        service->CmdInitBackupSession(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdStartFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        msg.WriteBuffer(data, size);
        ExpectReturn({false});
        service->CmdStart(msg, reply);

        ExpectReturn({true});
        service->CmdStart(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetLocalCapabilitiesFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        msg.WriteBuffer(data, size);
        ExpectReturn({false});
        service->CmdGetLocalCapabilities(msg, reply);

        ExpectReturn({true});
        service->CmdGetLocalCapabilities(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdPublishFileFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(TmpFileSN)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        BFileInfo info;
        info.sn = TypeCast<TmpFileSN>(data, &pos);
        int len = (size - pos) >> 1;
        info.owner = string(reinterpret_cast<const char*>(data + pos), len);
        info.fileName = string(reinterpret_cast<const char*>(data + pos + len), len);

        msg.WriteParcelable(&info);
        ExpectReturn({1, true, true, true, false});
        ExpectArgReturn({info.owner, info.fileName, info.sn});
        service->CmdPublishFile(msg, reply);

        msg.FlushBuffer();
        msg.WriteParcelable(&info);
        ExpectReturn({1, true, true, true, true});
        ExpectArgReturn({info.owner, info.fileName, info.sn});
        service->CmdPublishFile(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppFileReadyFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(bool) + sizeof(int)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        int32_t errCode = TypeCast<int32_t>(data + pos, &pos);
        bool flag = TypeCast<bool>(data + pos, &pos);
        string fileName(reinterpret_cast<const char*>(data + pos), size - pos);

        service->CmdAppFileReady(msg, reply);

        ExpectReturn({false});
        ExpectArgReturn({fileName});
        service->CmdAppFileReady(msg, reply);

        msg.FlushBuffer();
        flag == true ? ExpectReturn({true, flag, fd, errCode, false}) : ExpectReturn({true, flag, errCode, false});
        ExpectArgReturn({fileName});
        service->CmdAppFileReady(msg, reply);

        msg.FlushBuffer();
        flag == true ? ExpectReturn({true, flag, fd, errCode, true}) : ExpectReturn({true, flag, errCode, true});
        ExpectArgReturn({fileName});
        service->CmdAppFileReady(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppDoneFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int32_t errCode = TypeCast<int32_t>(data);

        ExpectReturn({false});
        ExpectArgReturn({errCode});
        service->CmdAppDone(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({errCode});
        service->CmdAppDone(msg, reply);

        ExpectReturn({true, true});
        ExpectArgReturn({errCode});
        service->CmdAppDone(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdResultReportFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t scenario = TypeCast<int32_t>(data, &pos);
        int32_t errCode = TypeCast<int32_t>(data + pos, &pos);
        string restoreRetInfo(reinterpret_cast<const char*>(data + pos), size - pos);

        ExpectReturn({false});
        ExpectArgReturn({restoreRetInfo});
        service->CmdResultReport(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({restoreRetInfo, scenario});
        service->CmdResultReport(msg, reply);

        ExpectReturn({true, true, false});
        ExpectArgReturn({restoreRetInfo, scenario, errCode});
        service->CmdResultReport(msg, reply);

        ExpectReturn({true, true, true, false});
        ExpectArgReturn({restoreRetInfo, scenario, errCode});
        service->CmdResultReport(msg, reply);

        ExpectReturn({true, true, true, true});
        ExpectArgReturn({restoreRetInfo, scenario, errCode});
        service->CmdResultReport(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetFileHandleFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        int len = size >> 1;
        string bundleName(reinterpret_cast<const char*>(data), len);
        string fileName(reinterpret_cast<const char*>(data + len), size - len);

        ExpectReturn({false});
        ExpectArgReturn({bundleName});
        service->CmdGetFileHandle(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({bundleName, fileName});
        service->CmdGetFileHandle(msg, reply);

        ExpectReturn({true, true});
        ExpectArgReturn({bundleName, fileName});
        service->CmdGetFileHandle(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesRestoreSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int) + sizeof(int32_t) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        int32_t type = TypeCast<int32_t>(data + pos, &pos);
        int32_t userId = TypeCast<int32_t>(data + pos, &pos);
        vector<string> bundleNames;
        bundleNames.emplace_back(string(reinterpret_cast<const char*>(data + pos), size - pos));

        ExpectReturn({fd, false});
        ExpectArgReturn({bundleNames});
        service->CmdAppendBundlesRestoreSession(msg, reply);

        ExpectReturn({fd, true, false});
        ExpectArgReturn({bundleNames, type});
        service->CmdAppendBundlesRestoreSession(msg, reply);

        ExpectReturn({fd, true, true, false});
        ExpectArgReturn({bundleNames, type, userId});
        service->CmdAppendBundlesRestoreSession(msg, reply);

        ExpectReturn({fd, true, true, true, false});
        ExpectArgReturn({bundleNames, type, userId});
        service->CmdAppendBundlesRestoreSession(msg, reply);

        ExpectReturn({fd, true, true, true, true});
        ExpectArgReturn({bundleNames, type, userId});
        service->CmdAppendBundlesRestoreSession(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesDetailsRestoreSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int) + sizeof(int32_t) + sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int fd = TypeCast<int>(data, &pos);
        int32_t type = TypeCast<int32_t>(data + pos, &pos);
        int32_t userId = TypeCast<int32_t>(data + pos, &pos);
        int len = (size - pos) >> 1;
        vector<string> bundleNames;
        bundleNames.emplace_back(string(reinterpret_cast<const char*>(data + pos), len));
        vector<string> detailInfos;
        detailInfos.emplace_back(string(reinterpret_cast<const char*>(data + pos + len), len));

        ExpectReturn({fd, false});
        ExpectArgReturn({bundleNames});
        service->CmdAppendBundlesDetailsRestoreSession(msg, reply);

        ExpectReturn({fd, true, false});
        ExpectArgReturn({bundleNames, detailInfos});
        service->CmdAppendBundlesDetailsRestoreSession(msg, reply);

        ExpectReturn({fd, true, true, false});
        ExpectArgReturn({bundleNames, detailInfos, type});
        service->CmdAppendBundlesDetailsRestoreSession(msg, reply);

        ExpectReturn({fd, true, true, true, false});
        ExpectArgReturn({bundleNames, detailInfos, type, userId});
        service->CmdAppendBundlesDetailsRestoreSession(msg, reply);

        ExpectReturn({fd, true, true, true, true, false});
        ExpectArgReturn({bundleNames, detailInfos, type, userId});
        service->CmdAppendBundlesDetailsRestoreSession(msg, reply);

        ExpectReturn({fd, true, true, true, true, true});
        ExpectArgReturn({bundleNames, detailInfos, type, userId});
        service->CmdAppendBundlesDetailsRestoreSession(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        vector<string> bundleNames;
        bundleNames.emplace_back(string(reinterpret_cast<const char*>(data), size));

        ExpectReturn({false});
        ExpectArgReturn({bundleNames});
        service->CmdAppendBundlesBackupSession(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({bundleNames});
        service->CmdAppendBundlesBackupSession(msg, reply);

        ExpectReturn({true, true});
        ExpectArgReturn({bundleNames});
        service->CmdAppendBundlesBackupSession(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesDetailsBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        int len = size >> 1;
        vector<string> bundleNames;
        bundleNames.emplace_back(string(reinterpret_cast<const char*>(data), len));
        vector<string> detailInfos;
        detailInfos.emplace_back(string(reinterpret_cast<const char*>(data + len), len));

        ExpectReturn({false});
        ExpectArgReturn({bundleNames});
        service->CmdAppendBundlesDetailsBackupSession(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({bundleNames, detailInfos});
        service->CmdAppendBundlesDetailsBackupSession(msg, reply);

        ExpectReturn({true, true, false});
        ExpectArgReturn({bundleNames, detailInfos});
        service->CmdAppendBundlesDetailsBackupSession(msg, reply);

        ExpectReturn({true, true, true});
        ExpectArgReturn({bundleNames, detailInfos});
        service->CmdAppendBundlesDetailsBackupSession(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdFinishFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    try {
        msg.WriteBuffer(data, size);
        ExpectReturn({false});
        service->CmdFinish(msg, reply);

        ExpectReturn({true});
        service->CmdFinish(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

[[maybe_unused]] bool CmdReleaseFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    try {
        msg.WriteBuffer(data, size);
        service->CmdRelease(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetLocalCapabilitiesIncrementalFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        BIncrementalData bundleNames(string(reinterpret_cast<const char*>(data), size), 0);

        int32_t infoSize = 1;
        ExpectReturn({false});
        ExpectArgReturn({infoSize});
        service->CmdGetLocalCapabilitiesIncremental(msg, reply);

        ExpectReturn({true, true, true, true, true, true, 1, true, true, true, 0, true, false});
        ExpectArgReturn({infoSize, "", "", 1});
        msg.FlushBuffer();
        msg.WriteParcelable(&bundleNames);
        service->CmdGetLocalCapabilitiesIncremental(msg, reply);

        ExpectReturn({true, true, true, true, true, true, 1, true, true, true, 0, true, true});
        ExpectArgReturn({infoSize, "", "", 1});
        msg.FlushBuffer();
        msg.WriteParcelable(&bundleNames);
        service->CmdGetLocalCapabilitiesIncremental(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetAppLocalListAndDoIncrementalBackupFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    try {
        msg.WriteBuffer(data, size);
        ExpectReturn({false});
        service->CmdGetAppLocalListAndDoIncrementalBackup(msg, reply);

        ExpectReturn({true});
        service->CmdGetAppLocalListAndDoIncrementalBackup(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdInitIncrementalBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        msg.WriteBuffer(data, size);
        service->CmdInitIncrementalBackupSession(msg, reply);

        BIncrementalBackupSession::Callbacks callbacks;
        msg.WriteRemoteObject(new ServiceReverse(callbacks));
        ExpectReturn({false});
        service->CmdInitIncrementalBackupSession(msg, reply);

        msg.FlushBuffer();
        msg.WriteRemoteObject(new ServiceReverse(callbacks));
        ExpectReturn({true});
        service->CmdInitIncrementalBackupSession(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesIncrementalBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        BIncrementalData bundlesToBackup(string(reinterpret_cast<const char*>(data), size), 0);

        ExpectReturn({false});
        service->CmdAppendBundlesIncrementalBackupSession(msg, reply);

        int32_t infoSize = 1;
        ExpectReturn({true, true, true, true, true, true, 1, true, true, true, 0, false});
        ExpectArgReturn({infoSize, "", "", 1});
        msg.FlushBuffer();
        msg.WriteParcelable(&bundlesToBackup);
        service->CmdAppendBundlesIncrementalBackupSession(msg, reply);

        ExpectReturn({true, true, true, true, true, true, 1, true, true, true, 0, true});
        ExpectArgReturn({infoSize, "", "", 1});
        msg.FlushBuffer();
        msg.WriteParcelable(&bundlesToBackup);
        service->CmdAppendBundlesIncrementalBackupSession(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppendBundlesDetailsIncrementalBackupSessionFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        int32_t infoSize = 1;
        int len = size >> 1;
        BIncrementalData bundlesToBackup(string(reinterpret_cast<const char*>(data), len), 0);
        std::vector<std::string> infos;
        infos.emplace_back(string(reinterpret_cast<const char*>(data + len), len));

        ExpectReturn({false});
        ExpectArgReturn({infoSize});
        service->CmdAppendBundlesDetailsIncrementalBackupSession(msg, reply);

        ExpectReturn({true, true, true, true, true, true, 1, true, true, true, 0, false});
        ExpectArgReturn({infoSize, "", "", 1, infos});
        msg.FlushBuffer();
        msg.WriteParcelable(&bundlesToBackup);
        service->CmdAppendBundlesDetailsIncrementalBackupSession(msg, reply);

        ExpectReturn({true, true, true, true, true, true, 1, true, true, true, 0, true, false});
        ExpectArgReturn({infoSize, "", "", 1, infos});
        msg.FlushBuffer();
        msg.WriteParcelable(&bundlesToBackup);
        service->CmdAppendBundlesDetailsIncrementalBackupSession(msg, reply);

        ExpectReturn({true, true, true, true, true, true, 1, true, true, true, 0, true, true});
        ExpectArgReturn({infoSize, "", "", 1, infos});
        msg.FlushBuffer();
        msg.WriteParcelable(&bundlesToBackup);
        service->CmdAppendBundlesDetailsIncrementalBackupSession(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdPublishIncrementalFileFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(TmpFileSN)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        BFileInfo info;
        info.sn = TypeCast<TmpFileSN>(data, &pos);
        int len = (size - pos) >> 1;
        info.owner = string(reinterpret_cast<const char*>(data + pos), len);
        info.fileName = string(reinterpret_cast<const char*>(data + pos + len), len);

        msg.WriteParcelable(&info);
        ExpectReturn({1, true, true, true, false});
        ExpectArgReturn({info.owner, info.fileName, info.sn});
        service->CmdPublishIncrementalFile(msg, reply);

        msg.FlushBuffer();
        msg.WriteParcelable(&info);
        ExpectReturn({1, true, true, true, true});
        ExpectArgReturn({info.owner, info.fileName, info.sn});
        service->CmdPublishIncrementalFile(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

[[maybe_unused]] bool CmdPublishSAIncrementalFileFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int) + sizeof(TmpFileSN)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        BFileInfo info;
        int fd = TypeCast<int>(data, &pos);
        info.sn = TypeCast<TmpFileSN>(data, &pos);
        int len = (size - pos) >> 1;
        info.owner = string(reinterpret_cast<const char*>(data + pos), len);
        info.fileName = string(reinterpret_cast<const char*>(data + pos + len), len);

        msg.WriteParcelable(&info);
        ExpectReturn({fd, false});
        service->CmdPublishSAIncrementalFile(msg, reply);

        msg.FlushBuffer();
        msg.WriteParcelable(&info);
        ExpectReturn({fd, true});
        service->CmdPublishSAIncrementalFile(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppIncrementalFileReadyFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(bool) + sizeof(int) + sizeof(int)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int fd1 = TypeCast<int>(data, &pos);
        int fd2 = TypeCast<int>(data + pos, &pos);
        int32_t errCode = TypeCast<int32_t>(data + pos, &pos);
        bool flag = TypeCast<bool>(data + pos, &pos);
        string fileName(reinterpret_cast<const char*>(data + pos), size - pos);

        ExpectReturn({false});
        ExpectArgReturn({fileName});
        service->CmdAppIncrementalFileReady(msg, reply);

        if (flag) {
            fd1 < 0 ? ExpectReturn({true, flag, fd1, errCode, false}) :
                ExpectReturn({true, flag, fd1, fd2, errCode, false});
        } else {
            ExpectReturn({true, flag, errCode, false});
        }
        ExpectArgReturn({fileName});
        service->CmdAppIncrementalFileReady(msg, reply);

        if (flag) {
            fd1 < 0 ? ExpectReturn({true, flag, fd1, errCode, true}) :
            ExpectReturn({true, flag, fd1, fd2, errCode, true});
        } else {
            ExpectReturn({true, flag, errCode, true});
        }
        ExpectArgReturn({fileName});
        service->CmdAppIncrementalFileReady(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdAppIncrementalDoneFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int32_t errCode = TypeCast<int32_t>(data);

        ExpectReturn({false});
        ExpectArgReturn({errCode});
        service->CmdAppIncrementalDone(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({errCode});
        service->CmdAppIncrementalDone(msg, reply);

        ExpectReturn({true, true});
        ExpectArgReturn({errCode});
        service->CmdAppIncrementalDone(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetIncrementalFileHandleFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    try {
        int len = size >> 1;
        string bundleName(reinterpret_cast<const char*>(data), len);
        string fileName(reinterpret_cast<const char*>(data + len), size - len);

        ExpectReturn({false});
        ExpectArgReturn({bundleName});
        service->CmdGetIncrementalFileHandle(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({bundleName, fileName});
        service->CmdGetIncrementalFileHandle(msg, reply);

        ExpectReturn({true, true});
        ExpectArgReturn({bundleName, fileName});
        service->CmdGetIncrementalFileHandle(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdGetBackupInfoFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        string bundleName(reinterpret_cast<const char*>(data), size);

        ExpectReturn({false});
        ExpectArgReturn({bundleName});
        service->CmdGetBackupInfo(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({bundleName});
        service->CmdGetBackupInfo(msg, reply);

        ExpectReturn({true, true});
        ExpectArgReturn({bundleName});
        service->CmdGetBackupInfo(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdUpdateTimerFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        uint32_t timeout = TypeCast<uint32_t>(data, &pos);
        string bundleName(reinterpret_cast<const char*>(data + pos), size - pos);

        ExpectReturn({false});
        ExpectArgReturn({bundleName});
        service->CmdUpdateTimer(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({bundleName, timeout});
        service->CmdUpdateTimer(msg, reply);

        ExpectReturn({true, true, false});
        ExpectArgReturn({bundleName, timeout});
        service->CmdUpdateTimer(msg, reply);

        ExpectReturn({true, true, true});
        ExpectArgReturn({bundleName, timeout});
        service->CmdUpdateTimer(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}

bool CmdUpdateSendRateFuzzTest(sptr<Service> service, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    try {
        int pos = 0;
        int32_t sendRate = TypeCast<int32_t>(data, &pos);
        string bundleName(reinterpret_cast<const char*>(data + pos), size - pos);

        ExpectReturn({false});
        ExpectArgReturn({bundleName});
        service->CmdUpdateSendRate(msg, reply);

        ExpectReturn({true, false});
        ExpectArgReturn({bundleName, sendRate});
        service->CmdUpdateSendRate(msg, reply);

        ExpectReturn({true, true, false});
        ExpectArgReturn({bundleName, sendRate});
        service->CmdUpdateSendRate(msg, reply);

        ExpectReturn({true, true, true});
        ExpectArgReturn({bundleName, sendRate});
        service->CmdUpdateSendRate(msg, reply);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::sptr service(new OHOS::FileManagement::Backup::Service());
    if (service == nullptr) {
        return 0;
    }

    try {
        OHOS::OnRemoteRequestFuzzTest(service, data, size);
        CmdInitRestoreSessionFuzzTest(service, data, size);
        CmdInitBackupSessionFuzzTest(service, data, size);
        CmdStartFuzzTest(service, data, size);
        CmdGetLocalCapabilitiesFuzzTest(service, data, size);
        CmdPublishFileFuzzTest(service, data, size);
        CmdAppFileReadyFuzzTest(service, data, size);
        CmdAppDoneFuzzTest(service, data, size);
        CmdResultReportFuzzTest(service, data, size);
        CmdGetFileHandleFuzzTest(service, data, size);
        CmdAppendBundlesRestoreSessionFuzzTest(service, data, size);
        CmdAppendBundlesDetailsRestoreSessionFuzzTest(service, data, size);
        CmdAppendBundlesBackupSessionFuzzTest(service, data, size);
        CmdAppendBundlesDetailsBackupSessionFuzzTest(service, data, size);
        CmdFinishFuzzTest(service, data, size);
        CmdGetLocalCapabilitiesIncrementalFuzzTest(service, data, size);
        CmdGetAppLocalListAndDoIncrementalBackupFuzzTest(service, data, size);
        CmdInitIncrementalBackupSessionFuzzTest(service, data, size);
        CmdAppendBundlesIncrementalBackupSessionFuzzTest(service, data, size);
        CmdAppendBundlesDetailsIncrementalBackupSessionFuzzTest(service, data, size);
        CmdPublishIncrementalFileFuzzTest(service, data, size);
        CmdAppIncrementalFileReadyFuzzTest(service, data, size);
        CmdAppIncrementalDoneFuzzTest(service, data, size);
        CmdGetIncrementalFileHandleFuzzTest(service, data, size);
        CmdGetBackupInfoFuzzTest(service, data, size);
        CmdUpdateTimerFuzzTest(service, data, size);
        CmdUpdateSendRateFuzzTest(service, data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return 0;
}