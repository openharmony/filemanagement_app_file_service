/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "backupext_fuzzer.h"
#include <cstring>

#include "ext_backup.h"
#include "ext_extension.h"
#include "message_parcel.h"

namespace OHOS {
using namespace std;
using namespace OHOS::FileManagement::Backup;

template <class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T *>(data));
}

bool InitFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    (void)data;
    shared_ptr<AbilityRuntime::AbilityLocalRecord> record = nullptr;
    shared_ptr<AbilityRuntime::OHOSApplication> application = nullptr;
    shared_ptr<AbilityRuntime::AbilityHandler> handler = nullptr;
    const sptr<IRemoteObject> token = nullptr;
    backup->Init(record, application, handler, token);
    return true;
}

bool OnCommandFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool) + sizeof(int)) {
        return true;
    }

    int pos = 0;
    bool restart = TypeCast<bool>(data, &pos);
    int startId = TypeCast<int>(data + pos, &pos);
    int len = (size - pos) >> 1;
    AAFwk::Want want;
    want.SetElementName(string(reinterpret_cast<const char *>(data + pos), len),
                        string(reinterpret_cast<const char *>(data + pos + len), len));

    backup->OnCommand(want, restart, startId);
    return true;
}

bool OnConnectFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    int len = size >> 1;
    AAFwk::Want want;
    want.SetElementName(string(reinterpret_cast<const char *>(data), len),
                        string(reinterpret_cast<const char *>(data + len), size - len));

    backup->OnConnect(want);
    return true;
}

bool CreateFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    (void)data;
    unique_ptr<AbilityRuntime::Runtime> runtime = nullptr;
    backup->Create(runtime);
    return true;
}

bool GetExtensionActionFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    (void)data;
    backup->GetExtensionAction();
    return true;
}

bool OnRestoreFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    function<void(ErrCode, string)> callback = [data](ErrCode, string) {};
    function<void(ErrCode, const string)> callbackEx;
    backup->OnRestore(callback, callbackEx);
    return true;
}

bool GetBackupInfoFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    function<void(ErrCode, string)> callback = [data](ErrCode, string) {};
    backup->GetBackupInfo(callback);
    return true;
}

bool WasFromSpecialVersionFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    (void)data;
    backup->WasFromSpecialVersion();
    return true;
}

bool SpecialVersionForCloneAndCloudFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    (void)data;
    backup->SpecialVersionForCloneAndCloud();
    return true;
}

bool RestoreDataReadyFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    (void)data;
    backup->RestoreDataReady();
    return true;
}

bool InvokeAppExtMethodFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    string result = string(reinterpret_cast<const char *>(data), size);
    backup->InvokeAppExtMethod(BError(BError::Codes::OK), result);
    return true;
}

bool SetCreatorFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    (void)data;
    CreatorFunc creator;
    backup->SetCreator(creator);
    return true;
}

