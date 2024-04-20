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

#include "message_parcel.h"
#include "ext_extension.h"
#include "ext_backup.h"

namespace OHOS {
using namespace std;
using namespace OHOS::FileManagement::Backup;

constexpr uint8_t MAX_CALL_TRANSACTION = 10;

template<class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T*>(data));
}

bool OnRemoteRequestFuzzTest(shared_ptr<BackupExtExtension> extension, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;

    int pos = 0;
    uint32_t code = TypeCast<uint32_t>(data, &pos);
    msg.WriteInterfaceToken(ExtExtensionStub::GetDescriptor());
    msg.WriteBuffer(data + pos, size - pos);
    msg.RewindRead(0);

    extension->OnRemoteRequest(code % MAX_CALL_TRANSACTION, msg, reply, option);
    return true;
}

bool InitFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
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
    want.SetElementName(string(reinterpret_cast<const char*>(data + pos), len),
                        string(reinterpret_cast<const char*>(data + pos + len), len));

    backup->OnCommand(want, restart, startId);
    return true;
}

bool OnConnectFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    int len = size >> 1;
    AAFwk::Want want;
    want.SetElementName(string(reinterpret_cast<const char*>(data), len),
                        string(reinterpret_cast<const char*>(data + len), size - len));

    backup->OnConnect(want);
    return true;
}

bool CreateFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    unique_ptr<AbilityRuntime::Runtime> runtime;
    backup->Create(runtime);
    return true;
}

bool GetExtensionActionFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    backup->GetExtensionAction();
    return true;
}

bool OnBackupFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    function<void()> callback = nullptr;
    backup->OnBackup(callback);
    return true;
}

bool OnRestoreFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    function<void()> callback;
    function<void(const string)> callbackEx;
    function<void()> callbackExAppDone;
    backup->OnRestore(callback, callbackEx, callbackExAppDone);
    return true;
}

bool OnRestore2FuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    function<void()> callback = nullptr;
    backup->OnRestore(callback);
    return true;
}

bool GetBackupInfoFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    function<void(string)> callback = nullptr;
    backup->GetBackupInfo(callback);
    return true;
}

bool WasFromSpecialVersionFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    backup->WasFromSpecialVersion();
    return true;
}

bool SpeicalVersionForCloneAndCloudFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    backup->SpeicalVersionForCloneAndCloud();
    return true;
}

bool RestoreDataReadyFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    backup->RestoreDataReady();
    return true;
}

bool CallExtRestoreFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    string result = string(reinterpret_cast<const char*>(data), size);
    backup->CallExtRestore(result);
    return true;
}

bool SetCreatorFuzzTest(shared_ptr<ExtBackup> backup, const uint8_t *data, size_t size)
{
    CreatorFunc creator;
    backup->SetCreator(creator);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    auto extBackup = std::make_shared<OHOS::FileManagement::Backup::ExtBackup>();
    auto extension = std::make_shared<OHOS::FileManagement::Backup::BackupExtExtension>(extBackup);

    OHOS::OnRemoteRequestFuzzTest(extension, data, size);
    OHOS::InitFuzzTest(extBackup, data, size);
    OHOS::OnCommandFuzzTest(extBackup, data, size);
    OHOS::OnConnectFuzzTest(extBackup, data, size);
    OHOS::CreateFuzzTest(extBackup, data, size);
    OHOS::GetExtensionActionFuzzTest(extBackup, data, size);
    OHOS::OnBackupFuzzTest(extBackup, data, size);
    OHOS::OnRestoreFuzzTest(extBackup, data, size);
    OHOS::OnRestore2FuzzTest(extBackup, data, size);
    OHOS::GetBackupInfoFuzzTest(extBackup, data, size);
    OHOS::WasFromSpecialVersionFuzzTest(extBackup, data, size);
    OHOS::SpeicalVersionForCloneAndCloudFuzzTest(extBackup, data, size);
    OHOS::RestoreDataReadyFuzzTest(extBackup, data, size);
    OHOS::CallExtRestoreFuzzTest(extBackup, data, size);
    OHOS::SetCreatorFuzzTest(extBackup, data, size);

    return 0;
}
