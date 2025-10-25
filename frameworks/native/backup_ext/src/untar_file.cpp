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

#include "untar_file.h"

#include <utime.h>

#include "b_anony/b_anony.h"
#include "b_filesystem/b_dir.h"
#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "securec.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const int32_t PATH_MAX_LEN = 4096;
const int32_t OCTAL = 8;
const int DEFAULT_ERR = -1;

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
    string::const_iterator it = octalStr.begin();

    while (it != octalStr.end() && (*it < '0' || *it > '7') && destLen > 0) {
        ++it;
        --destLen;
    }

    while (it != octalStr.end() && *it >= '0' && *it <= '7' && destLen > 0) {
        ret *= OCTAL;
        ret += *it - '0';
        ++it;
        --destLen;
    }

    return ret;
}

static bool ForceCreateDirectoryWithMode(const string& path, mode_t mode)
{
    string::size_type index = 0;
    do {
        index = path.find('/', index + 1);
        string subPath = (index == string::npos) ? path : path.substr(0, index);
        if (access(subPath.c_str(), F_OK) != 0) {
            if (mkdir(subPath.c_str(), mode) != 0) {
                return false;
            }
        }
    } while (index != string::npos);
    return access(path.c_str(), F_OK) == 0;
}

static void RTrimNull(std::string &s)
{
    auto iter = std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return ch != '\0'; });
    s.erase(iter.base(), s.end());
}

std::tuple<int, ErrFileInfo> UntarFile::ReadLongName(FileStatInfo &info)
{
    size_t nameLen = static_cast<size_t>(tarFileSize_);
    int ret = 0;
    if (nameLen <= PATH_MAX_LEN) {
        string tempName("");
        tempName.resize(nameLen);
        size_t read = fread(&(tempName[0]), sizeof(char), nameLen, tarFilePtr_);
        if (read < nameLen) {
            HILOGE("Failed to fread longName of %{private}s", info.fullPath.c_str());
            ret = -1;
        }
        info.longName = tempName;
    } else {
        HILOGE("longName of %{private}s exceed PATH_MAX_LEN", info.fullPath.c_str());
        ret = -1;
    }
    ErrFileInfo errFileInfo;
    if (ret != 0) {
        errFileInfo[info.fullPath].emplace_back(ret);
        return {-1, errFileInfo};
    }
    if (fseeko(tarFilePtr_, pos_ + tarFileBlockCnt_ * BLOCK_SIZE, SEEK_SET) != 0) {
        HILOGE("Failed to fseeko of %{private}s, err = %{public}d", info.fullPath.c_str(), errno);
        errFileInfo[info.fullPath].emplace_back(errno);
        return {-1, errFileInfo};
    }
    return {0, errFileInfo};
}

UntarFile &UntarFile::GetInstance()
{
    static UntarFile instance;
    return instance;
}

std::tuple<int, EndFileInfo, ErrFileInfo> UntarFile::UnPacket(
    const std::string &tarFile, const std::string &rootPath)
{
    tarFilePtr_ = fopen(tarFile.c_str(), "rb");
    if (tarFilePtr_ == nullptr) {
        HILOGE("Failed to open tar file %{public}s, err = %{public}d", tarFile.c_str(), errno);
        return {errno, {}, {}};
    }

    auto [ret, fileInfos, errInfos] = ParseTarFile(rootPath);
    if (ret != 0) {
        HILOGE("Failed to parse tar file");
    }

    fclose(tarFilePtr_);
    tarFilePtr_ = nullptr;

    return {0, fileInfos, errInfos};
}

std::tuple<int, EndFileInfo, ErrFileInfo> UntarFile::IncrementalUnPacket(
    const string &tarFile, const string &rootPath, const unordered_map<string, struct ReportFileInfo> &includes)
{
    includes_ = includes;
    tarFilePtr_ = fopen(tarFile.c_str(), "rb");
    if (tarFilePtr_ == nullptr) {
        HILOGE("Failed to open tar file %{public}s, err = %{public}d", tarFile.c_str(), errno);
        return {errno, {}, {}};
    }

    auto [ret, fileInfos, errFileInfos] = ParseIncrementalTarFile(rootPath);
    if (ret != 0) {
        HILOGE("Failed to parse tar file");
    }

    fclose(tarFilePtr_);
    tarFilePtr_ = nullptr;

    return {0, fileInfos, errFileInfos};
}

