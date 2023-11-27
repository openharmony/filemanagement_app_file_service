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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H

#include "b_resources/b_constants.h"
#include "ext_backup_context.h"
#include "extension_base.h"
#include "runtime.h"
#include "want.h"

namespace OHOS::FileManagement::Backup {

class ExtBackup;
using CreatorFunc = std::function<ExtBackup *(const std::unique_ptr<AbilityRuntime::Runtime> &runtime)>;

class ExtBackup : public AbilityRuntime::ExtensionBase<ExtBackupContext> {
public:
    /**
     * @brief Called when this extension is started. You must override this function if you want to perform some
     *        initialization operations during extension startup.
     *
     * This function can be called only once in the entire lifecycle of an extension.
     * @param Want Indicates the {@link Want} structure containing startup information about the extension.
     */
    void OnStart(const AAFwk::Want &want) override;

    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    void Init(const std::shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
              const std::shared_ptr<AbilityRuntime::OHOSApplication> &application,
              std::shared_ptr<AbilityRuntime::AbilityHandler> &handler,
              const sptr<IRemoteObject> &token) override;

    /**
     * @brief Called back when Service is started.
     * This method can be called only by Service. You can use the StartAbility(ohos.aafwk.content.Want) method to start
     * Service. Then the system calls back the current method to use the transferred want parameter to execute its own
     * logic.
     *
     * @param want Indicates the want of Service to start.
     * @param restart Indicates the startup mode. The value true indicates that Service is restarted after being
     * destroyed, and the value false indicates a normal startup.
     * @param startId Indicates the number of times the Service extension has been started. The startId is
     * incremented by 1 every time the extension is started. For example, if the extension has been started
     * for six times, the value of startId is 6.
     */
    void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;

    /**
     * @brief Called when this backup extension ability is connected for the first time.
     *
     * You can override this function to implement your own processing logic.
     *
     * @param want Indicates the {@link Want} structure containing connection information about the backup
     * extension.
     * @return Returns a pointer to the <b>sid</b> of the connected backup extension ability.
     */
    sptr<IRemoteObject> OnConnect(const AAFwk::Want &want) override;

    /**
     * @brief Called when all abilities connected to this Wallpaper extension are disconnected.
     *
     * You can override this function to implement your own processing logic.
     *
     */
    void OnDisconnect(const AAFwk::Want &want) override;

public:
    /**
     * @brief Create Extension.
     *
     * @param runtime The runtime.
     * @return The ServiceExtension instance.
     */
    static ExtBackup *Create(const std::unique_ptr<AbilityRuntime::Runtime> &runtime);

    /**
     * @brief Get the Extension Action object
     *
     * @return BConstants::ExtensionAction
     */
    virtual BConstants::ExtensionAction GetExtensionAction() const;

    /**
     * @brief Get the User Config, then check if
     *
     * @return allowed ro not
     */
    virtual bool AllowToBackupRestore() const;

    /**
     * @brief Get whether FullBackupOnly or not
     *
     * @return FullBackupOnly ro not
     */
    virtual bool UseFullBackupOnly(void) const;

    /**
     * @brief Get the user configure
     *
     * @return user configure
     */
    virtual std::string GetUsrConfig() const;

    /**
     * @brief do backup. Subclasses can inherit to implement their own custom functionality.
     */
    virtual ErrCode OnBackup(std::function<void()> callback);

    /**
     * @brief Called do restore.
     */
    virtual ErrCode OnRestore(std::function<void()> callback);

    /**
     * @brief 数据迁移判断
     *
     * @return true
     * @return false
     */
    bool WasFromSpeicalVersion(void);

    /**
     * @brief Version for clone and cloud
     *
     * @return true
     * @return false
     */
    bool SpeicalVersionForCloneAndCloud(void);

    /**
     * @brief 数据以准备就绪
     *
     * @return true
     * @return false
     */
    bool RestoreDataReady();

public:
    ExtBackup() = default;
    ~ExtBackup() override = default;

    static void SetCreator(const CreatorFunc &creator);

protected:
    std::string appVersionStr_;
    int appVersionCode_;
    int restoreType_;

private:
    BConstants::ExtensionAction VerifyAndGetAction(const AAFwk::Want &want,
                                                   std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo);

    ErrCode GetParament(const AAFwk::Want &want);

    BConstants::ExtensionAction extAction_ {BConstants::ExtensionAction::INVALID};

    static CreatorFunc creator_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H
