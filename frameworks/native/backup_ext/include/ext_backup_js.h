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

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_H

#include <string_view>
#include <tuple>
#include <vector>

#include "b_resources/b_constants.h"
#include "ext_backup.h"
#include "js_runtime.h"
#include "native_reference.h"
#include "native_value.h"
#include "unique_fd.h"
#include "want.h"

namespace OHOS::FileManagement::Backup {
class ExtBackupJs : public ExtBackup {
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
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
              const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
              std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
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
     * @brief Create ExtBackupJs.
     *
     * @param runtime The runtime.
     * @return The ExtBackupJs instance.
     */
    static ExtBackupJs *Create(const std::unique_ptr<AbilityRuntime::Runtime> &runtime);

    /**
     * @brief Get the File Handle object
     *
     * @param fileName
     * @return UniqueFd
     */
    UniqueFd GetFileHandle(std::string &fileName);

    /**
     * @brief Get the Extension Action object
     *
     * @return BConstants::ExtensionAction
     */
    BConstants::ExtensionAction GetExtensionAction() const;

    /**
     * @brief Get the User Config, then check if
     *
     * @return allowed ro not
     */
    bool AllowToBackupRestore() const;

    /**
     * @brief Get the user configure
     *
     * @return user configure
     */
    std::string GetUsrConfig() const;

public:
    explicit ExtBackupJs(AbilityRuntime::JsRuntime &jsRuntime) : jsRuntime_(jsRuntime) {}
    ~ExtBackupJs()
    {
        jsRuntime_.FreeNativeReference(std::move(jsObj_));
    }

private:
    std::tuple<ErrCode, NativeValue *> CallObjectMethod(std::string_view name,
                                                        const std::vector<NativeValue *> &argv = {});

    AbilityRuntime::JsRuntime &jsRuntime_;
    std::unique_ptr<NativeReference> jsObj_;
    BConstants::ExtensionAction extAction_ {BConstants::ExtensionAction::INVALID};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_JS_H