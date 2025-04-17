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
#include "InstalldTarUtils.h"
#include "TarUtil.h"

namespace installd {
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
    int ParseTarFile(const char *rootPath = "", UnTarFile::EParseType type = eList);
    bool IsEmptyBlock(const char *p);
    bool IsValidTarBlock(const TarHeader *tarHeader);
    bool VerifyChecksum(const TarHeader *tarHeader);
    bool CheckSliceTar(const char *tarInfo, const char *dstPathName, std::vector<std::string> &fileNameVector);
    bool HandleCheckFile(const char *tarBaseName, std::vector<std::string> &fileNameVector, int &num);
    void FreePointer(char *longName, char *longLink, char *fullPath);
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