bool CmdGetFileHandleFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_GET_FILE_HANDLE_WITH_UNIQUE_FD);
    msg.WriteString(string(reinterpret_cast<const char *>(data), size));
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdHandleClearFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_HANDLE_CLEAR);
    msg.WriteBuffer(data, size);
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdHandleUser0BackupFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_USER0_ON_BACKUP);
    msg.WriteBuffer(data, size);
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdHandleBackupFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_HANDLE_BACKUP);
    msg.WriteBool(*reinterpret_cast<const bool *>(data));
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdPublishFileFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_PUBLISH_FILE);
    msg.WriteString(string(reinterpret_cast<const char *>(data), size));
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdHandleRestoreFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_HANDLE_RESTORE);
    msg.WriteBool(*reinterpret_cast<const bool *>(data));
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdGetIncrementalFileHandleFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_GET_INCREMENTAL_FILE_HANDLE);
    msg.WriteString(string(reinterpret_cast<const char *>(data), size));
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdPublishIncrementalFileFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_PUBLISH_INCREMENTAL_FILE);
    msg.WriteString(string(reinterpret_cast<const char *>(data), size));
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdHandleIncrementalBackupFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int) + sizeof(int)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    int pos = 0;
    int incrementalFd = TypeCast<int>(data, &pos);
    int manifestFd = TypeCast<int>(data + pos);
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_HANDLE_INCREMENTAL_BACKUP);
    msg.WriteFileDescriptor(incrementalFd);
    msg.WriteFileDescriptor(manifestFd);
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdIncrementalOnBackupFuzzTest(OHOS::sptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_INCREMENTAL_ON_BACKUP);
    msg.WriteBool(*reinterpret_cast<const bool *>(data));
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdGetIncrementalBackupFileHandleFuzzTest(OHOS::sptr<BackupExtExtension> extension,
                                               const uint8_t *data,
                                               size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IExtensionIpcCode::COMMAND_GET_INCREMENTAL_BACKUP_FILE_HANDLE);
    msg.WriteBuffer(data, size);
    if (extension == nullptr) {
        return false;
    }
    extension->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool OnRemoteRequestFuzzTest(OHOS::sptr<BackupExtExtension> extension,  const uint8_t *data, size_t size)
{
    uint32_t codeMax = 17;
    for (uint32_t code = 1; code < codeMax; code++) {
        MessageParcel datas;
        MessageParcel reply;
        MessageOption option;

        datas.WriteInterfaceToken(ExtensionStub::GetDescriptor());
        datas.WriteBuffer(reinterpret_cast<const char*>(data), size);
        datas.RewindRead(0);
        if (extension == nullptr) {
            return false;
        }
        try {
            extension->OnRemoteRequest(code, datas, reply, option);
        } catch (OHOS::FileManagement::Backup::BError &err) {
            HILOGE("BackupSaFuzzTest error");
        } catch (...) {
            HILOGE("BackupSaFuzzTest exception");
        }
    }
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    auto extBackup = std::make_shared<OHOS::FileManagement::Backup::ExtBackup>();
    auto extension = OHOS::sptr<OHOS::FileManagement::Backup::BackupExtExtension>(
        new OHOS::FileManagement::Backup::BackupExtExtension(extBackup, ""));

    OHOS::OnRemoteRequestFuzzTest(extension, data, size);
    OHOS::InitFuzzTest(extBackup, data, size);
    OHOS::OnCommandFuzzTest(extBackup, data, size);
    OHOS::OnConnectFuzzTest(extBackup, data, size);
    OHOS::CreateFuzzTest(extBackup, data, size);
    OHOS::GetExtensionActionFuzzTest(extBackup, data, size);
    OHOS::OnRestoreFuzzTest(extBackup, data, size);
    OHOS::GetBackupInfoFuzzTest(extBackup, data, size);
    OHOS::WasFromSpecialVersionFuzzTest(extBackup, data, size);
    OHOS::SpecialVersionForCloneAndCloudFuzzTest(extBackup, data, size);
    OHOS::RestoreDataReadyFuzzTest(extBackup, data, size);
    OHOS::InvokeAppExtMethodFuzzTest(extBackup, data, size);
    OHOS::SetCreatorFuzzTest(extBackup, data, size);

    try {
        OHOS::CmdGetFileHandleFuzzTest(extension, data, size);
        OHOS::CmdHandleClearFuzzTest(extension, data, size);
        OHOS::CmdHandleUser0BackupFuzzTest(extension, data, size);
        OHOS::CmdHandleBackupFuzzTest(extension, data, size);
        OHOS::CmdPublishFileFuzzTest(extension, data, size);
        OHOS::CmdHandleRestoreFuzzTest(extension, data, size);
        OHOS::CmdGetIncrementalFileHandleFuzzTest(extension, data, size);
        OHOS::CmdPublishIncrementalFileFuzzTest(extension, data, size);
        OHOS::CmdHandleIncrementalBackupFuzzTest(extension, data, size);
        OHOS::CmdIncrementalOnBackupFuzzTest(extension, data, size);
        OHOS::CmdGetIncrementalBackupFileHandleFuzzTest(extension, data, size);
    } catch (OHOS::FileManagement::Backup::BError &err) {
        // Only filter BError errors, Other results are not expected.
    }
    return 0;
}
