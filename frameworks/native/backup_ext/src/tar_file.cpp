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

#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stack>
#include <sys/types.h>
#include <unistd.h>

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
const string VERSION = "1.0";
const string LONG_LINK_SYMBOL = "longLinkSymbol";
} // namespace

static vector<uint8_t> CharToUint8(char *input)
{
    vector<uint8_t> vec {};
    for (size_t i = 0; i < strlen(input); i++) {
        vec.emplace_back(static_cast<uint8_t>(input[i]));
    }
    return vec;
}

TarFile &TarFile::GetInstance()
{
    static TarFile instance;
    return instance;
}

bool TarFile::Packet(const vector<string> &srcFiles, const string &tarFileName, const string &pkPath, TarMap &tarMap)
{
    if (srcFiles.empty() || tarFileName.empty() || pkPath.empty()) {
        HILOGE("Invalid parameter");
        return false;
    }
    ioBuffer_.resize(READ_BUFF_SIZE);
    baseTarName_ = tarFileName;
    packagePath_ = pkPath;
    if (pkPath[pkPath.length() - 1] == '/') {
        packagePath_ = packagePath_.substr(0, packagePath_.length() - 1);
    }

    if (!CreateSplitTarFile()) {
        HILOGE("Failed to create split tar file");
        return false;
    }

    for (auto &filePath : srcFiles) {
        rootPath_ = filePath;
        TraversalFile(rootPath_);
    }
    bool ret = FillSplitTailBlocks();
    tarMap = tarMap_;

    if (currentTarFile_ != nullptr) {
        fclose(currentTarFile_);
        currentTarFile_ = nullptr;
    }

    return ret;
}

bool TarFile::TraversalDir(const string &currentPath)
{
    stack<string> dirs, destDirs;
    dirs.push(currentPath);
    while (dirs.size() > 0) {
        string curPath = dirs.top();
        dirs.pop();

        DIR *curDir = opendir(curPath.c_str());
        if (curDir == nullptr) {
            HILOGI("Failed to opendir errno:%{public}d", errno);
            return false;
        }

        string fullPath(currentPath);
        if (fullPath[fullPath.length() - 1] != '/') {
            fullPath.append("/");
        }

        struct dirent *ptr = readdir(curDir);
        while (ptr != nullptr) {
            if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
                continue;
            }
            string subPath = fullPath + ptr->d_name;
            dirs.push(subPath);
            destDirs.push(subPath);
            ptr = readdir(curDir);
        }

        closedir(curDir);
    }

    while (destDirs.size() > 0) {
        string curPath = destDirs.top();
        TraversalFile(curPath);
        destDirs.pop();
    }

    return true;
}

bool TarFile::TraversalFile(string &filePath)
{
    if (access(filePath.c_str(), F_OK) != 0) {
        HILOGE("File path does not exists, err = %{public}d", errno);
        return false;
    }

    struct stat currentStat {};
    memset_s(&currentStat, sizeof(currentStat), 0, sizeof(currentStat));
    int err = lstat(filePath.c_str(), &currentStat);
    if (err != 0) {
        HILOGI("Failed to lstat err = %{public}d", errno);
        return false;
    }
    AddFile(filePath, currentStat, false);

    // tar包内文件数量大于6000，分片打包
    fileCount_++;
    if (fileCount_ == MAX_FILE_COUNT) {
        HILOGI("The number of files in the tar package exceeds %{public}d, start to slice.", MAX_FILE_COUNT);
        fileCount_ = 0;
        FillSplitTailBlocks();
        CreateSplitTarFile();
        FillSplitHeaderBlocks();
    }

    if (S_ISDIR(currentStat.st_mode)) {
        HILOGI("%{public}s is a directory", filePath.data());
        return TraversalDir(filePath);
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
        FlushTarSizeAndFlag(isSplit, st.st_size, hdrSize, hdr.typeFlag);
        if (sizeof(off_t) <= OFF_T_SIZE || st.st_size <= static_cast<off_t>(MAX_FILE_SIZE)) {
            memcpy_s(hdr.size, sizeof(hdr.size), I2Ocs(sizeof(hdr.size), hdrSize).c_str(), sizeof(hdr.size) - 1);
        } else {
            return false;
        }
    } else if (S_ISLNK(st.st_mode)) {
        hdr.typeFlag = SYMTYPE;
        int targetLen = sizeof(hdr.linkName);
        memcpy_s(hdr.linkName, targetLen, FillLinkName(targetLen, fileName, st.st_size + 1).c_str(), targetLen - 1);
    } else if (S_ISDIR(st.st_mode)) {
        hdr.typeFlag = DIRTYPE;
    } else {
        return true;
    }

    if (S_ISDIR(st.st_mode)) {
        fileName.append("/");
    }

    return true;
}