off_t UntarFile::HandleTarBuffer(const string &buff, const string &name, FileStatInfo &info)
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
        info.longName.clear();
    }
    if (realName.length() > 0 && realName[0] == '/') {
        info.fullPath = realName.substr(1, realName.length() - 1);
        return tarFileSize_;
    }
    info.fullPath = realName;
    return tarFileSize_;
}

int UntarFile::CheckAndFillTarSize()
{
    // tarFileSize
    int ret = fseeko(tarFilePtr_, 0L, SEEK_END);
    if (ret != 0) {
        HILOGE("Failed to fseeko tarFileSize SEEK_SET, err = %{public}d", errno);
        return ret;
    }
    tarFileSize_ = ftello(tarFilePtr_);
    // reback file to begin
    ret = fseeko(tarFilePtr_, 0L, SEEK_SET);
    if (ret != 0) {
        HILOGE("Failed to fseeko reback SEEK_SET, err = %{public}d", errno);
        return ret;
    }
    return ret;
}

int UntarFile::DealParseTarFileResult(const std::tuple<int, bool, ErrFileInfo> &result,
    const off_t fileSize, const std::string &fileName, EndFileInfo &fileInfos, ErrFileInfo &errInfos)
{
    auto [ret, isFilter, subErrInfos] = result;
    if (ret != 0) {
        HILOGE("Failed to parse incremental file by type flag");
        return ret;
    }
    if (!isFilter) {
        fileInfos[fileName] = fileSize;
    }
    if (!errInfos.empty()) {
        errInfos.merge(subErrInfos);
    }
    return 0;
}

bool UntarFile::CheckIfTarBlockValid(char *buff, size_t buffLen, TarHeader *header, int &ret)
{
    // two empty continuous block indicate end of file
    if (buff == nullptr || buffLen != BLOCK_SIZE || header == nullptr) {
        return false;
    }
    if (IsEmptyBlock(buff) && header->typeFlag != GNUTYPE_LONGNAME) {
        char tailBuff[BLOCK_SIZE] = {0};
        size_t tailRead = fread(tailBuff, 1, BLOCK_SIZE, tarFilePtr_);
        if (tailRead == BLOCK_SIZE && IsEmptyBlock(tailBuff)) {
            HILOGE("Parsing tar file completed, tailBuff is empty.");
            ret = 0;
            return false;
        }
    }
    // check header
    if (!IsValidTarBlock(*header)) {
        // when split unpack, ftell size is over than file really size [0,READ_BUFF_SIZE]
        if (ftello(tarFilePtr_) > (tarFileSize_ + READ_BUFF_SIZE) || !IsEmptyBlock(buff)) {
            HILOGE("Invalid tar file format");
            ret = ERR_FORMAT;
        }
        HILOGE("invalid tar block header");
        return false;
    }
    return true;
}

std::tuple<int, EndFileInfo, ErrFileInfo> UntarFile::ParseTarFile(const string &rootPath)
{
    // re-parse tar header
    rootPath_ = rootPath;
    char buff[BLOCK_SIZE] = {0};
    FileStatInfo info {};
    int ret = 0;
    if ((ret = CheckAndFillTarSize()) != 0) {
        return {ret, {}, {}};
    }
    EndFileInfo fileInfos;
    ErrFileInfo errInfos;
    while (1) {
        readCnt_ = fread(buff, 1, BLOCK_SIZE, tarFilePtr_);
        if (readCnt_ < BLOCK_SIZE) {
            HILOGE("Parsing tar file completed, read data count is less then block size.");
            return {0, fileInfos, errInfos};
        }
        TarHeader *header = reinterpret_cast<TarHeader *>(buff);
        bool isValid = CheckIfTarBlockValid(buff, sizeof(buff), header, ret);
        if (!isValid) {
            return {ret, fileInfos, errInfos};
        }
        off_t fileSize = HandleTarBuffer(string(buff, BLOCK_SIZE), header->name, info);
        auto result = ParseFileByTypeFlag(header->typeFlag, info);
        if ((ret = DealParseTarFileResult(result, fileSize, info.fullPath, fileInfos, errInfos)) != 0) {
            return {ret, fileInfos, errInfos};
        }
    }

    return {ret, fileInfos, errInfos};
}

int UntarFile::DealIncreParseTarFileResult(const std::tuple<int, bool, ErrFileInfo> &result,
    const off_t fileSize, const std::string &fileName, EndFileInfo &fileInfos, ErrFileInfo &errInfos)
{
    auto [ret, isFilter, subErrInfo] = result;
    if (ret != 0) {
        HILOGE("Failed to parse incremental file by type flag");
        return ret;
    }
    if (!isFilter) {
        fileInfos[fileName] = fileSize;
    }
    if (!subErrInfo.empty()) {
        errInfos.merge(subErrInfo);
    }
    return 0;
}

