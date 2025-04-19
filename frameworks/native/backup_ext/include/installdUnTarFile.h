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

#ifndef PHONECLONE_INSTALLDUNTARFILE_H
#define PHONECLONE_INSTALLDUNTARFILE_H
#include <vector>
#include <string>
#include "installdTarUtils.h"
#include "tarUtil.h"

namespace installd {

struct ParseTarPath {
    char *longName = nullptr;
    char *longLink = nullptr;
    char *fullPath = nullptr;
    char *realName = nullptr;
    char *realLink = nullptr;
};

struct TarFileInfo {
    off_t fileSize;
    off_t fileBlockCnt;
    off_t pos;
};

// helper function.
off_t ParseOctalStr(const char *p, size_t n);

class UnTarFile {
public:
    UnTarFile(const char *TarPath);
    virtual ~UnTarFile(void);

public:
    std::vector<std::string> GetFileNames();
    int UnSplitPack(const char *path, uid_t owner = 0);
    int UnPack(const char *path, uid_t owner = 0);
    void Reset();
    bool CheckIsSplitTar(const std::string &tarFile, const std::string &rootpath);
    int UnSplitTar(const std::string &tarFile, const std::string &rootpath);

public:
    typedef enum { eList = 0, eUnpack = 1, eCheckSplit = 2 } EParseType;

private:
    bool IsProcessTarEnd(char *buff, int &ret);
    int ParseTarFile(const char *rootPath = "", EParseType type = eList);
    bool IsEmptyBlock(const char *p);
    int CheckFileAndInitPath(const char *rootPath, ParseTarPath *parseTarPath);
    void SetFileChmodAndChown(char *buff, ParseTarPath *parseTarPath, bool &isSoftLink);
    void HandleGnuLongLink(ParseTarPath *parseTarPath, bool &isSkip, TarFileInfo &tarFileInfo);
    void HandleGnuLongName(ParseTarPath *parseTarPath, bool &isSkip, TarFileInfo &tarFileInfo);
    void HandleRegularFile(char *buff, EParseType type, ParseTarPath *parseTarPath, bool &isSkip,
        TarFileInfo &tarFileInfo);
    bool FileReadAndWrite(char *destBuff, FILE *destF, size_t readBuffSize);
    void HandleRegularEUnpackFile(char *buff, ParseTarPath *parseTarPath, bool &isSkip, TarFileInfo &tarFileInfo);
    bool ProcessTarBlock(char *buff, EParseType type, ParseTarPath *parseTarPath, bool &isSkip, bool &isSoftLink);
    bool IsValidTarBlock(const TarHeader *tarHeader);
    bool VerifyChecksum(const TarHeader *tarHeader);
    bool CheckSliceTar(const char *tarInfo, const char *dstPathName, std::vector<std::string> &fileNameVector);
    bool HandleCheckFile(const char *tarBaseName, std::vector<std::string> &fileNameVector, int &num);
    void FreePointer(ParseTarPath *parseTarPath);
    void FreeLongTypePointer(ParseTarPath *parseTarPath);
    bool CreateDirWithRecursive(const std::string &filePath, mode_t mode = (mode_t)448);

private:
    FILE *FilePtr;
    off_t tarSize;
    uid_t newOwner;
    std::string m_srcPath;
    bool isSplit = false;
    std::vector<std::string> file_names;
    std::vector<off_t> file_sizes;
    std::vector<off_t> file_data_addrs;
};
} // namespace installd
#endif // PHONECLONE_INSTALLDUNTARFILE_H