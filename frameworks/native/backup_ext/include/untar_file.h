/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_BACKUP_UNTAR_FILE_H
#define OHOS_FILEMGMT_BACKUP_BACKUP_UNTAR_FILE_H

#include "tar_file.h"
#include "b_json/b_report_entity.h"

namespace OHOS::FileManagement::Backup {
struct FileStatInfo {
    std::string fullPath {};
    mode_t mode {0};
    uid_t uid {0};
    gid_t gid {0};
    off_t mtime {0};
    std::string longName {};
};
class UntarFile {
public:
    typedef enum { ERR_FORMAT = -1 } ErrorCode;
    static UntarFile &GetInstance();
    int UnPacket(const std::string &tarFile, const std::string &rootPath);
    int IncrementalUnPacket(const std::string &tarFile, const std::string &rootPath,
                            const std::unordered_map<std::string, struct ReportFileInfo> &includes);

private:
    UntarFile() = default;
    ~UntarFile() = default;
    UntarFile(const UntarFile &instance) = delete;
    UntarFile &operator=(const UntarFile &instance) = delete;

    /**
     * @brief parse tar file
     *
     * @param rootpath 解包的目标路径
     */
    int ParseTarFile(const std::string &rootPath);

    /**
     * @brief parse incremental tar file
     *
     * @param rootpath 解包的目标路径
     */
    int ParseIncrementalTarFile(const std::string &rootPath);

    /**
     * @brief verfy check sum
     *
     * @param tarHeader tar文件头
     */
    bool VerifyChecksum(TarHeader &tarHeader);

    /**
     * @brief is valid tarblock
     *
     * @param tarHeader tar文件头
     */
    bool IsValidTarBlock(TarHeader &tarHeader);

    /**
     * @brief get real path
     *
     * @param rootpath 解包的目标路径
     * @param pathName 文件路径名
     */
    std::string GenRealPath(const std::string &rootPath, const std::string &pathName);

    /**
     * @brief creat dir
     *
     * @param path 文件路径名
     * @param mode 文件打开模式
     */
    void CreateDir(std::string &path, mode_t mode);

    /**
     * @brief creat a file
     *
     * @param path 文件路径名
     * @param mode 文件打开模式
     * @param fileType 文件类型
     */
    FILE *CreateFile(std::string &path, mode_t mode, char fileType);

    /**
     * @brief parse regular file
     *
     * @param info 文件属性结构体
     * @param typeFlag 文件类型标志
     */
    void ParseRegularFile(FileStatInfo &info, char typeFlag);

    /**
     * @brief handle tar buffer
     *
     * @param buff 读取tar文件数据缓冲区
     * @param name 文件名
     * @param info 文件属性结构体
     */
    void HandleTarBuffer(const std::string &buff, const std::string &name, FileStatInfo &info);

    /**
     * @brief parse file by typeFlag
     *
     * @param typeFlag 文件类型标志
     * @param info 文件属性结构体
     */
    void ParseFileByTypeFlag(char typeFlag, FileStatInfo &info);

    /**
     * @brief parse incremental file by typeFlag
     *
     * @param typeFlag 文件类型标志
     * @param info 文件属性结构体
     */
    int ParseIncrementalFileByTypeFlag(char typeFlag, FileStatInfo &info);

private:
    std::string rootPath_ {};

    FILE *tarFilePtr_ {nullptr};
    off_t tarFileSize_ {0};
    off_t tarFileBlockCnt_ {0};
    off_t pos_ {0};
    size_t readCnt_ {0};
    std::unordered_map<std::string, struct ReportFileInfo> includes_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_UNTAR_FILE_H