bool TarFile::AddFile(string &fileName, const struct stat &st, bool isSplit)
{
    currentFileName_ = fileName;

    TarHeader hdr;
    if (!I2OcsConvert(st, hdr, fileName, isSplit)) {
        return false;
    }

    if (strncpy_s(hdr.name, sizeof(hdr.name), fileName.c_str(), sizeof(hdr.name) - 1) == 0) {
        if (fileName.length() >= sizeof(hdr.name)) {
            WriteLongName(fileName, GNUTYPE_LONGNAME);
        }
    }

    memcpy_s(hdr.magic, sizeof(hdr.magic), TMAGIC.c_str(), sizeof(hdr.magic) - 1);
    memcpy_s(hdr.version, sizeof(hdr.version), VERSION.c_str(), sizeof(hdr.version) - 1);

    FillOwnerName(hdr, st);
    SetCheckSum(hdr);

    if ((hdr.typeFlag != REGTYPE) && (hdr.typeFlag != SPLIT_START_TYPE)) {
        bool flag = false;
        vector<uint8_t> hdrBuf = CharToUint8(reinterpret_cast<char *>(&hdr));
        if (SplitWriteAll(hdrBuf, BLOCK_SIZE, flag) != BLOCK_SIZE) {
            HILOGI("Failed to split write all");
            return false;
        }
        return true;
    }

    // write tar header of src file
    string hdrStr(reinterpret_cast<char *>(&hdr), BLOCK_SIZE);
    if (WriteAll(hdrStr, BLOCK_SIZE) != BLOCK_SIZE) {
        HILOGI("Failed to write all");
        return false;
    }
    // write src file content to tar file
    if (!WriteFileContent(fileName, st.st_size)) {
        HILOGI("Failed to write file content");
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
    fileRemainSize_ = remain;
    bool isFilled = false;

    while (remain > 0) {
        size_t read = ioBuffer_.size();
        if (size < ioBuffer_.size()) {
            read = size;
        } else {
            if (read > remain) {
                read = remain;
            }
        }
        // read buffer from src file
        if (ReadAll(fd, ioBuffer_, size) != read) {
            HILOGI("Failed to read all");
            break;
        }

        fileRemainSize_ -= read;
        // write buffer to tar file
        if (SplitWriteAll(ioBuffer_, read, isFilled) != read) {
            HILOGI("Failed to split write all");
            break;
        }
        remain -= read;
    }

    close(fd);
    fileRemainSize_ = 0;
    if (0 == remain) {
        if (!isFilled) {
            return CompleteBlock(size);
        } else {
            return true;
        }
    }
    return false;
}

int TarFile::SplitWriteAll(const vector<uint8_t> &ioBuffer, int read, bool &isFilled)
{
    size_t len = ioBuffer.size();
    if (len > read) {
        len = read;
    }
    size_t count = 0;
    while (count < len) {
        auto writeBytes = fwrite(&ioBuffer[count], sizeof(uint8_t), len - count, currentTarFile_);
        if (writeBytes < 1) {
            HILOGI("Failed to fwrite tar file, err = %{public}d", errno);
            return writeBytes;
        }
        count += writeBytes;
        currentTarFileSize_ += writeBytes;
        if (currentTarFileSize_ >= DEFAULT_SLICE_SIZE) {
            HILOGI("Current tar file size is over %{public}dM, start to splice tar", DEFAULT_SLICE_SIZE);
            fileCount_ = 0;
            if (count < READ_BUFF_SIZE) {
                isFilled = true;
                CompleteBlock(count);
            }
            FillSplitTailBlocks();
            CreateSplitTarFile();
            FillSplitHeaderBlocks();
        }
    }
    return count;
}


} // namespace OHOS::FileManagement::Backup