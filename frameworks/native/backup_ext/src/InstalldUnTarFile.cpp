/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "InstalldUnTarFile.h"

#include <cstdio>
#include <directory_ex.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <unistd.h>

#include "securec.h"

#include <hilog/log.h>

namespace installd {
const uid_t OCTAL = 8;

uid_t FixUpOwnerDirFile(const uid_t uid, const gid_t gid, const uid_t owner, gid_t& newGid)
{
    uid_t newUid = owner;
    if (0 == owner || uid < APP_ID_START) {
        newUid = uid;
        newGid = gid;

        return newUid;
    }

    if (uid == gid) {
        newGid = newUid;
    } else if (0 == ((gid - uid) % UID_GID_OFFSET)) {
        newGid = (gid - uid) + newUid;
    } else {
        newGid = gid;
    }

    return newUid;
}

off_t ParseOctalStr(const char* p, size_t n)
{
    off_t ret = 0;
    std::string octalStr(p);
    auto it = octalStr.begin();

    while (it != octalStr.end() && (*it < '0' || *it > '7') && n > 0) {
        ++it;
        --n;
    }

    while (it != octalStr.end() && *it >= '0' && *it <= '7' && n > 0) {
        ret *= OCTAL;
        ret += *it - '0';
        ++it;
        --n;
    }

    return ret;
}

static int GenRealPath(const char *rootPath, const char *pathName, char* &realPath)
{
    if (rootPath == nullptr || pathName == nullptr || realPath == nullptr) {
        return ERR_PARAM;
    }
    size_t allLen = strlen(rootPath);
    if (rootPath[allLen - 1] != '/') {
        allLen += 1;
    }
    allLen += strlen(pathName);
    if (0 == allLen || allLen >= PATH_MAX_LEN) {
        LOGE("ERR_PARAM");
        return ERR_PARAM;
    }

    size_t curLen = strlen(rootPath);
    if (strncpy_s(realPath, PATH_MAX_LEN, rootPath, curLen) != 0) {
        LOGE("GenRealPath get realPath error");
        return ERR_PARAM;
    }

    if (rootPath[curLen - 1] != '/') {
        realPath[curLen] = '/';
        curLen += 1;
    }

    if (strncpy_s(realPath + curLen, PATH_MAX_LEN - curLen, pathName, strlen(pathName)) != 0) {
        LOGE("GenRealPath get realPath by curLen error");
        return ERR_PARAM;
    }
    realPath[allLen] = '\0';
    return 0;
}

static int CreateDir(char *path, mode_t mode)
{
    if (path == nullptr) {
        return ERR_PARAM;
    }

    size_t len = strlen(path);
    if (path[len - 1] == '/') {
        path[len - 1] = '\0';
    }

    int ret = access(path, F_OK);
    if (ret == -1) {
        ret = mkdir(path, mode);
    }
    return ret;
}

static FILE *CreateFile(char* path, mode_t mode, char fileType)
{
    if (path == nullptr) {
        return nullptr;
    }

    std::string appendStr = "wb+";
    if (fileType == SPLIT_END_TYPE || fileType == SPLIT_CONTINUE_TYPE) {
        appendStr = "ab+";
    }
    FILE *f = fopen(path, appendStr.c_str());
    if (f == nullptr) {
        char *p = strrchr(path, '/');
        if (p != nullptr) {
            *p = '\0';
            if (CreateDir(path, mode) == 0) {
                *p = '/';
                f = fopen(path, "wb+");
            }
        }
    }
    if (f == nullptr) {
        return f;
    }
    if (fchmod(fileno(f), S_IRUSR | S_IWUSR) == -1) {
        LOGE("fail to change file permission");
        return nullptr;
    }

    return f;
}

static int CreateSoftlink(const char* oldPath, const char* newPath)
{
    if (oldPath == nullptr || newPath == nullptr) {
        return ERR_PARAM;
    }

    unlink(newPath);
    int ret = symlink(oldPath, newPath);
    return ret;
}

UnTarFile::UnTarFile(const char *tarPath): FilePtr(nullptr), tarSize(0), newOwner(0)
{
    file_names.clear();
    file_sizes.clear();
    file_data_addrs.clear();

    if (tarPath != nullptr) {
        LOGI("untarfile begin..");
        m_srcPath = tarPath;
        FilePtr = fopen(tarPath, "rb");
        if (FilePtr == nullptr) {
            LOGE("open file fail");
        }
    }
}

UnTarFile::~UnTarFile(void)
{
    if (FilePtr != nullptr) {
        (void)fclose(FilePtr);
        FilePtr = nullptr;
    }

    file_names.clear();
    file_sizes.clear();
    file_data_addrs.clear();
}

void UnTarFile::Reset()
{
    if (FilePtr != nullptr) {
        (void)fclose(FilePtr);
        FilePtr = nullptr;
    }

    isSplit = false;
    file_names.clear();
    file_sizes.clear();
    file_data_addrs.clear();
}

int UnTarFile::UnSplitTar(const std::string &tarFile, const std::string &rootpath)
{
    FilePtr = fopen(tarFile.c_str(), "rb");
    if (FilePtr == nullptr) {
        LOGE("UnTarFile::UnSplitPack, untar split failed!");
    }
    isSplit = true;
    std::string destPath(rootpath);

    CreateDirWithRecursive(destPath);

    int parseRet = ParseTarFile(destPath.c_str(), eUnpack);
    if (parseRet != 0) {
        LOGE("UnTarFile::UnSplitPack, untar split failed!");
    } else {
        LOGI("UnTarFile::UnSplitPack, untar split suc!");
    }
    (void)fclose(FilePtr);
    FilePtr = nullptr;
    return parseRet;
}

bool UnTarFile::CheckIsSplitTar(const std::string &tarFile, const std::string &rootpath)
{
    FilePtr = fopen(tarFile.c_str(), "rb");
    if (FilePtr == nullptr) {
        LOGE("UnTarFile::CheckIsSplitTar, open split failed!");
    }
    std::string destPath(rootpath);
    int parseRet = ParseTarFile(destPath.c_str(), eCheckSplit);
    if (parseRet != 0) {
        LOGE("UnTarFile::CheckIsSplitTar, check is split failed!");
    } else {
        LOGI("UnTarFile::CheckIsSplitTar, check is split, %{public}d", isSplit);
    }
    (void)fclose(FilePtr);
    FilePtr = nullptr;
    if (isSplit) {
        return true;
    }
    return false;
}

bool UnTarFile::VerifyChecksum(const TarHeader *tarHeader)
{
    if (tarHeader == nullptr) {
        return false;
    }

    char *headerBuff = (char *)tarHeader;

    int u = 0;
    for (int n = 0; n < BLOCK_SIZE; ++n) {
        if (n < CHKSUM_BASE || n > CHKSUM_BASE + TarUtil::CHKSUM_LEN - 1) {
            /* Standard tar checksum adds unsigned bytes. */
            u += (*(headerBuff + n) & 0xFF);
        } else {
            u += BLANK_SPACE;
        }
    }

    return (u == static_cast<int>(ParseOctalStr(headerBuff + CHKSUM_BASE, TarUtil::CHKSUM_LEN)));
}

bool UnTarFile::IsValidTarBlock(const TarHeader *tarHeader)
{
    if (tarHeader == nullptr) {
        return false;
    }

    // check magic && checksum
    if (0 == strncmp(tarHeader->magic, TMAGIC, TMAGIC_LEN - 1) && VerifyChecksum(tarHeader)) {
        return true;
    }

    LOGD("Invalid tar format.");
    return false;
}

bool UnTarFile::IsEmptyBlock(const char *p)
{
    return ('\0' == p[0]);
}

int UnTarFile::ParseTarFile(const char *rootPath, EParseType type)
{
    if (FilePtr == nullptr) {
        LOGE("read tar happened error!\n");
        return ERR_PARAM;
    }

    if (rootPath == nullptr) {
        LOGE("rootPath is nullptr!\n");
        return ERR_NOEXIST;
    }
    LOGI("ParseTarFile");

    // re-parse tar header
    char buff[BLOCK_SIZE] = {0};
    size_t readCnt = 0;
    off_t pos = 0;
    char *longName = nullptr;
    char *longLink = nullptr;
    char *fullPath = nullptr;
    bool isSkip = false;
    bool isSoftlink = false;
    int ret = 0;

    // tarSize
    fseeko(FilePtr, 0L, SEEK_END);
    tarSize = ftello(FilePtr);

    // reback file to begin
    fseeko(FilePtr, 0L, SEEK_SET);
    if (tarSize % BLOCK_SIZE != 0) {
        LOGE("tarfile size should be a multiple of 512 bytes");
        return ERR_FORMAT;
    }

    fullPath = (char *)malloc(PATH_MAX_LEN * sizeof(char));
    if (fullPath == nullptr) {
        return ERR_MALLOC;
    }

    memset_s(fullPath, PATH_MAX_LEN * sizeof(char), 0, PATH_MAX_LEN * sizeof(char));

    while (1) {
        readCnt = fread(buff, 1, BLOCK_SIZE, FilePtr);
        if (readCnt < BLOCK_SIZE) {
            LOGE("read short than 512 expected, got %{public}zu, tarSize", readCnt);

            // when split unpack, ftell size is over than file really size [0,READ_BUFF_SIZE]
            if (!isSplit || readCnt != 0 || ftello(FilePtr) > (tarSize + READ_BUFF_SIZE)) {
                ret = ERR_IO;
            }
            FreePointer(longName, longLink, fullPath);
            return ret;
        }

        // two empty continuous block indicate end of file
        if (IsEmptyBlock(buff)) {
            char tailBuff[BLOCK_SIZE] = {0};
            size_t tailRead = 0;
            tailRead = fread(tailBuff, 1, BLOCK_SIZE, FilePtr);
            if ((tailRead == BLOCK_SIZE) && IsEmptyBlock(tailBuff)) {
                LOGI("untarfile is end.Success!");
                FreePointer(longName, longLink, fullPath);
                return ret;
            }
        }

        // check header
        TarHeader *tarHeader = (TarHeader *)buff;
        if (!IsValidTarBlock(tarHeader)) {
            LOGE("isSplit cur size %{public}jd, tarSize %{public}jd", ftello(FilePtr), tarSize);

            // when split unpack, ftell size is over than file really size [0,READ_BUFF_SIZE]
            if (!isSplit || ftello(FilePtr) > (tarSize + READ_BUFF_SIZE) || !IsEmptyBlock(buff)) {
                ret = ERR_FORMAT;
            }
            FreePointer(longName, longLink, fullPath);
            return ret;
        }

        // mode
        mode_t mode = (mode_t) 448;

        // uid & gid
        gid_t newGid = 0;
        uid_t uid = (uid_t)ParseOctalStr(buff + TUID_BASE, TUID_LEN);
        gid_t gid = (gid_t)ParseOctalStr(buff + TGID_BASE, TGID_LEN);
        uid_t newUid = FixUpOwnerDirFile(uid, gid, newOwner, newGid);

        // file size & content offset
        off_t fileSize = ParseOctalStr(buff + TSIZE_BASE, TSIZE_LEN);
        off_t fileBlockCnt = (fileSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
        pos = ftello(FilePtr);

        // longName & longLink
        char *realName = tarHeader->name;
        if (longName != nullptr) {
            realName = longName;
        }

        GenRealPath(rootPath, realName, fullPath);
        char *realLink = tarHeader->linkname;
        if (longLink != nullptr) {
            realLink = longLink;
        }
        CreateDir(const_cast<char *>(rootPath), mode);
        switch (tarHeader->typeflag) {
            case SPLIT_START_TYPE:
            case SPLIT_END_TYPE:
            case SPLIT_CONTINUE_TYPE:
                if (eCheckSplit == type) {
                    isSplit = true;
                    FreePointer(longName, longLink, fullPath);
                    return ret;
                }
            case REGTYPE:    /* regular file */
            case AREGTYPE: { /* regular file */
                if (eList == type) {
                    file_names.push_back(std::string(realName));
                    file_sizes.push_back(fileSize);
                    file_data_addrs.push_back(pos);

                    fseeko(FilePtr, fileBlockCnt * BLOCK_SIZE, SEEK_CUR);
                } else if (eUnpack == type) {
                    char *destBuff = (char *)malloc(READ_BUFF_SIZE * sizeof(char));
                    if (destBuff != nullptr) {
                        FILE *destF = CreateFile(fullPath, mode, tarHeader->typeflag);
                        bool IsAbort = false;
                        bool IsInvalid = false;
                        if (destF != nullptr) {
                            off_t restSize = fileSize;
                            size_t readBuffSize = READ_BUFF_SIZE;

                            memset_s(destBuff, READ_BUFF_SIZE * sizeof(char), 0, READ_BUFF_SIZE * sizeof(char));

                            while (restSize > 0) {
                                if (restSize < READ_BUFF_SIZE) {
                                    readBuffSize = restSize;
                                }
                                if (readBuffSize != fread(destBuff, sizeof(char), readBuffSize, FilePtr)) {
                                    LOGE("read file content shorter than expect!\n");
                                    IsInvalid = true;
                                    break;
                                }

                                if (readBuffSize != fwrite(destBuff, sizeof(char), readBuffSize, destF)) {
                                    LOGE("write file content shorter than expect!\n");
                                    IsInvalid = true;
                                    break;
                                }
                                restSize -= readBuffSize;
                            }

                            if (destBuff != nullptr) {
                                free(destBuff);
                                destBuff = nullptr;
                            }
                            if (destF != nullptr) {
                                fflush(destF);
                                (void)fclose(destF);
                                destF = nullptr;
                            }
                            if (IsInvalid) {
                                unlink(fullPath);
                                isSkip = true;
                            }
                            if (IsAbort) {
                                FreePointer(longName, longLink, fullPath);
                                return ret;
                            }

                            // anyway, go to correct pos
                            fseeko(FilePtr, pos + fileBlockCnt * BLOCK_SIZE, SEEK_SET);
                        } else {
                            LOGE("destF is null!");
                            fseeko(FilePtr, fileBlockCnt * BLOCK_SIZE, SEEK_CUR);
                        }
                    } else {
                        LOGE("malloc memory fail!skip!");
                    }
                    isSkip = false;
                }
                break;
            }
            case SYMTYPE: {
                CreateSoftlink(realLink, fullPath);
                isSoftlink = true;
                isSkip = false;
                break;
            }
            case DIRTYPE: {
                CreateDir(fullPath, mode);
                isSkip = false;
                break;
            }
            case TarUtil::GNUTYPE_LONGNAME: {
                if (longName != nullptr) {
                    free(longName);
                    longName = nullptr;
                }

                size_t nameLen = (size_t)fileSize;
                if (nameLen < PATH_MAX_LEN) {
                    longName = (char *)malloc((nameLen + 1) * sizeof(char));
                }
                if (longName != nullptr) {
                    memset_s(longName, (nameLen + 1) * sizeof(char), 0, (nameLen + 1) * sizeof(char));
                    if (nameLen != fread(longName, sizeof(char), nameLen, FilePtr)) {
                        free(longName);
                        longName = nullptr;
                    }
                }

                // anyway, go to correct pos
                isSkip = true;
                fseeko(FilePtr, pos + fileBlockCnt * BLOCK_SIZE, SEEK_SET);
                continue;
            }
            case GNUTYPE_LONGLINK: {
                /* long link */
                if (longLink != nullptr) {
                    free(longLink);
                    longLink = nullptr;
                }

                size_t nameLen = (size_t)fileSize;
                if (nameLen < PATH_MAX_LEN) {
                    longLink = (char *)malloc((nameLen + 1) * sizeof(char));
                }
                if (longLink != nullptr) {
                    memset_s(longLink, (nameLen + 1) * sizeof(char), 0, (nameLen + 1) * sizeof(char));
                    if (nameLen != fread(longLink, sizeof(char), nameLen, FilePtr)) {
                        free(longLink);
                        longLink = nullptr;
                    }
                }

                // anyway, go to correct pos
                isSkip = true;
                fseeko(FilePtr, pos + fileBlockCnt * BLOCK_SIZE, SEEK_SET);
                continue;
            }
            default: {
                // Ignoring, skip
                isSkip = true;
                fseeko(FilePtr, fileBlockCnt * BLOCK_SIZE, SEEK_CUR);
                break;
            }
        }

        if (!isSkip) {
            if (!isSoftlink) {
                chmod(fullPath, mode);
                chown(fullPath, newUid, newGid);
            } else {
                lchown(fullPath, newUid, newGid);
                isSoftlink = false;
            }
        }
        isSkip = false;

        if (longName != nullptr) {
            free(longName);
            longName = nullptr;
        }
        if (longLink != nullptr) {
            free(longLink);
            longLink = nullptr;
        }
    }
    return ret;
}

void UnTarFile::FreePointer(char *longName, char *longLink, char *fullPath)
{
    if (fullPath != nullptr) {
        free(fullPath);
        fullPath = nullptr;
    }
    if (longName != nullptr) {
        free(longName);
        longName = nullptr;
    }
    if (longLink != nullptr) {
        free(longLink);
        longLink = nullptr;
    }
}

bool UnTarFile::CreateDirWithRecursive(const std::string &filePath, mode_t mode)
{
    if (filePath.empty()) {
        LOGE("CreateDirWithRecursive filePath is empty");
        return false;
    }
    if (access(filePath.c_str(), F_OK) == 0) {
        return true;
    }

    LOGI("CreateDirWithRecursive filePath %{public}s is not exist, need create", filePath.c_str());
    std::string::size_type index = 0;
    do {
        index = filePath.find('/', index + 1);
        std::string subPath = (index == std::string::npos) ? filePath : filePath.substr(0, index);
        if (access(subPath.c_str(), F_OK) != 0) {
            if (mkdir(subPath.c_str(), mode) != 0) {
                return false;
            }
        }
    } while (index != std::string::npos);
    return access(filePath.c_str(), F_OK) == 0;
}

std::vector<std::string> UnTarFile::GetFileNames()
{
    if (file_names.empty()) {
        ParseTarFile();
    }

    return file_names;
}

int UnTarFile::UnPack(const char *path, uid_t owner)
{
    newOwner = owner;
    int ret = ParseTarFile(path, eUnpack);
    if (remove(m_srcPath.c_str()) != 0) {
        LOGI("delete failed");
    }
    return ret;
}

int UnTarFile::UnSplitPack(const char *path, uid_t owner)
{
    LOGI("Start UnSplitPack");
    newOwner = owner;
    isSplit = true;
    int num = 0;
    std::vector<std::string> taskSrcPathName;
    std::string pathDir(path);
    pathDir = pathDir.substr(0, pathDir.find_last_of('/'));

    if (!HandleCheckFile(m_srcPath.c_str(), taskSrcPathName, num)) {
        LOGE("UnTarFile::UnSplitPack, handleCheckFile failed!");
        return -1;
    }

    int ret = 0;
    if (FilePtr != nullptr) {
        (void)fclose(FilePtr);
        FilePtr = nullptr;
        LOGE("FilePtr is not null!");
    }
    for (int i = 0; i < num; i++) {
        FilePtr = fopen(taskSrcPathName[i].c_str(), "rb");
        if (FilePtr != nullptr) {
            int parseRet = ParseTarFile(pathDir.c_str(), eUnpack);
            if (parseRet != 0) {
                LOGE("UnTarFile::UnSplitPack, untar split failed!");
                (void)fclose(FilePtr);
                FilePtr = nullptr;
                return parseRet;
            } else {
                LOGI("UnTarFile::UnSplitPack, untar split suc!");
                (void)fclose(FilePtr);
                FilePtr = nullptr;
            }
        }
        if (remove(taskSrcPathName[i].c_str()) != 0) {
            LOGI("delete failed");
        }
    }

    LOGI("UnTarFile::UnSplitPack, untar split finish!");
    taskSrcPathName.clear();
    return ret;
}

// check slice tar file according the tar info
bool UnTarFile::CheckSliceTar(const char *tarInfo, const char *dstPathName, std::vector<std::string> &fileNameVector)
{
    if (tarInfo == nullptr) {
        LOGE("error, tarInfo is NULL");
        return false;
    }
    std::string info(tarInfo);
    size_t pos = info.find_last_of("|");
    std::string tarName(dstPathName);
    tarName.append("/");
    tarName.append(info.substr(0, pos));

    std::string subSize = info.substr(pos + 1);
    if (subSize.empty()) {
        LOGE("error subSize is empty");
        return false;
    }
    off_t size = strtol(subSize.c_str(), nullptr, 0);

    if (access(tarName.c_str(), F_OK)) {
        LOGE("error, slice tar file don't exist, error:%{public}s", strerror(errno));
        return true;
    }

    struct stat stSliceTar;
    if (stat(tarName.c_str(), &stSliceTar) == -1) {
        LOGE("error, failed to get stat of tar file, error: %{public}s", strerror(errno));
        return false;
    }

    if (size != stSliceTar.st_size) {
        LOGE("error, the size of the tar file is not equal to the size in the check file\n");
        return false;
    }
    fileNameVector.push_back(tarName);
    return true;
}

bool UnTarFile::HandleCheckFile(const char *tarBaseName, std::vector<std::string> &fileNameVector, int &num)
{
    LOGI("HandleCheckFile");
    if (tarBaseName == nullptr) {
        LOGE("tarBaseName is nullptr");
        return false;
    }

    FILE *fd = fopen(tarBaseName, "rb");
    if (fd == nullptr) {
        LOGE("open check file error, error:%{public}s", strerror(errno));
        return false;
    }

    std::string taskSPath(tarBaseName);
    std::string dirName = taskSPath.substr(0, taskSPath.find_last_of('/'));

    char tarInfo[PATH_MAX_LEN] = {0};
    bool ret = true;
    while ((fgets(tarInfo, (sizeof(tarInfo) - 1), fd)) != nullptr) {
        if (!CheckSliceTar(tarInfo, dirName.c_str(), fileNameVector)) {
            LOGE("handleCheckFile: failed");
            ret = false;
            break;
        }
        num++;
        memset_s(tarInfo, PATH_MAX_LEN, 0, PATH_MAX_LEN);
    }
    LOGI("HandleCheckFile end");
    (void)fclose(fd);
    fd = nullptr;
    return ret;
}
} // namespace installd
