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

#include "tar_file.h"
namespace OHOS::FileManagement::Backup {

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
        return false;
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
    struct stat staTar {};
    int ret = stat(currentTarName_.c_str(), &staTar);
    if (ret != 0) {
        HILOGE("Failed to stat file %{public}s, err = %{public}d", currentTarName_.c_str(), errno);
        return false;
    }
    if (staTar.st_size == 0 && tarFileCount_ > 0 && currentTarFile_ != nullptr) {
        fclose(currentTarFile_);
        currentTarFile_ = nullptr;
        remove(currentTarName_.c_str());
        return true;
    }
    // write tar file tail
    const int END_BLOCK_SIZE = 1024;
    vector<uint8_t> buff {};
    buff.resize(BLOCK_SIZE);
    WriteAll(buff, END_BLOCK_SIZE);
    tarMap_.emplace(tarFileName_, make_tuple(currentTarName_, staTar, false));

    fflush(currentTarFile_);
    fclose(currentTarFile_);
    currentTarFile_ = nullptr;
    tarFileCount_++;

    return true;
}

bool TarFile::FillSplitHeaderBlocks()
{
    if (fileRemainSize_ == 0) {
        return true;
    }
    struct stat curStat;
    lstat(currentFileName_.c_str(), &curStat);
    return AddFile(currentFileName_, curStat, true);
}

void TarFile::FlushTarSizeAndFlag(bool isSplit, const off_t fileSize, off_t &hdrSize, char &typeFlag)
{
    if (isSplit) {
        if (fileRemainSize_ <= DEFAULT_SLICE_SIZE) {
            typeFlag = SPLIT_END_TYPE;
            hdrSize = fileRemainSize_;
        } else {
            typeFlag = SPLIT_CONTINUE_TYPE;
            hdrSize = DEFAULT_SLICE_SIZE;
        }
    } else {
        if (IsFirstSplitPKT(fileSize)) {
            typeFlag = SPLIT_START_TYPE;
            hdrSize = GetFirstSizeOfSplitFile(fileSize);
        }
    }
}

bool TarFile::IsFirstSplitPKT(off_t fileSize)
{
    off_t fileSpace = GetSizeOfFile(fileSize);
    off_t splitRemainSpace = GetSplitPKTRemainSize();
    if (fileSpace > splitRemainSpace) {
        return true;
    }
    return false;
}

off_t TarFile::GetSizeOfFile(off_t fileSize)
{
    const int PADDING_BLOCK_NUM = 2;
    size_t nameSize = currentFileName_.length();
    off_t len = 0;
    if (nameSize >= TNAME_LEN) {
        len = (nameSize / BLOCK_SIZE + PADDING_BLOCK_NUM) * BLOCK_SIZE;
    }
    len += (fileSize / BLOCK_SIZE + PADDING_BLOCK_NUM - 1) * BLOCK_SIZE;
    return len;
}

off_t TarFile::GetSplitPKTRemainSize()
{
    off_t size = DEFAULT_SLICE_SIZE - currentTarFileSize_;
    return size;
}

off_t TarFile::GetFirstSizeOfSplitFile(off_t fileSize)
{
    off_t fileSpace = GetSizeOfFile(fileSize);
    off_t splitRemainSpace = GetSplitPKTRemainSize();

    const int END_BLOCK_SIZE = 1024;
    if (fileSpace - splitRemainSpace <= END_BLOCK_SIZE) {
        return fileSpace;
    }
    return splitRemainSpace;
}

void TarFile::SetCheckSum(TarHeader &hdr)
{
    int sum = 0;
    vector<uint8_t> buffer {};
    buffer.resize(sizeof(hdr));
    buffer.assign(reinterpret_cast<uint8_t *>(&hdr), reinterpret_cast<uint8_t *>(&hdr) + sizeof(hdr));
    for (int index = 0; index < BLOCK_SIZE; index++) {
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
        int ret = snprintf_s(hdr.uname, sizeof(hdr.uname), sizeof(hdr.uname) - 1, "%s", pw->pw_name);
        if (ret < 0 || ret >= sizeof(hdr.uname)) {
            HILOGE("Fill pw_name failed, err = %{public}d", errno);
        }
    } else {
        int ret = snprintf_s(hdr.uname, sizeof(hdr.uname), sizeof(hdr.uname) - 1, "%d", st.st_uid);
        if (ret < 0 || ret >= sizeof(hdr.uname)) {
            HILOGE("Fill uid failed, err = %{public}d", errno);
        }
    }

    struct group *gr = getgrgid(st.st_gid);
    if (gr != nullptr) {
        int ret = snprintf_s(hdr.gname, sizeof(hdr.gname), sizeof(hdr.gname) - 1, "%s", gr->gr_name);
        if (ret < 0 || ret >= sizeof(hdr.gname)) {
            HILOGE("Fill gr_name failed, err = %{public}d", errno);
        }
    } else {
        int ret = snprintf_s(hdr.gname, sizeof(hdr.gname), sizeof(hdr.gname) - 1, "%d", st.st_gid);
        if (ret < 0 || ret >= sizeof(hdr.gname)) {
            HILOGE("Fill gid failed, err = %{public}d", errno);
        }
    }
}

int TarFile::ReadAll(int fd, vector<uint8_t> &ioBuffer, off_t size)
{
    size_t count = 0;
    size_t len = ioBuffer.size();
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
        count += i;
        currentTarFileSize_ += i;
    }
    return count;
}

string TarFile::I2Ocs(int len, off_t val)
{
    char tmp[OCTSTR_LEN] = {0};
    int ret = sprintf_s(tmp, sizeof(tmp), "%0*llo", len - 1, val);
    if (ret < 0) {
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
        return false;
    }

    // write name to archive
    vector<uint8_t> buffer {};
    buffer.resize(sz);
    buffer.assign(reinterpret_cast<uint8_t *>(&name), reinterpret_cast<uint8_t *>(&name) + sz);
    if (WriteAll(buffer, sz) != sz) {
        return false;
    }

    return CompleteBlock(sz);
}

} // namespace OHOS::FileManagement::Backup