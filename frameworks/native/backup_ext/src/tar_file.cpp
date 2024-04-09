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

#include "tar_file.h"

#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stack>
#include <sys/types.h>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "securec.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
namespace {
const uint32_t OFF_T_SIZE = 4;
const uint32_t PERMISSION_MASK = 07777;
const uint64_t MAX_FILE_SIZE = 0777777777777L;
const uint32_t OCTSTR_LEN = sizeof(off_t) * 3 + 1;
const uint32_t DEFAULT_SLICE_SIZE = 100 * MB_TO_BYTE; // 分片文件大小为100M
const uint32_t MAX_FILE_COUNT = 6000;                 // 单个tar包最多包含6000个文件
const uint32_t WAIT_INDEX = 100000;
const uint32_t WAIT_TIME = 5;
const string VERSION = "1.0";
const string LONG_LINK_SYMBOL = "longLinkSymbol";
} // namespace

TarFile &TarFile::GetInstance()
{
    static TarFile instance;
    return instance;
}

bool TarFile::Packet(const vector<string> &srcFiles, const string &tarFileName, const string &pkPath, TarMap &tarMap)
{
    if (tarFileName.empty() || pkPath.empty()) {
        HILOGE("Invalid parameter");
        return false;
    }
    ioBuffer_.resize(READ_BUFF_SIZE);
    baseTarName_ = tarFileName;
    packagePath_ = pkPath;
    if (pkPath[pkPath.length() - 1] == '/') {
        packagePath_ = packagePath_.substr(0, packagePath_.length() - 1);
    }

    CreateSplitTarFile();

    size_t index = 0;
    for (const auto &filePath : srcFiles) {
        rootPath_ = filePath;
        if (!TraversalFile(rootPath_)) {
            HILOGE("Failed to traversal file");
        }
        index++;
        if (index >= WAIT_INDEX) {
            HILOGD("Sleep to wait");
            sleep(WAIT_TIME);
            index = 0;
        }
    }

    FillSplitTailBlocks();

    tarMap = tarMap_;

    if (currentTarFile_ != nullptr) {
        fclose(currentTarFile_);
        currentTarFile_ = nullptr;
    }

    return true;
}

bool TarFile::TraversalFile(string &filePath)
{
    if (access(filePath.c_str(), F_OK) != 0) {
        HILOGE("File path does not exists, err = %{public}d", errno);
        return false;
    }

    struct stat curFileStat {};
    memset_s(&curFileStat, sizeof(curFileStat), 0, sizeof(curFileStat));
    if (lstat(filePath.c_str(), &curFileStat) != 0) {
        HILOGE("Failed to lstat, err = %{public}d", errno);
        return false;
    }
    if (!AddFile(filePath, curFileStat, false)) {
        HILOGE("Failed to add file to tar package");
        throw BError(BError::Codes::EXT_BACKUP_PACKET_ERROR, "TraversalFile Failed to add file to tar package");
    }

    if (isReset_) {
        return true;
    }

    if (currentTarFileSize_ >= DEFAULT_SLICE_SIZE) {
        HILOGI("Current tar file size is over %{public}d, start to slice",
               static_cast<int32_t>(DEFAULT_SLICE_SIZE / MB_TO_BYTE));
        fileCount_ = 0;
        FillSplitTailBlocks();
        CreateSplitTarFile();
        return true;
    }

    // tar包内文件数量大于6000，分片打包
    fileCount_++;
    if (fileCount_ == MAX_FILE_COUNT) {
        HILOGI("The number of files in the tar package exceeds %{public}d, start to slice", MAX_FILE_COUNT);
        fileCount_ = 0;
        FillSplitTailBlocks();
        CreateSplitTarFile();
    }

    return true;
}