std::tuple<int, EndFileInfo, ErrFileInfo> UntarFile::ParseIncrementalTarFile(const string &rootPath)
{
    // re-parse tar header
    rootPath_ = rootPath;
    char buff[BLOCK_SIZE] = {0};
    FileStatInfo info {};
    int ret = 0;
    if ((ret = CheckAndFillTarSize()) != 0) {
        return {ret, {}, {}};
    }
    EndFileInfo fileInfos;
    ErrFileInfo errFileInfo;
    do {
        readCnt_ = fread(buff, 1, BLOCK_SIZE, tarFilePtr_);
        if (readCnt_ < BLOCK_SIZE) {
            HILOGE("Parsing tar file completed, read data count is less then block size.");
            return {0, fileInfos, errFileInfo};
        }
        TarHeader *header = reinterpret_cast<TarHeader *>(buff);
        bool isValid = CheckIfTarBlockValid(buff, sizeof(buff), header, ret);
        if (!isValid) {
            return {ret, fileInfos, errFileInfo};
        }
        off_t fileSize = HandleTarBuffer(string(buff, BLOCK_SIZE), header->name, info);
        auto result = ParseIncrementalFileByTypeFlag(header->typeFlag, info);
        ret = DealIncreParseTarFileResult(result, fileSize, info.fullPath, fileInfos, errFileInfo);
        if (ret != 0) {
            return {ret, fileInfos, errFileInfo};
        }
    } while (readCnt_ >= BLOCK_SIZE);

    return {ret, fileInfos, errFileInfo};
}

void UntarFile::MatchAregType(bool &isRightRes, FileStatInfo &info, ErrFileInfo &errFileInfo, bool &isFilter)
{
    info.fullPath = GenRealPath(rootPath_, info.fullPath);
    if (!BDir::IsFilePathValid(info.fullPath)) {
        HILOGE("Check file path : %{public}s err, path is forbidden", GetAnonyPath(info.fullPath).c_str());
        isRightRes = false;
        return;
    }
    errFileInfo = ParseRegularFile(info);
    isFilter = false;
}

void UntarFile::MatchDirType(bool &isRightRes, FileStatInfo &info, ErrFileInfo &errFileInfo, bool &isFilter)
{
    info.fullPath = GenRealPath(rootPath_, info.fullPath);
    if (!BDir::IsFilePathValid(info.fullPath)) {
        HILOGE("Check file path : %{public}s err, path is forbidden", GetAnonyPath(info.fullPath).c_str());
        isRightRes = false;
        return;
    }
    errFileInfo = CreateDir(info.fullPath, info.mode);
    isFilter = false;
}

void UntarFile::MatchGnuTypeLongName(bool &isRightRes, FileStatInfo &info, ErrFileInfo &errFileInfo, bool &isFilter)
{
    auto result = ReadLongName(info);
    if (!BDir::IsFilePathValid(info.fullPath) || !BDir::IsFilePathValid(info.longName)) {
        HILOGE("Check file path : %{public}s or long name : %{public}s err, path is forbidden",
            GetAnonyPath(info.fullPath).c_str(), GetAnonyPath(info.longName).c_str());
        isRightRes = false;
        return;
    }
    errFileInfo = std::get<SECOND_PARAM>(result);
    isFilter = true;
    isRightRes = (std::get<FIRST_PARAM>(result) == ERR_OK) ? true : false;
}

void UntarFile::MatchExtHeader(bool &isRightRes, FileStatInfo &info, bool &isFilter)
{
    auto [err, LongName] = ParsePaxBlock();
    if (err != ERR_OK) {
        isRightRes = false;
        return;
    }
    CheckLongName(LongName, info);
    isFilter = true;
}

void UntarFile::MatchDefault(bool &isRightRes, FileStatInfo &info)
{
    if (fseeko(tarFilePtr_, tarFileBlockCnt_ * BLOCK_SIZE, SEEK_CUR) != 0) {
        HILOGE("Failed to fseeko of %{private}s, err = %{public}d", info.fullPath.c_str(), errno);
        isRightRes = false;
    }
}

