/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_DIR_H
#define OHOS_FILEMGMT_BACKUP_B_DIR_H

#include <linux/stat.h>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>
#include <vector>

#include "b_json/b_report_entity.h"
#include "b_radar/radar_app_statistic.h"
#include "errors.h"

namespace OHOS::FileManagement::Backup {
class BDir {
public:
    /**
     * @brief 读取指定目录下所有文件(非递归)
     *
     * @param 目录
     * @return 错误码、文件名数组
     */
    static std::tuple<ErrCode, std::vector<std::string>> GetDirFiles(const std::string &path);

    /**
     * @brief 校验文件父目录是否存在，不存在时创建
     *
     * @param 文件路径
     * @return 文件父目录是否已可用
     */
    static bool CheckAndCreateDirectory(const std::string &filePath);

    /**
     * @brief 从给定的includes和excludes目录及文件中获取所有有用大文件和其链接文件的集合
     *
     * @param includes 需要包含的文件及目录集合
     * @param excludes 需要排除的文件及目录集合
     * @return 错误码、大文件名集合
     */
    static std::tuple<ErrCode, std::map<std::string, struct stat>, std::map<std::string, size_t>> GetBigFiles(
        const std::vector<std::string> &includes, const std::vector<std::string> &excludes);

    /**
     * @brief Get the Dirs object
     *
     * @param paths 目录集合可带有通配符路径
     * @return std::vector<std::string> 目录集合
     */
    static std::vector<std::string> GetDirs(const std::vector<std::string_view> &paths);

    /**
     * @brief 从给定的includes和excludes目录中获取所有的大文件和小文件
     *
     * @param includes 需要包含的文件及目录集合
     * @param excludes 需要排除的文件及目录集合
     * @return 大文件和小文件的集合
     */
    static std::tuple<std::vector<std::string>, std::vector<std::string>> GetBackupList(
        const std::vector<std::string> &includes, const std::vector<std::string> &excludes);

    /**
     * @brief 获取bigfile和smaillfile的文件信息并生成清单
     *
     * @param bigFile 需要包含的文件及目录集合
     * @param smallFile 需要排除的文件及目录集合
     * @param allFiles 生成的所有文件信息清单
     * @param smallFiles 生成的小文件信息清单
     * @param bigFiles 生成的大文件信息清单
     * @return
     */
    static void GetUser0FileStat(std::vector<std::string> bigFile,
                                 std::vector<std::string> smallFile,
                                 std::vector<struct ReportFileInfo> &allFiles,
                                 std::vector<struct ReportFileInfo> &smallFiles,
                                 std::vector<struct ReportFileInfo> &bigFiles);

    /**
     * @brief 核实文件是否为异常无效路径
     *
     * @param filePath 待核实的路径
     * @return 是否是异常无效路径
     */
    static bool IsFilePathValid(const std::string &filePath);

    /**
     * @brief 判断目录列表是否包含路径
     */
    static bool IsDirsMatch(const std::vector<std::string> &excludePaths, const std::string &path);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_DIR_H