bool TarFile::I2OcsConvert(const struct stat &st, TarHeader &hdr, string &fileName, bool isSplit)
{
    memset_s(&hdr, sizeof(hdr), 0, sizeof(hdr));
    memcpy_s(hdr.mode, sizeof(hdr.mode), I2Ocs(sizeof(hdr.mode), st.st_mode & PERMISSION_MASK).c_str(),
             sizeof(hdr.mode) - 1);
    memcpy_s(hdr.uid, sizeof(hdr.uid), I2Ocs(sizeof(hdr.uid), st.st_uid).c_str(), sizeof(hdr.uid) - 1);
    memcpy_s(hdr.gid, sizeof(hdr.gid), I2Ocs(sizeof(hdr.gid), st.st_gid).c_str(), sizeof(hdr.gid) - 1);
    memcpy_s(hdr.size, sizeof(hdr.size), I2Ocs(sizeof(hdr.size), 0).c_str(), sizeof(hdr.size) - 1);
    memcpy_s(hdr.mtime, sizeof(hdr.mtime), I2Ocs(sizeof(hdr.mtime), st.st_mtime).c_str(), sizeof(hdr.mtime) - 1);
    memset_s(hdr.chksum, sizeof(hdr.chksum), BLANK_SPACE, sizeof(hdr.chksum));

    if (S_ISREG(st.st_mode)) {
        hdr.typeFlag = REGTYPE;
        off_t hdrSize = st.st_size;
        if (sizeof(off_t) <= OFF_T_SIZE || st.st_size <= static_cast<off_t>(MAX_FILE_SIZE)) {
            memcpy_s(hdr.size, sizeof(hdr.size), I2Ocs(sizeof(hdr.size), hdrSize).c_str(), sizeof(hdr.size) - 1);
        } else {
            HILOGE("Invalid tar header size");
            return false;
        }
    } else if (S_ISDIR(st.st_mode)) {
        hdr.typeFlag = DIRTYPE;
    } else {
        return true;
    }

    if (S_ISDIR(st.st_mode) && fileName.back() != '/') {
        fileName.append("/");
    }

    return true;
}

bool TarFile::AddFile(string &fileName, const struct stat &st, bool isSplit)
{
    HILOGI("tar file %{public}s", fileName.c_str());
    currentFileName_ = fileName;

    TarHeader hdr;
    if (!I2OcsConvert(st, hdr, fileName, isSplit)) {
        HILOGE("Failed to I2OcsConvert");
        return false;
    }

    if (fileName.length() < TNAME_LEN) {
        memcpy_s(hdr.name, sizeof(hdr.name), fileName.c_str(), sizeof(hdr.name) - 1);
    } else {
        WriteLongName(fileName, GNUTYPE_LONGNAME);
        memcpy_s(hdr.name, sizeof(hdr.name), LONG_LINK_SYMBOL.c_str(), sizeof(hdr.name) - 1);
    }
    memcpy_s(hdr.magic, sizeof(hdr.magic), TMAGIC.c_str(), sizeof(hdr.magic) - 1);
    memcpy_s(hdr.version, sizeof(hdr.version), VERSION.c_str(), sizeof(hdr.version) - 1);
    FillOwnerName(hdr, st);
    SetCheckSum(hdr);

    if (hdr.typeFlag != REGTYPE) {
        if (WriteTarHeader(hdr) != BLOCK_SIZE) {
            HILOGE("Failed to write all");
            return false;
        }
        currentFileName_ = "";
        return true;
    }

    // write tar header of src file
    if (WriteTarHeader(hdr) != BLOCK_SIZE) {
        HILOGE("Failed to write all");
        return false;
    }
    // write src file content to tar file
    if (!WriteFileContent(fileName, st.st_size)) {
        HILOGE("Failed to write file content");
        return false;
    }
    currentFileName_ = "";
    return true;
}

bool TarFile::WriteFileContent(const string &fileName, off_t size)
{
    int fd = open(fileName.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        HILOGE("Failed to open file %{public}s, err = %{public}d", fileName.data(), errno);
        return false;
    }

    off_t remain = size;
    while (remain > 0) {
        off_t read = ioBuffer_.size();
        if (remain < read) {
            read = remain;
        }
        // read buffer from src file
        if (ReadAll(fd, ioBuffer_, read) != read) {
            HILOGE("Failed to read all");
            break;
        }

        // write buffer to tar file
        if (SplitWriteAll(ioBuffer_, read) != read) {
            HILOGE("Failed to split write all");
            break;
        }
        remain -= read;
    }

    close(fd);
    if (remain == 0) {
        return CompleteBlock(size);
    }
    return false;
}

off_t TarFile::SplitWriteAll(const vector<uint8_t> &ioBuffer, off_t read)
{
    off_t len = ioBuffer.size();
    if (len > read) {
        len = read;
    }
    off_t count = 0;
    while (count < len) {
        auto writeBytes = fwrite(&ioBuffer[count], sizeof(uint8_t), len - count, currentTarFile_);
        if (writeBytes < 1) {
            // 再执行一遍
            writeBytes = fwrite(&ioBuffer[count], sizeof(uint8_t), len - count, currentTarFile_);
            if (writeBytes < 1) {
                HILOGE("Failed to fwrite tar file, err = %{public}d", errno);
                return count;
            }
        }
        count += writeBytes;
        currentTarFileSize_ += writeBytes;
    }
    return count;
}

