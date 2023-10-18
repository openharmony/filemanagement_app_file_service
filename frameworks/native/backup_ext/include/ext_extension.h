/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H
#define OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H

#include <shared_mutex>
#include <string>
#include <vector>

#include "b_json/b_json_entity_extension_config.h"
#include "b_resources/b_constants.h"
#include "ext_backup_js.h"
#include "ext_extension_stub.h"
#include "thread_pool.h"

namespace OHOS::FileManagement::Backup {
class BackupExtExtension : public ExtExtensionStub {
public:
    UniqueFd GetFileHandle(const std::string &fileName) override;
    ErrCode HandleClear() override;
    ErrCode PublishFile(const std::string &fileName) override;
    ErrCode HandleBackup() override;
    ErrCode HandleRestore() override;

    void AsyncTaskRestoreForUpgrade(void);

public:
    explicit BackupExtExtension(const std::shared_ptr<Backup::ExtBackup> &extension) : extension_(extension)
    {
        threadPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
    }
    ~BackupExtExtension()
    {
        threadPool_.Stop();
    }

private:
    /**
     * @brief verify caller uid
     *
     */
    void VerifyCaller();

    /**
     * @brief backup
     *
     * @param usrConfig user configure
     */
    int DoBackup(const BJsonEntityExtensionConfig &usrConfig);

    /**
     * @brief restore
     *
     * @param fileName name of the file that to be untar
     */
    int DoRestore(const string &fileName);

    /** @brief clear backup restore data */
    void DoClear();

    /**
     * @brief extension backup restore is done
     *
     * @param errCode
     */
    void AppDone(ErrCode errCode);

    /**
     * @brief Executing Backup Tasks Asynchronously
     *
     * @param extAction action
     *
     * @param config user configure
     */
    void AsyncTaskBackup(const std::string config);

    /**
     * @brief Executing Restoration Tasks Asynchronously
     *
     */
    void AsyncTaskRestore();

    void AsyncTaskOnBackup();

private:
    std::shared_mutex lock_;
    std::shared_ptr<ExtBackup> extension_;
    std::vector<std::string> tars_;
    OHOS::ThreadPool threadPool_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H