tuple<int, bool, ErrFileInfo> UntarFile::ParseFileByTypeFlag(char typeFlag, FileStatInfo &info)
{
    HILOGD("untar file: %{public}s, rootPath: %{public}s", GetAnonyPath(info.fullPath).c_str(), rootPath_.c_str());
    bool isFilter = true;
    ErrFileInfo errFileInfo;
    bool isRightRes = true;
    switch (typeFlag) {
        case REGTYPE:
        case AREGTYPE:
            MatchAregType(isRightRes, info, errFileInfo, isFilter);
            break;
        case SYMTYPE:
            break;
        case DIRTYPE:
            MatchDirType(isRightRes, info, errFileInfo, isFilter);
            break;
        case GNUTYPE_LONGNAME:
            MatchGnuTypeLongName(isRightRes, info, errFileInfo, isFilter);
            break;
        case EXTENSION_HEADER:
            MatchExtHeader(isRightRes, info, isFilter);
            break;
        default:
            MatchDefault(isRightRes, info);
    }
    if (!isRightRes) {
        return {DEFAULT_ERR, true, {{info.fullPath, {DEFAULT_ERR}}}};
    }
    return {0, isFilter, errFileInfo};
}

bool UntarFile::DealFileTag(ErrFileInfo &errFileInfo,
    FileStatInfo &info, bool &isFilter, const std::string &tmpFullPath)
{
    if (!includes_.empty() && includes_.find(tmpFullPath) == includes_.end()) { // not in includes
        if (fseeko(tarFilePtr_, pos_ + tarFileBlockCnt_ * BLOCK_SIZE, SEEK_SET) != 0) {
            HILOGE("Failed to fseeko of %{private}s, err = %{public}d", info.fullPath.c_str(), errno);
            errFileInfo[info.fullPath].emplace_back(DEFAULT_ERR);
            return false;
        }
        isFilter = true;
        return true;
    }
    info.fullPath = GenRealPath(rootPath_, info.fullPath);
    if (!BDir::IsFilePathValid(info.fullPath)) {
        HILOGE("Check file path : %{public}s err, path is forbidden", GetAnonyPath(info.fullPath).c_str());
        errFileInfo[info.fullPath].emplace_back(DEFAULT_ERR);
        return false;
    }
    errFileInfo = ParseRegularFile(info);
    isFilter = false;
    return true;
}

std::tuple<int, bool, ErrFileInfo> UntarFile::MatchIncrementalScenario(bool isFilter, ErrFileInfo &errFileInfo,
    string tmpFullPath, char typeFlag, FileStatInfo &info)
{
    switch (typeFlag) {
        case REGTYPE:
        case AREGTYPE: {
            if (!DealFileTag(errFileInfo, info, isFilter, tmpFullPath)) {
                return {DEFAULT_ERR, true, errFileInfo};
            }
            break;
        }
        case SYMTYPE:
            break;
        case DIRTYPE:
            info.fullPath = GenRealPath(rootPath_, info.fullPath);
            if (!BDir::IsFilePathValid(info.fullPath)) {
                HILOGE("Check file path : %{public}s err, path is forbidden", GetAnonyPath(info.fullPath).c_str());
                return {DEFAULT_ERR, true, {{info.fullPath, {DEFAULT_ERR}}}};
            }
            errFileInfo = CreateDir(info.fullPath, info.mode);
            isFilter = false;
            break;
        case GNUTYPE_LONGNAME: {
            auto result = ReadLongName(info);
            if (!BDir::IsFilePathValid(info.fullPath)) {
                HILOGE("Check file path : %{public}s err, path is forbidden", GetAnonyPath(info.fullPath).c_str());
                return {DEFAULT_ERR, true, {{info.fullPath, {DEFAULT_ERR}}}};
            }
            return {std::get<FIRST_PARAM>(result), isFilter, std::get<SECOND_PARAM>(result)};
            break;
        }
        case EXTENSION_HEADER: { // pax x header
            auto [err, LongName] = ParsePaxBlock();
            if (err == ERR_OK) {
                CheckLongName(LongName, info);
                return {err, true, errFileInfo};
            }
            return {err, isFilter, {{info.fullPath, {DEFAULT_ERR}}}};
            break;
        }
        default: {
            if (fseeko(tarFilePtr_, tarFileBlockCnt_ * BLOCK_SIZE, SEEK_CUR) != 0) {
                HILOGE("Failed to fseeko of %{private}s, err = %{public}d", info.fullPath.c_str(), errno);
                return {DEFAULT_ERR, true, {{info.fullPath, {DEFAULT_ERR}}}};
            }
            break;
        }
    }
    return {0, isFilter, errFileInfo};
}

