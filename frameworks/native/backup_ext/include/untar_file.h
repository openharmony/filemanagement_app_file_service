/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include <utime.h>

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

using ErrFileInfo = std::map<std::string, std::vector<int>>;
using EndFileInfo = std::map<std::string, off_t>;

const int FIRST_PARAM = 0;
const int SECOND_PARAM = 1;
const int THIRD_PARAM = 2;
class UntarFile {
public:
    typedef enum { ERR_FORMAT = -1 } ErrorCode;
    static UntarFile &GetInstance();
    std::tuple<int, EndFileInfo, ErrFileInfo> UnPacket(
        const std::string &tarFile, const std::string &rootPath);
    std::tuple<int, EndFileInfo, ErrFileInfo> IncrementalUnPacket(
        const std::string &tarFile, const std::string &rootPath,
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
    std::tuple<int, EndFileInfo, ErrFileInfo> ParseTarFile(const std::string &rootPath);

    /**
     * @brief parse incremental tar file
     *
     * @param rootpath 解包的目标路径
     */
    std::tuple<int, EndFileInfo, ErrFileInfo> ParseIncrementalTarFile(const std::string &rootPath);

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
    ErrFileInfo CreateDir(std::string &path, mode_t mode);

    /**
     * @brief creat a file
     *
     * @param path 文件路径名
     */
    FILE *CreateFile(std::string &path);

    /**
     * @brief parse regular file
     *
     * @param info 文件属性结构体
     * @param typeFlag 文件类型标志
     */
    ErrFileInfo ParseRegularFile(FileStatInfo &info);

    /**
     * @brief handle tar buffer
     *
     * @param buff 读取tar文件数据缓冲区
     * @param name 文件名
     * @param info 文件属性结构体
     */
    off_t HandleTarBuffer(const std::string &buff, const std::string &name, FileStatInfo &info);

    /**
     * @brief parse file by typeFlag
     *
     * @param typeFlag 文件类型标志
     * @param info 文件属性结构体
     */
    std::tuple<int, bool, ErrFileInfo> ParseFileByTypeFlag(char typeFlag, FileStatInfo &info);

    /**
     * @brief parse incremental file by typeFlag
     *
     * @param typeFlag 文件类型标志
     * @param info 文件属性结构体
     */
    std::tuple<int, bool, ErrFileInfo> ParseIncrementalFileByTypeFlag(char typeFlag, FileStatInfo &info);

    /**
     * @brief Check tar and fill tar size
     */
    int CheckAndFillTarSize();

    /**
     * @brief Read long name and recode errinfo
     *
     * @param info file state info
     */
    std::tuple<int, ErrFileInfo> ReadLongName(FileStatInfo &info);

    /**
     * @brief deal parse tar file result
     *
     * @param result parse tar file result info
     * @param fileSize size of the file in tar
     * @param fileName file state info
     * @param fileInfos out param, record file info
     * @param errInfos out param, record err file info
     */
    int DealParseTarFileResult(const std::tuple<int, bool, ErrFileInfo> &result,
        const off_t fileSize, const std::string &fileName, EndFileInfo &fileInfos, ErrFileInfo &errInfos);

    /**
     * @brief deal incremental parse tar file result
     *
     * @param result parse tar file result info
     * @param fileSize size of the file in tar
     * @param fileName file state info
     * @param fileInfos out param, record file info
     * @param errInfos out param, record err file info
     */
    int DealIncreParseTarFileResult(const std::tuple<int, bool, ErrFileInfo> &result,
        const off_t fileSize, const std::string &fileName, EndFileInfo &fileInfos, ErrFileInfo &errInfos);

    /**
     * @brief check if tar block valid
     *
     * @param buff check buff info
     * @param header check header info
     * @param ret out param, the err info
     */
    bool CheckIfTarBlockValid(char *buff, size_t buffLen, TarHeader *header, int &ret);

    /**
     * @brief deal file tag info
     *
     * @param errFileInfo out param, err file info
     * @param info out param, file info
     * @param isFilter out param, is Filter
     * @param tmpFullPath in param, tmpFullPath
     */
    bool DealFileTag(ErrFileInfo &errFileInfo,
        FileStatInfo &info, bool &isFilter, const std::string &tmpFullPath);

    std::tuple<int, std::string> ParsePaxBlock();

    void CheckLongName(std::string longName, FileStatInfo &info);

    std::tuple<int, std::string> GetLongName(uint32_t recLen, uint32_t allLen);

    std::tuple<int, bool, ErrFileInfo> MatchIncrementalScenario(bool isFilter, ErrFileInfo &errFileInfo,
        std::string tmpFullPath, char typeFlag, FileStatInfo &info);

    void MatchAregType(bool &isRightRes, FileStatInfo &info, ErrFileInfo &errFileInfo, bool &isFilter);

    void MatchDirType(bool &isRightRes, FileStatInfo &info, ErrFileInfo &errFileInfo, bool &isFilter);

    void MatchGnuTypeLongName(bool &isRightRes, FileStatInfo &info, ErrFileInfo &errFileInfo, bool &isFilter);

    void MatchExtHeader(bool &isRightRes, FileStatInfo &info, bool &isFilter);

    void MatchDefault(bool &isRightRes, FileStatInfo &info);

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