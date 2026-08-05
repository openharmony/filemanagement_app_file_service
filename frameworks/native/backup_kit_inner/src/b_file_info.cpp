/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "backup_file_info.h"

#include "filemgmt_libhilog.h"
#include "message_parcel.h"
#include <sstream>

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

bool BFileInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(owner) || !parcel.WriteString(fileName) || !parcel.WriteUint32(sn)) {
        HILOGE("Failed");
        return false;
    }
    return true;
}

bool BFileInfo::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(owner) || !parcel.ReadString(fileName) || !parcel.ReadUint32(sn)) {
        HILOGE("Failed");
        return false;
    }
    return true;
}

BFileInfo *BFileInfo::Unmarshalling(Parcel &parcel)
{
    try {
        auto result = make_unique<BFileInfo>();
        if (!result->ReadFromParcel(parcel)) {
            return nullptr;
        }
        return result.release();
    } catch (const bad_alloc &e) {
        HILOGE("Failed to unmarshall BFileInfo because of %{public}s", e.what());
    }
    return nullptr;
}

int FileOpenResult::GetReleasedFd() const
{
    return fd ? fd->Release() : -1;
}

int FileOpenResult::GetReleasedManifestFd() const
{
    return manifestFd ? manifestFd->Release() : -1;
}

std::string FileOpenResult::ToString()
{
    std::ostringstream oss;
    oss << "FileOpenResult { "
        << "errCode=" << errCode << ", "
        << "fd=" << (fd ? fd->Get() : -1) << ", "
        << "manifestFd=" << (manifestFd ? manifestFd->Get() : -1) << " }";
    return oss.str();
}

bool FileOpenResult::Marshalling(Parcel &parcel) const
{
    auto messageParcel = static_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        HILOGE("Failed to cast parcel to MessageParcel");
        return false;
    }

    if (!messageParcel->WriteInt32(errCode)) {
        HILOGE("Failed to write error code: %d", errCode);
        return false;
    }

    int isMainFdValid = (fd != nullptr && fd->Get() >= 0) ? 1 : 0;
    if (!messageParcel->WriteInt32(isMainFdValid)) {
        HILOGE("Failed to write main file descriptor validity");
        return false;
    }
    if (isMainFdValid) {
        if (!messageParcel->WriteFileDescriptor(fd->Get())) {
            HILOGE("Failed to write main file descriptor: fd=%d", fd->Get());
            return false;
        }
    }

    int isManifestFdValid = (manifestFd != nullptr && manifestFd->Get() >= 0) ? 1 : 0;
    if (!messageParcel->WriteInt32(isManifestFdValid)) {
        HILOGE("Failed to write manifest file descriptor validity");
        return false;
    }
    if (isManifestFdValid) {
        if (!messageParcel->WriteFileDescriptor(manifestFd->Get())) {
            HILOGE("Failed to write manifest file descriptor: fd=%d", manifestFd->Get());
            return false;
        }
    }

    return true;
}

bool FileOpenResult::ReadFromParcel(Parcel &parcel)
{
    auto messageParcel = static_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        HILOGE("Failed to cast parcel to MessageParcel");
        return false;
    }

    if (!messageParcel->ReadInt32(errCode)) {
        HILOGE("Failed to read error code from parcel");
        return false;
    }

    int mainFdVal = -1;
    int isMainFdValid = 0;
    if (!messageParcel->ReadInt32(isMainFdValid)) {
        HILOGE("Failed to read main file descriptor validity");
        return false;
    }
    if (isMainFdValid) {
        mainFdVal = messageParcel->ReadFileDescriptor();
        if (mainFdVal < 0) {
            HILOGE("Failed to read main file descriptor from parcel");
            return false;
        }
        fd = std::make_shared<UniqueFd>(mainFdVal);
    } else {
        fd = nullptr;
    }

    int manifestFdVal = -1;
    int isManifestFdValid = 0;
    if (!messageParcel->ReadInt32(isManifestFdValid)) {
        HILOGE("Failed to read manifest file descriptor validity");
        return false;
    }
    if (isManifestFdValid) {
        manifestFdVal = messageParcel->ReadFileDescriptor();
        if (manifestFdVal < 0) {
            HILOGE("Failed to read manifest file descriptor from parcel");
            return false;
        }
        manifestFd = std::make_shared<UniqueFd>(manifestFdVal);
    } else {
        manifestFd = nullptr;
    }

    return true;
}

FileOpenResult *FileOpenResult::Unmarshalling(Parcel &parcel)
{
    try {
        auto result = make_unique<FileOpenResult>();
        if (!result->ReadFromParcel(parcel)) {
            return nullptr;
        }
        return result.release();
    } catch (const bad_alloc &e) {
        HILOGE("Failed to unmarshall FileOpenResult because of %{public}s", e.what());
    }
    return nullptr;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS