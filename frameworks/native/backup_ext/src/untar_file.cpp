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

#include "untar_file.h"

#include <utime.h>

#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "securec.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const int32_t PATH_MAX_LEN = 2048;
const int32_t OCTAL = 8;

static bool IsEmptyBlock(const char *p)
{
    if (p != nullptr) {
        return (p[0] == '\0');
    }
    return true;
}

// 八进制字符串转十进制数字
static off_t ParseOctalStr(const string &octalStr, size_t destLen)
{
    off_t ret = 0;
    for (auto &ch : octalStr) {
        if (ch < '0' || ch > '7') {
            --destLen;
        }
    }
    for (auto &ch : octalStr) {
        if (ch >= '0' && ch <= '7' && destLen > 0) {
            ret *= OCTAL;
            ret += ch - '0';
            --destLen;
        }
    }
    return ret;
}

UntarFile &UntarFile::GetInstance()
{
    static UntarFile instance;
    return instance;
}

int UntarFile::UnPacket(const string &tarFile, const string &rootPath)
{
    tarFilePtr_ = fopen(tarFile.c_str(), "rb");
    if (tarFilePtr_ == nullptr) {
        HILOGE("Failed to open tar file %{public}s, err = %{public}d", tarFile.c_str(), errno);
        return errno;
    }

    if (ParseTarFile(rootPath) != 0) {
        HILOGE("Failed to parse tar file");
    }

    fclose(tarFilePtr_);
    tarFilePtr_ = nullptr;

    return 0;
}

void UntarFile::HandleTarBuffer(const string &buff, const string &name, FileStatInfo &info)
{
    info.mode = static_cast<mode_t>(ParseOctalStr(&buff[0] + TMODE_BASE, TMODE_LEN));
    info.uid = static_cast<uid_t>(ParseOctalStr(&buff[0] + TUID_BASE, TUID_LEN));
    info.gid = static_cast<gid_t>(ParseOctalStr(&buff[0] + TGID_BASE, TGID_LEN));
    info.mtime = ParseOctalStr(&buff[0] + TMTIME_BASE, MTIME_LEN);

    tarFileSize_ = ParseOctalStr(&buff[0] + TSIZE_BASE, TSIZE_LEN);
    tarFileBlockCnt_ = (tarFileSize_ + BLOCK_SIZE - 1) / BLOCK_SIZE;
    pos_ = ftello(tarFilePtr_);

    string realName = name;
    if (!info.longName.empty()) {
        realName = info.longName;
    }
    info.fullPath = GenRealPath(rootPath_, realName);
}

int UntarFile::ParseTarFile(const string &rootPath)
{
    // re-parse tar header
    rootPath_ = rootPath;
    char buff[BLOCK_SIZE] = {0};
    bool isSkip = false;
    FileStatInfo info {};

    // tarFileSize
    int ret = fseeko(tarFilePtr_, 0L, SEEK_END);
    tarFileSize_ = ftello(tarFilePtr_);
    // reback file to begin
    ret = fseeko(tarFilePtr_, 0L, SEEK_SET);

    while (1) {
        readCnt_ = fread(buff, 1, BLOCK_SIZE, tarFilePtr_);
        if (readCnt_ < BLOCK_SIZE) {
            HILOGE("Parsing tar file completed, read data count is less then block size.");
            return 0;
        }
        // two empty continuous block indicate end of file
        if (IsEmptyBlock(buff)) {
            char tailBuff[BLOCK_SIZE] = {0};
            size_t tailRead = fread(tailBuff, 1, BLOCK_SIZE, tarFilePtr_);
            if (tailRead == BLOCK_SIZE && IsEmptyBlock(tailBuff)) {
                HILOGE("Parsing tar file completed, tailBuff is empty.");
                return 0;
            }
        }
        // check header
        TarHeader *header = reinterpret_cast<TarHeader *>(buff);
        if (!IsValidTarBlock(*header)) {
            // when split unpack, ftell size is over than file really size [0,READ_BUFF_SIZE]
            if (ftello(tarFilePtr_) > (tarFileSize_ + READ_BUFF_SIZE) || !IsEmptyBlock(buff)) {
                HILOGE("Invalid tar file format");
                ret = ERR_FORMAT;
            }
            return ret;
        }
        HandleTarBuffer(string(buff, BLOCK_SIZE), header->name, info);
        ParseFileByTypeFlag(header->typeFlag, isSkip, info);
        ret = HandleFileProperties(isSkip, info);
        if (ret != 0) {
            HILOGE("Failed to handle file property");
        }
    }

    return ret;
}

void UntarFile::ParseFileByTypeFlag(char typeFlag, bool &isSkip, FileStatInfo &info)
{
    switch (typeFlag) {
        case REGTYPE:
        case AREGTYPE:
            ParseRegularFile(info, typeFlag, isSkip);
            break;
        case SYMTYPE:
            isSkip = false;
            break;
        case DIRTYPE:
            CreateDir(info.fullPath, info.mode);
            isSkip = false;
            break;
        case GNUTYPE_LONGNAME: {
            size_t nameLen = static_cast<size_t>(tarFileSize_);
            if (nameLen < PATH_MAX_LEN) {
                fread(&(info.longName[0]), sizeof(char), nameLen, tarFilePtr_);
            }
            isSkip = true;
            fseeko(tarFilePtr_, pos_ + tarFileBlockCnt_ * BLOCK_SIZE, SEEK_SET);
            break;
        }
        default: {
            // Ignoring, skip
            isSkip = true;
            fseeko(tarFilePtr_, tarFileBlockCnt_ * BLOCK_SIZE, SEEK_CUR);
            break;
        }
    }
}