std::tuple<int, bool, ErrFileInfo> UntarFile::ParseIncrementalFileByTypeFlag(char typeFlag, FileStatInfo &info)
{
    HILOGD("untar file: %{public}s, rootPath: %{public}s", GetAnonyPath(info.fullPath).c_str(), rootPath_.c_str());
    string tmpFullPath = info.fullPath;
    bool isFilter = true;
    ErrFileInfo errFileInfo;
    RTrimNull(tmpFullPath);
    return MatchIncrementalScenario(isFilter, errFileInfo, tmpFullPath, typeFlag, info);
}

ErrFileInfo UntarFile::ParseRegularFile(FileStatInfo &info)
{
    ErrFileInfo errFileInfo;
    FILE *destFile = CreateFile(info.fullPath);
    if (destFile != nullptr) {
        string destStr("");
        destStr.resize(READ_BUFF_SIZE);
        size_t remainSize = static_cast<size_t>(tarFileSize_);
        size_t readBuffSize = READ_BUFF_SIZE;
        while (remainSize > 0) {
            if (remainSize < READ_BUFF_SIZE) {
                readBuffSize = remainSize;
            }
            auto readSize = fread(&destStr[0], sizeof(char), readBuffSize, tarFilePtr_);
            if (readSize != readBuffSize) {
                readBuffSize = readSize;
            }
            size_t writeSize = 0;
            do {
                writeSize += fwrite(&destStr[writeSize], sizeof(char), readBuffSize - writeSize, destFile);
            } while (writeSize < readBuffSize);
            remainSize -= readBuffSize;
        }
        fclose(destFile);
        if (chmod(info.fullPath.data(), info.mode) != 0) {
            HILOGE("Failed to chmod of %{public}s, err = %{public}d", GetAnonyPath(info.fullPath).c_str(), errno);
            errFileInfo[info.fullPath].emplace_back(errno);
        }
        struct utimbuf times;
        struct stat attr;
        if (stat(info.fullPath.c_str(), &attr) != 0) {
            errFileInfo[info.fullPath].emplace_back(errno);
            HILOGE("Failed to get stat of %{public}s, err = %{public}d", GetAnonyPath(info.fullPath).c_str(), errno);
            times.actime = info.mtime;
        } else {
            times.actime = attr.st_atime;
        }
        times.modtime = info.mtime;
        if (info.mtime != 0 && utime(info.fullPath.c_str(), &times) != 0) {
            errFileInfo[info.fullPath].emplace_back(errno);
            HILOGE("Failed to set mtime of %{public}s, err = %{public}d", GetAnonyPath(info.fullPath).c_str(), errno);
        }
        // anyway, go to correct
        fseeko(tarFilePtr_, pos_ + tarFileBlockCnt_ * BLOCK_SIZE, SEEK_SET);
    } else {
        HILOGE("Failed to create file %{public}s, err = %{public}d", GetAnonyPath(info.fullPath).c_str(), errno);
        errFileInfo[info.fullPath].emplace_back(errno);
        fseeko(tarFilePtr_, tarFileBlockCnt_ * BLOCK_SIZE, SEEK_CUR);
    }
    return errFileInfo;
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
    if (realPath[0] == '/') {
        realPath = realPath.substr(1, realPath.length());
    }
    return realPath;
}

ErrFileInfo UntarFile::CreateDir(string &path, mode_t mode)
{
    ErrFileInfo errFileInfo;
    if (path.empty()) {
        return errFileInfo;
    }
    size_t len = path.length();
    if (path[len - 1] == '/') {
        path[len - 1] = '\0';
    }
    if (access(path.c_str(), F_OK) != 0) {
        HILOGD("directory does not exist, path:%{public}s, err = %{public}d", GetAnonyPath(path).c_str(), errno);
        if (!ForceCreateDirectoryWithMode(path, mode)) {
            HILOGD("Failed to force create directory %{public}s, err = %{public}d", GetAnonyPath(path).c_str(), errno);
            errFileInfo[path].emplace_back(errno);
        }
    }
    return errFileInfo;
}

