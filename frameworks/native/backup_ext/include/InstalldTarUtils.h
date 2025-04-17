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

#ifndef PHONECLONE_INSTALLDTARUTILS_H
#define PHONECLONE_INSTALLDTARUTILS_H
#include <sys/types.h>
#include <string>
#include "securec.h"
#include "log.h"

namespace installd {
#define UID_GID_OFFSET 10000
#define APP_ID_START 10000
#define EOVERLAP_AND_RESET 182
#define TMAGIC "ustar" /* ustar and a null */
#define TMAGIC_LEN 6

#define TMODE_BASE 100
#define TMODE_LEN 8
#define TUID_BASE 108
#define TUID_LEN 8
#define TGID_BASE 116
#define TGID_LEN 8
#define TSIZE_BASE 124
#define TSIZE_LEN 12

#define CHKSUM_BASE 148

#define BLOCK_SIZE 512
#define BLANK_SPACE 0x20

#define PATH_MAX_LEN 2048
#define READ_BUFF_SIZE (512 * 1024)
const int MB_TO_BYTE = (1024 * 1024);

#define ERR_PARAM (-1)
#define ERR_NOEXIST (-2)
#define ERR_FORMAT (-3)
#define ERR_MALLOC (-4)
#define ERR_IO (-5)

#define REGTYPE '0' /* regular file */
#define AREGTYPE '\0' /* regular file */
#define SYMTYPE '2' /* reserved */
#define DIRTYPE '5' /* directory */
#define SPLIT_START_TYPE '8'
#define SPLIT_CONTINUE_TYPE '9'
#define SPLIT_END_TYPE 'A'

#define GNUTYPE_LONGLINK 'K'

#define PERMISSION_MASK 07777
#define MAX_FILESIZE 0777777777777LL
const int SIZE_OF_LONG_IN_32_SYSTEM = 4;

#define OCTSTRING_LENGTH (sizeof(off_t) * 3 + 1)

#define LONG_LINK_SYMBOL "././@LongLink"
#define VERSION "00"
#define SPACE ' '
#define SLASH '/'

#define DO_ERROR (-1)
#define DO_CONTINUE 0
#define DO_IGNORETHIS 1
#define DO_TRAVERSAL 2
#define DO_EXIT 3
#define SPLIT_SIZE (1024 * 1024 * 500) // 500M
#define FILTER_SIZE (1024 * 1024 * 100) // 100M

// callback
class TarCallBack {
public:
    virtual ~TarCallBack() {}

public:
    virtual bool IsAborted() = 0;
    virtual void OnTaskProgress(size_t szProcessed) = 0;
    virtual void OnPackagedUseBriefGenTar(long szProcessed) = 0;
    virtual void OnPackagedOneSplitTar(const char *name) = 0;
    virtual void WaitTaskLocked() = 0;
};

const int PATH_LENTH = 2048;
struct File_info {
    char path[PATH_LENTH];
    int mode;
    int type;
    long size;
    time_t modified_time;
};
} // namespace installd
#endif // PHONECLONE_INSTALLDTARUTILS_H