void UntarFile::ParseRegularFile(FileStatInfo &info, char typeFlag, bool &isSkip)
{
    FILE *destFile = CreateFile(info.fullPath, info.mode, typeFlag);
    if (destFile != nullptr) {
        string destStr("");
        destStr.resize(READ_BUFF_SIZE);
        off_t remainSize = tarFileSize_;
        size_t readBuffSize = READ_BUFF_SIZE;
        while (remainSize > 0) {
            if (remainSize < READ_BUFF_SIZE) {
                readBuffSize = remainSize;
            }
            fread(&destStr[0], sizeof(char), readBuffSize, tarFilePtr_);
            fwrite(&destStr[0], sizeof(char), readBuffSize, destFile);
            remainSize -= readBuffSize;
        }
        fclose(destFile);
        chmod(info.fullPath.data(), info.mode);
        struct utimbuf times;
        times.modtime = info.mtime;
        if (utime(info.fullPath.c_str(), &times) != 0) {
            HILOGE("Failed to set mtime of %{public}s, err = %{public}d", info.fullPath.c_str(), errno);
        }
        // anyway, go to correct
        fseeko(tarFilePtr_, pos_ + tarFileBlockCnt_ * BLOCK_SIZE, SEEK_SET);
    } else {
        HILOGE("Failed to create file %{public}s, err = %{public}d", info.fullPath.c_str(), errno);
        fseeko(tarFilePtr_, tarFileBlockCnt_ * BLOCK_SIZE, SEEK_CUR);
    }
    isSkip = false;
}

int UntarFile::HandleFileProperties(bool &isSkip, FileStatInfo &info)
{
    if (isSkip) {
        isSkip = false;
        return 0;
    }
    int ret = lchown(info.fullPath.c_str(), info.uid, info.gid);
    if (ret != 0) {
        HILOGE("Failed to lchown %{public}s, err = %{public}d", info.fullPath.c_str(), errno);
    }

    return ret;
}

bool UntarFile::VerifyChecksum(TarHeader &header)
{
    vector<uint8_t> buffer {};
    buffer.resize(sizeof(header));
    buffer.assign(reinterpret_cast<uint8_t *>(&header), reinterpret_cast<uint8_t *>(&header) + sizeof(header));
    int sum = 0;
    for (uint32_t index = 0; index < BLOCK_SIZE; ++index) {
        if (index < CHKSUM_BASE || index > CHKSUM_BASE + CHKSUM_LEN - 1) {
            // Standard tar checksum adds unsigned bytes.
            sum += (buffer[index] & 0xFF);
        } else {
            sum += BLANK_SPACE;
        }
    }
    string strChksum;
    strChksum.assign(buffer.begin(), buffer.end());
    return (sum == ParseOctalStr(&strChksum[0] + CHKSUM_BASE, CHKSUM_LEN));
}

bool UntarFile::IsValidTarBlock(TarHeader &header)
{
    // check magic && checksum
    if (strncmp(header.magic, TMAGIC.c_str(), TMAGIC_LEN - 1) == 0 && VerifyChecksum(header)) {
        return true;
    }
    HILOGE("Invalid tar block");
    return false;
}

string UntarFile::GenRealPath(const string &rootPath, const string &realName)
{
    if (rootPath.empty() || realName.empty()) {
        return "";
    }
    string realPath(rootPath);
    size_t len = realPath.length();
    if (realPath[len - 1] == '/') {
        realPath = realPath.substr(0, len - 1);
    }
    realPath.append((realName[0] == '/') ? realName : ("/" + realName));
    return realPath;
}

void UntarFile::CreateDir(string &path, mode_t mode)
{
    size_t len = path.length();
    if (path[len - 1] == '/') {
        path[len - 1] = '\0';
    }
    if (access(path.c_str(), F_OK) != 0) {
        HILOGE("%{public}s does not exist, err = %{public}d", path.c_str(), errno);
        if (mkdir(path.c_str(), mode) != 0) {
            HILOGE("Failed to mkdir %{public}s, err = %{public}d", path.c_str(), errno);
        }
    }
}

FILE *UntarFile::CreateFile(string &filePath, mode_t mode, char fileType)
{
    FILE *f = fopen(filePath.c_str(), "wb+");
    if (f != nullptr) {
        return f;
    }

    HILOGE("Failed to open file %{public}s, err = %{public}d", filePath.c_str(), errno);
    size_t pos = filePath.rfind('/');
    if (pos == string::npos) {
        return nullptr;
    }

    string path = filePath.substr(0, pos);
    if (ForceCreateDirectory(path)) {
        f = fopen(filePath.c_str(), "wb+");
        if (f == nullptr) {
            HILOGE("Failed to open file %{public}s, err = %{public}d", filePath.c_str(), errno);
        }
    }

    return f;
}

} // namespace OHOS::FileManagement::Backup