FILE *UntarFile::CreateFile(string &filePath)
{
    FILE *f = nullptr;
    char rpath[PATH_MAX] = {0};
    if (realpath(filePath.c_str(), rpath)) {
        f = fopen(filePath.c_str(), "wb+");
        if (f == nullptr) {
            HILOGE("Failed to create file %{public}s, err = %{public}d", GetAnonyPath(filePath).c_str(), errno);
        }
        return f;
    }

    uint32_t len = filePath.length();
    HILOGD("Failed to open file %{public}d, %{public}s, err = %{public}d, Will create", len,
        GetAnonyPath(filePath).c_str(), errno);
    size_t pos = filePath.rfind('/');
    if (pos == string::npos) {
        return nullptr;
    }

    string path = filePath.substr(0, pos);
    if (ForceCreateDirectory(path)) {
        if (!realpath(path.c_str(), rpath)) {
            HILOGE("Failed to access path %{public}s, err = %{public}d", GetAnonyPath(path).c_str(), errno);
            return nullptr;
        }
        f = fopen(filePath.c_str(), "wb+");
        if (f == nullptr) {
            HILOGE("Failed to open file %{public}s, err = %{public}d", GetAnonyPath(filePath).c_str(), errno);
        }
    }

    return f;
}

std::tuple<int, std::string> UntarFile::ParsePaxBlock()
{
    int err = DEFAULT_ERR;
    char block[BLOCK_SIZE] = {0};
    auto readCnt = fread(block, 1, BLOCK_SIZE, tarFilePtr_);
    if (readCnt < BLOCK_SIZE) {
        HILOGE("Parsing tar file completed, read data count is less then block size.");
        return {err, ""};
    }
    string content(block, BLOCK_SIZE);
    string longName = "";
    size_t pos = 0;
    uint32_t recLen = 0;
    uint32_t allLen = 0;
    bool isLongName = false;
    while (pos < BLOCK_SIZE) {
        size_t lenEnd = content.find(' ', pos);
        if (lenEnd == string::npos) {
            err = ERR_OK;
            break;
        }
        string pathLen = content.substr(pos, lenEnd - pos);
        recLen = static_cast<uint32_t>(std::atoi(pathLen.c_str()));
        allLen = recLen + OTHER_HEADER;
        if (allLen > BLOCK_SIZE) {
            isLongName = true;
            break;
        }
        string kvPair = content.substr(lenEnd + 1, recLen - (lenEnd - pos + 1));
        size_t eqPos = kvPair.find('=');
        if (eqPos == string::npos) {
            break;
        }
        string key = kvPair.substr(0, eqPos);
        string value = kvPair.substr(eqPos + 1);
        if (key == "path") {
            longName = value;
            err = ERR_OK;
        }
        pos += recLen;
    }
    if (isLongName) {
        HILOGI("is long name");
        return GetLongName(recLen, allLen);
    }
    return {err, longName};
}

void UntarFile::CheckLongName(std::string longName, FileStatInfo &info)
{
    if (!longName.empty() && longName.back() == '\n') {
        longName.pop_back();
        info.longName = longName;
    }
}

std::tuple<int, std::string> UntarFile::GetLongName(uint32_t recLen, uint32_t allLen)
{
    int err = DEFAULT_ERR;
    off_t curPos = ftello(tarFilePtr_);
    if (fseeko(tarFilePtr_, curPos - BLOCK_SIZE, SEEK_SET) != 0) {
        HILOGE("fseeko failed");
        return {err, ""};
    }
    double result = static_cast<double>(allLen) / BLOCK_SIZE;
    auto ret = static_cast<uint32_t>(std::ceil(result));
    auto curSize = ret * BLOCK_SIZE;
    char *block = new char[curSize];
    if (memset_s(block, curSize, 0, curSize) != EOK) {
        delete[] block;
        return {err, ""};
    }
    auto readCnt = fread(block, 1, curSize, tarFilePtr_);
    if (readCnt < curSize) {
        HILOGE("Parsing tar file completed, read data count is less then block size.");
        delete[] block;
        return {err, ""};
    }
    string content(block, curSize);
    string longName = "";
    size_t pos = 0;
    while (pos < curSize) {
        size_t lenEnd = content.find(' ', pos);
        if (lenEnd == string::npos) {
            err = ERR_OK;
            break;
        }
        string pathLen = content.substr(pos, lenEnd - pos);
        size_t recLen = static_cast<size_t>(std::atoi(pathLen.c_str()));
        string KvPair = content.substr(lenEnd + 1, recLen - (lenEnd - pos + 1));
        size_t eqPos = KvPair.find('=');
        if (eqPos == string::npos) {
            break;
        }
        string key = KvPair.substr(0, eqPos);
        string value = KvPair.substr(eqPos + 1);
        if (key == "path") {
            longName = value;
            err = ERR_OK;
        }
        pos += recLen;
    }
    delete[] block;
    return {err, longName};
}
} // namespace OHOS::FileManagement::Backup