bool TarFile::CreateSplitTarFile()
{
    tarFileName_ = baseTarName_ + "." + to_string(tarFileCount_) + ".tar";
    currentTarName_ = packagePath_ + "/" + tarFileName_;
    if (currentTarFile_ != nullptr) {
        fclose(currentTarFile_);
        currentTarFile_ = nullptr;
    }
    // create a tar file
    currentTarFile_ = fopen(currentTarName_.c_str(), "wb+");
    if (currentTarFile_ == nullptr) {
        HILOGE("Failed to open file %{public}s, err = %{public}d", currentTarName_.c_str(), errno);
        throw BError(BError::Codes::EXT_BACKUP_PACKET_ERROR, "CreateSplitTarFile Failed to open file");
    }
    currentTarFileSize_ = 0;

    return true;
}

bool TarFile::CompleteBlock(off_t size)
{
    if ((size % BLOCK_SIZE) > 0) {
        int append = BLOCK_SIZE - (size % BLOCK_SIZE);
        vector<uint8_t> buff {};
        buff.resize(BLOCK_SIZE);
        WriteAll(buff, append);
    }
    return true;
}

bool TarFile::FillSplitTailBlocks()
{
    if (currentTarFile_ == nullptr) {
        throw BError(BError::Codes::EXT_BACKUP_PACKET_ERROR, "FillSplitTailBlocks currentTarFile_ is null");
    }
    
    // write tar file tail
    const int END_BLOCK_SIZE = 1024;
    vector<uint8_t> buff {};
    buff.resize(BLOCK_SIZE);
    WriteAll(buff, END_BLOCK_SIZE);
    fflush(currentTarFile_);

    struct stat staTar {};
    int ret = stat(currentTarName_.c_str(), &staTar);
    if (ret != 0) {
        HILOGE("Failed to stat file %{public}s, err = %{public}d", currentTarName_.c_str(), errno);
        throw BError(BError::Codes::EXT_BACKUP_PACKET_ERROR, "FillSplitTailBlocks Failed to stat file");
    }

    if (staTar.st_size == 0 && tarFileCount_ > 0 && fileCount_ == 0) {
        fclose(currentTarFile_);
        currentTarFile_ = nullptr;
        remove(currentTarName_.c_str());
        return true;
    }

    if (isReset_) {
        tarMap_.clear();
    }

    tarMap_.emplace(tarFileName_, make_tuple(currentTarName_, staTar, false));

    fclose(currentTarFile_);
    currentTarFile_ = nullptr;
    tarFileCount_++;

    return true;
}

void TarFile::SetCheckSum(TarHeader &hdr)
{
    int sum = 0;
    vector<uint32_t> buffer {};
    buffer.resize(sizeof(hdr));
    buffer.assign(reinterpret_cast<uint8_t *>(&hdr), reinterpret_cast<uint8_t *>(&hdr) + sizeof(hdr));
    for (uint32_t index = 0; index < BLOCK_SIZE; index++) {
        if (index < CHKSUM_BASE || index > CHKSUM_BASE + CHKSUM_LEN - 1) {
            sum += (buffer[index] & 0xFF);
        } else {
            sum += BLANK_SPACE;
        }
    }
    memcpy_s(hdr.chksum, sizeof(hdr.chksum), I2Ocs(sizeof(hdr.chksum), sum).c_str(), sizeof(hdr.chksum) - 1);
}

void TarFile::FillOwnerName(TarHeader &hdr, const struct stat &st)
{
    struct passwd *pw = getpwuid(st.st_uid);
    if (pw != nullptr) {
        auto ret = snprintf_s(hdr.uname, sizeof(hdr.uname), sizeof(hdr.uname) - 1, "%s", pw->pw_name);
        if (ret < 0 || ret >= static_cast<int>(sizeof(hdr.uname))) {
            HILOGE("Fill pw_name failed, err = %{public}d", errno);
        }
    } else {
        auto ret = snprintf_s(hdr.uname, sizeof(hdr.uname), sizeof(hdr.uname) - 1, "%d", st.st_uid);
        if (ret < 0 || ret >= static_cast<int>(sizeof(hdr.uname))) {
            HILOGE("Fill uid failed, err = %{public}d", errno);
        }
    }

    struct group *gr = getgrgid(st.st_gid);
    if (gr != nullptr) {
        auto ret = snprintf_s(hdr.gname, sizeof(hdr.gname), sizeof(hdr.gname) - 1, "%s", gr->gr_name);
        if (ret < 0 || ret >= static_cast<int>(sizeof(hdr.gname))) {
            HILOGE("Fill gr_name failed, err = %{public}d", errno);
        }
    } else {
        auto ret = snprintf_s(hdr.gname, sizeof(hdr.gname), sizeof(hdr.gname) - 1, "%d", st.st_gid);
        if (ret < 0 || ret >= static_cast<int>(sizeof(hdr.gname))) {
            HILOGE("Fill gid failed, err = %{public}d", errno);
        }
    }
}

