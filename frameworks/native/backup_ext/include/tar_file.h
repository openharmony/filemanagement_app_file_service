/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_BACKUP_TAR_FILE_H
#define OHOS_FILEMGMT_BACKUP_BACKUP_TAR_FILE_H

#include <map>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace OHOS::FileManagement::Backup {
namespace {
const uint32_t TMAGIC_LEN = 6;
const uint32_t TNAME_LEN = 100;
const uint32_t TMODE_LEN = 8;
const uint32_t TUID_LEN = 8;
const uint32_t TGID_LEN = 8;
const uint32_t TSIZE_LEN = 12;
const uint32_t MTIME_LEN = 12;
const uint32_t CHKSUM_LEN = 8;
const uint32_t VERSION_LEN = 2;
const uint32_t NAME_LEN = 32;
const uint32_t MAJOR_LEN = 8;
const uint32_t MINOR_LEN = 8;
const uint32_t PREFIX_LEN = 155;
const uint32_t PADDING_LEN = 12;
const uint32_t TMODE_BASE = 100;
const uint32_t TUID_BASE = 108;
const uint32_t TGID_BASE = 116;
const uint32_t TSIZE_BASE = 124;
const uint32_t TMTIME_BASE = 136;
const uint32_t CHKSUM_BASE = 148;
const uint32_t BLOCK_SIZE = 512;
const off_t READ_BUFF_SIZE = 512 * 1024;
const uint8_t BLANK_SPACE = 0x20;
const uint64_t MB_TO_BYTE = 1024 * 1024;
const std::string TMAGIC = "ustar";
const char REGTYPE = '0';   // regular file
const char AREGTYPE = '\0'; // regular file
const char SYMTYPE = '2';   // reserved
const char DIRTYPE = '5';   // directory
const char GNUTYPE_LONGNAME = 'L';
} // namespace

// 512 bytes
using TarHeader = struct {
    char name[TNAME_LEN];
    char mode[TMODE_LEN];
    char uid[TUID_LEN];
    char gid[TGID_LEN];
    char size[TSIZE_LEN];
    char mtime[MTIME_LEN];
    char chksum[CHKSUM_LEN];
    char typeFlag;
    char linkName[TNAME_LEN];
    char magic[TMAGIC_LEN];
    char version[VERSION_LEN];
    char uname[NAME_LEN];
    char gname[NAME_LEN];
    char devMajor[MAJOR_LEN];
    char devMinor[MINOR_LEN];
    char prefix[PREFIX_LEN];
    char pad[PADDING_LEN];
};
using TarMap = std::map<std::string, std::tuple<std::string, struct stat, bool>>;
class TarFile {
public:
    static TarFile &GetInstance();

    bool Packet(const std::vector<std::string> &srcFiles,
                const std::string &tarFileName,
                const std::string &pkPath,
                TarMap &tarMap);

private:
    TarFile() {}
    ~TarFile() = default;
    TarFile(const TarFile &instance) = delete;
    TarFile &operator=(const TarFile &instance) = delete;

    /**
     * @brief traversal file
     *
     * @param filename 文件名
     * @return true 遍历成功
     * @return false 遍历失败
     */
    bool TraversalFile(std::string &fileName);

    /**
     * @brief add files to the tar package
     *
     * @param filename 文件名
     * @param st 文件参数结构体
     * @param isSplit 是否分片
     */
    bool AddFile(std::string &fileName, const struct stat &st, bool isSplit);

    /**
     * @brief write files to content
     *
     * @param filename 文件名
     * @param size 文件大小
     */
    bool WriteFileContent(const std::string &fileName, off_t size);

    /**
     * @brief split write
     *
     * @param ioBuffer 写入的文件信息
     * @param read 读取文件
     * @param isFilled 是否写完
     */
    off_t SplitWriteAll(const std::vector<uint8_t> &ioBuffer, off_t read, bool &isFilled);

    /**
     * @brief creaat split tarfile
     */
    bool CreateSplitTarFile();

    /**
     * @brief complete block
     *
     * @param size 完成的块大小
     */
    bool CompleteBlock(off_t size);

    /**
     * @brief fill split tailblocks
     */
    bool FillSplitTailBlocks();

    /**
     * @brief set check sum
     *
     * @param hdr tar文件结构体
     */
    void SetCheckSum(TarHeader &hdr);

    /**
     * @brief fill owner name
     *
     * @param hdr tar文件结构体
     * @param st  文件结构体
     */
    void FillOwnerName(TarHeader &hdr, const struct stat &st);

    /**
     * @brief write long name
     *
     * @param name  文件名
     * @param type  文件类型
     */
    bool WriteLongName(std::string &name, char type);

    /**
     * @brief read files
     *
     * @param fd 文件描述符
     * @param iobuffer  文件信息数组
     * @param size  文件大小
     */
    off_t ReadAll(int fd, std::vector<uint8_t> &ioBuffer, off_t size);

    /**
     * @brief write files
     *
     * @param buffer 文件内容数组
     * @param len  长度
     */
    int WriteAll(const std::vector<uint8_t> &buffer, size_t len);

    /**
     * @brief write tar header to tar file
     *
     * @param header tar文件头结构体
     */
    int WriteTarHeader(TarHeader &header);

    /**
     * @brief Character conversion
     *
     * @param len  长度
     * @param val  需要转换的值
     */
    std::string I2Ocs(int len, off_t val);

    /**
     * @brief Character conversion
     *
     * @param st   文件信息结构体
     * @param hdr  tar包文件头
     */
    bool I2OcsConvert(const struct stat &st, TarHeader &hdr, std::string &fileName, bool isSplit);

private:
    uint32_t fileCount_ {0};
    TarMap tarMap_ {};

    std::string rootPath_ {};
    std::string packagePath_ {};
    std::string baseTarName_ {};
    std::string tarFileName_ {};

    std::vector<uint8_t> ioBuffer_ {};

    FILE *currentTarFile_ {nullptr};
    std::string currentTarName_ {};
    off_t currentTarFileSize_ {0};
    uint32_t tarFileCount_ {0};

    std::string currentFileName_ {};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_TAR_FILE_H