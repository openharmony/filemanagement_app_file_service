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

#ifndef OHOS_FILEMGMT_BACKUP_B_REPORT_ENTITY_H
#define OHOS_FILEMGMT_BACKUP_B_REPORT_ENTITY_H
#define FILE_DEFAULT_MODE "0660"

#include <fcntl.h>
#include <map>
#include <string>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
struct ReportFileInfo {
    std::string filePath;
    std::string mode {FILE_DEFAULT_MODE};
    bool isDir {false};
    off_t size {0};
    off_t mtime {0};
    std::string hash;
    bool isIncremental {false};
    off_t userTar {0};
    bool encodeFlag {false};
};

class BReportEntity {
public:
    /**
     * @brief 获取Report信息
     *
     * @return std::map<string, ReportFileInfo>
     */
    std::unordered_map<std::string, struct ReportFileInfo> GetReportInfos() const;

    /**
     * @brief 获取本地Report信息
     *
     * @return bool
     */
    bool GetStorageReportInfos(struct ReportFileInfo &fileStat);

    /**
     * @brief Check if line is encode
     *
     */
    void CheckAndUpdateIfReportLineEncoded(std::string &path);

    /**
     * @brief encode report item
     */
    static std::string EncodeReportItem(const std::string &reportItem, bool enableEncode);

    /**
     * @brief decode report item
     */
    static std::string DecodeReportItem(const std::string &reportItem, bool enableEncode);

public:
    /**
     * @brief 构造方法
     *
     * @param fd
     */
    explicit BReportEntity(UniqueFd fd) : srcFile_(std::move(fd)) {}

    BReportEntity() = delete;
    virtual ~BReportEntity() = default;

public:
    unsigned int attrNum = 0;

protected:
    UniqueFd srcFile_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_REPORT_ENTITY_H