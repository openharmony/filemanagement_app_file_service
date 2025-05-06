/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_FILE_H
#define OHOS_FILEMGMT_BACKUP_B_FILE_H

#include <memory>
#include <string>

#include "unique_fd.h"
#include "json/json.h"

#include "b_anony/b_anony.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
class BFile {
public:
    /**
     * @brief 一次性读取文件全部内容
     *
     * @param fd 文件描述符
     * @return std::unique_ptr<char[]> 文件全部内容，保证最后一个字节为'\0'
     * @throw std::system_error IO异常
     */
    static std::unique_ptr<char[]> ReadFile(const UniqueFd &fd);

    /**
     * @brief linux sendfile 二次封装
     * @param outFd 参数是待写入内容的文件描述符
     * @param inFd 参数是待读出内容的文件描述符
     * @throw std::system_error IO异常
     */
    static void SendFile(int outFd, int inFd);

    /**
     * @brief linux write 二次封装
     * @param fd 参数是待写入内容的文件描述符
     * @param str 待写入文件的字符串
     * @throw std::system_error IO异常
     */
    static void Write(const UniqueFd &fd, const string &str);

    /**
     * @brief copy file from old path to new path
     *
     * @param from old path
     * @param to new path
     * @return true copy succeess
     * @return false some error occur
     */
    static bool CopyFile(const string &from, const string &to);

    /**
     * @brief move file from old path to new path
     *
     * @param from old path
     * @param to new path
     * @return true move succeess
     * @return false some error occur
     */
    static bool MoveFile(const string &from, const string &to);

    /**
     * @brief get real path
     *
     * @param path src path
     * @param realPath real path
     * @return true get real path success
     * @return false some error occur
     */
    static bool GetRealPath(const string &path, string &realPath);

    /**
     * @brief check if string is endswith suffix
     *
     * @param str str
     * @param suffix suffix str
     * @return true str is endswith suffix
     * @return false str is not endswith suffix
     */
    static bool EndsWith(const string &str, const string &suffix);

        /**
     * @brief get file size by path
     *
     * @param path file path
     * @return 0 if path is invalid
     * @return file size in byte if path is valid
     */
    static uint64_t GetFileSize(const string &path);
private:
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_FILE_H