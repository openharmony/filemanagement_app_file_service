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

#ifndef OHOS_FILEMGMT_BACKUP_B_FILE_INFO_H
#define OHOS_FILEMGMT_BACKUP_B_FILE_INFO_H

#include <memory>
#include <string>

#include "parcel.h"

namespace OHOS::FileManagement::Backup {
using BundleName = std::string;
using TmpFileSN = uint32_t;

struct BFileInfo : public Parcelable {
    BundleName owner;
    std::string fileName;
    TmpFileSN sn; // 用于服务零拷贝接收文件场景

    BFileInfo() = default;
    BFileInfo(std::string bundleName, std::string strFileNanme, TmpFileSN id)
        : owner(bundleName), fileName(strFileNanme), sn(id) {}
    ~BFileInfo() override = default;

    bool ReadFromParcel(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
    static BFileInfo *Unmarshalling(Parcel &parcel);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_FILE_INFO_H