off_t TarFile::ReadAll(int fd, vector<uint8_t> &ioBuffer, off_t size)
{
    off_t count = 0;
    off_t len = ioBuffer.size();
    if (len > size) {
        len = size;
    }
    while (count < len) {
        auto readLen = read(fd, &ioBuffer[count], len - count);
        count += readLen;
        if (readLen == 0) {
            break;
        }
    }
    return count;
}

int TarFile::WriteTarHeader(TarHeader &header)
{
    vector<uint8_t> buffer {};
    buffer.resize(sizeof(header));
    buffer.assign(reinterpret_cast<uint8_t *>(&header), reinterpret_cast<uint8_t *>(&header) + sizeof(header));
    return WriteAll(buffer, BLOCK_SIZE);
}

int TarFile::WriteAll(const vector<uint8_t> &buf, size_t len)
{
    size_t count = 0;
    while (count < len) {
        auto i = fwrite(&buf[0] + count, sizeof(char), len - count, currentTarFile_);
        if (i < 1) {
            HILOGE("Failed to fwrite tar file, err = %{public}d", errno);
            return count;
        }
        count += i;
        currentTarFileSize_ += i;
    }
    return count;
}

string TarFile::I2Ocs(int len, off_t val)
{
    char tmp[OCTSTR_LEN] = {0};
    if (sprintf_s(tmp, sizeof(tmp), "%0*llo", len - 1, val) < 0) {
        return "";
    }
    return string(tmp);
}

bool TarFile::WriteLongName(string &name, char type)
{
    // fill tar header for long name
    TarHeader tmp;
    memset_s(&tmp, sizeof(tmp), 0, sizeof(tmp));

    int sz = name.length() + 1;
    const string FORMAT = "%0*d";

    strlcpy(tmp.name, LONG_LINK_SYMBOL.c_str(), sizeof(tmp.name));
    int ret = sprintf_s(tmp.mode, sizeof(tmp.mode), FORMAT.c_str(), (int)sizeof(tmp.mode) - 1, 0);
    if (ret < 0) {
        return false;
    }
    ret = sprintf_s(tmp.uid, sizeof(tmp.uid), FORMAT.c_str(), (int)sizeof(tmp.uid) - 1, 0);
    if (ret < 0) {
        return false;
    }
    ret = sprintf_s(tmp.gid, sizeof(tmp.gid), FORMAT.c_str(), (int)sizeof(tmp.gid) - 1, 0);
    if (ret < 0) {
        return false;
    }
    ret = sprintf_s(tmp.size, sizeof(tmp.size), FORMAT.c_str(), (int)sizeof(tmp.size) - 1, 0);
    if (ret < 0) {
        return false;
    }
    ret = sprintf_s(tmp.mtime, sizeof(tmp.mtime), FORMAT.c_str(), (int)sizeof(tmp.mtime) - 1, 0);
    if (ret < 0) {
        return false;
    }
    memcpy_s(tmp.size, sizeof(tmp.size), I2Ocs(sizeof(tmp.size), sz).c_str(), sizeof(tmp.size) - 1);

    tmp.typeFlag = type;
    memset_s(tmp.chksum, sizeof(tmp.chksum), BLANK_SPACE, sizeof(tmp.chksum));

    strlcpy(tmp.magic, TMAGIC.c_str(), sizeof(tmp.magic));
    strlcpy(tmp.version, VERSION.c_str(), sizeof(tmp.version));

    SetCheckSum(tmp);

    // write long name head to archive
    if (WriteTarHeader(tmp) != BLOCK_SIZE) {
        HILOGE("Failed to write long name header");
        return false;
    }

    // write name to archive
    vector<uint8_t> buffer {};
    buffer.resize(sz);
    buffer.assign(name.begin(), name.end());
    if (WriteAll(buffer, sz) != sz) {
        HILOGE("Failed to write long name buffer");
        return false;
    }

    return CompleteBlock(sz);
}

void TarFile::SetPacketMode(bool isReset)
{
    isReset_ = isReset;
}
} // namespace OHOS::FileManagement::Backup