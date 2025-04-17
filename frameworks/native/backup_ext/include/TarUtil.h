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

#ifndef TAR_UTIL_H
#define TAR_UTIL_H

#include <cstdint>

class TarUtil {
public:
    static const int32_t MODE_MASK = 07777;
    static const int32_t NAME_LEN = 100;
    static const int32_t MODE_LEN = 8;
    static const int32_t SIZE_LEN = 12;
    static const int32_t CHKSUM_LEN = 8;
    static const int32_t BLOCK_LEN = 512;
    static const int32_t MAX_PATH_LEN = 4096; // Linux系统中的文件路径长度上限为4096个字符
    static const int32_t RENAME_START_CNT = 1;
    static const int32_t CHKSUM_ASCII_VALUE = 256;
    static const char GNUTYPE_LONGNAME = 'L';
};

enum RESULT {
    OK = 0,
    ERROR = -1,
    NULL_POINTER = -2,
    LIST_EMPTY = -3,
    PATH_EMPTY = -4,
    OPEN_FILE_FAIL = -5,
    FILE_NOT_EXIST = -6,
    FILE_UNREADABLE = -7,
    READ_INCOMPLETE = -8,
    WRITE_INCOMPLETE = -9,
    FILE_REMOVE_FAIL = -10,
    MAKE_DIR_FAIL = -11,
};

typedef struct {        /* byte offset */
    char name[100];     /*   0 */
    char mode[8];       /* 100 */
    char uid[8];        /* 108 */
    char gid[8];        /* 116 */
    char size[12];      /* 124 */
    char mtime[12];     /* 136 */
    char chksum[8];     /* 148 */
    char typeflag;      /* 156 */
    char linkname[100]; /* 157 */
    char magic[6];      /* 257 */
    char version[2];    /* 263 */
    char uname[32];     /* 265 */
    char gname[32];     /* 297 */
    char devmajor[8];   /* 329 */
    char devminor[8];   /* 337 */
    char prefix[155];   /* 345 */
    char paddings[12];  /* 500 */
} TarHeader;

#endif // TAR_UTIL_H