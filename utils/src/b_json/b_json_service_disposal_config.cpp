/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "unique_fd.h"
#include "b_json/b_json_service_disposal_config.h"

#include "cJSON.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
    const string PATH_BUNDLE_BACKUP_HOME = "/data/service/el2/100/backup/";
    const string CONFIG_NAME = "RestoreDisposalConfig.json";
}

BJsonDisposalConfig::BJsonDisposalConfig()
{
    string filePath = PATH_BUNDLE_BACKUP_HOME + CONFIG_NAME;
    if (access(filePath.c_str(), F_OK) == 0) {
        HILOGI("file exist filePath:%{public}s", filePath.c_str());
        return;
    }
    HILOGI("Failed to access filePath :%{public}s", filePath.c_str());
    UniqueFd fd(open(filePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR));
    if (fd < 0) {
        HILOGE("Failed to creat filePath :%{public}s", filePath.c_str());
        return;
    }
    cJSON *jsonObjectDis = cJSON_CreateObject();
    if (jsonObjectDis == nullptr) {
        HILOGE("Creat json failed");
        return;
    }
    cJSON *jsonArray = cJSON_CreateArray();
    if (jsonArray == nullptr) {
        HILOGE("Creat json failed");
        cJSON_Delete(jsonObjectDis);
        return;
    }
    cJSON_AddItemToObject(jsonObjectDis, "DispoasalConfigFile", jsonArray);

    char *newStr = cJSON_Print(jsonObjectDis);
    if (newStr == nullptr) {
        HILOGE("cJSON_Print json failed");
        cJSON_Delete(jsonObjectDis);
        return;
    }
    ofstream outFile(filePath);
    if (!outFile.is_open()) {
        HILOGE("open json failed");
        cJSON_free(newStr);
        cJSON_Delete(jsonObjectDis);
        return;
    }
    outFile << newStr;
    outFile.close();
    cJSON_free(newStr);
    cJSON_Delete(jsonObjectDis);
    HILOGI("Creat filePath ok :%{public}s", filePath.c_str());
}

bool BJsonDisposalConfig::AppendIntoDisposalConfigFile(const string& bundleName)
{
    string filePath = PATH_BUNDLE_BACKUP_HOME + CONFIG_NAME;
    ifstream inFile(filePath);
    if (!inFile.is_open()) {
        HILOGE("open json failed");
        return false;
    }
    string jsonString((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    cJSON *jsonObjectDis = cJSON_Parse(jsonString.c_str());
    if (jsonObjectDis == nullptr) {
        HILOGE("parse json failed");
        return false;
    }

    cJSON *configArray = cJSON_GetObjectItem(jsonObjectDis, "DispoasalConfigFile");
    if (configArray == nullptr || !cJSON_IsArray(configArray)) {
        HILOGE("parse json failed");
        cJSON_Delete(jsonObjectDis);
        return false;
    }
    cJSON *newItem = cJSON_CreateObject();
    if (newItem == nullptr) {
        HILOGE("parse json failed");
        cJSON_Delete(jsonObjectDis);
        return false;
    }

    cJSON_AddStringToObject(newItem, "bundleName", bundleName.c_str());
    cJSON_AddItemToArray(configArray, newItem);
    char *newStr = cJSON_Print(jsonObjectDis);
    if (newStr == nullptr) {
        HILOGE("cJSON_Print json failed");
        cJSON_Delete(jsonObjectDis);
        return false;
    }
    ofstream outFile(filePath);
    if (!outFile.is_open()) {
        HILOGE("open json failed");
        free(newStr);
        cJSON_Delete(jsonObjectDis);
        return false;
    }
    outFile << newStr;
    outFile.close();

    free(newStr);
    cJSON_Delete(jsonObjectDis);
    return true;
}

bool BJsonDisposalConfig::DeleteFromDisposalConfigFile(const string& bundleName)
{
    lock_guard<mutex> autoLock(fileMutex_);
    string filePath = PATH_BUNDLE_BACKUP_HOME + CONFIG_NAME;
    ifstream input(filePath);
    if (!input.is_open()) {
        HILOGE("open json failed");
        return false;
    }

    string jsonString((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
    input.close();

    cJSON *jsonObjectDis = cJSON_Parse(jsonString.c_str());
    if (jsonObjectDis == nullptr) {
        HILOGE("parse json failed");
        return false;
    }

    cJSON *configArray = cJSON_GetObjectItem(jsonObjectDis, "DispoasalConfigFile");
    if (configArray == nullptr || !cJSON_IsArray(configArray)) {
        cJSON_Delete(jsonObjectDis);
        return false;
    }
    for (int i = 0; i < cJSON_GetArraySize(configArray); ++i) {
        cJSON *item = cJSON_GetArrayItem(configArray, i);
        if (item != nullptr && cJSON_GetObjectItem(item, "bundleName") != nullptr &&
            cJSON_GetObjectItem(item, "bundleName")->type == cJSON_String &&
            cJSON_GetObjectItem(item, "bundleName")->valuestring == bundleName) {
            cJSON_DeleteItemFromArray(configArray, i);
            break;
        }
    }
    char *newStr = cJSON_Print(jsonObjectDis);
    if (newStr == nullptr) {
        HILOGE("cJSON_Print json failed");
        cJSON_Delete(jsonObjectDis);
        return false;
    }
    ofstream output(filePath);
    if (!output.is_open()) {
        HILOGE("open json failed");
        free(newStr);
        cJSON_Delete(jsonObjectDis);
        return false;
    }
    output << newStr;
    output.close();

    free(newStr);
    cJSON_Delete(jsonObjectDis);
    return true;
}

bool BJsonDisposalConfig::IfBundleNameInDisposalConfigFile(const string& bundleName)
{
    lock_guard<mutex> autoLock(fileMutex_);
    string filePath = PATH_BUNDLE_BACKUP_HOME + CONFIG_NAME;
    ifstream inFile(filePath);
    if (!inFile.is_open()) {
        HILOGE("open json failed");
        return false;
    }

    string jsonString((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    cJSON *jsonObjectDis = cJSON_Parse(jsonString.c_str());
    if (jsonObjectDis == nullptr) {
        HILOGE("parse json failed");
        return false;
    }

    cJSON *configArray = cJSON_GetObjectItem(jsonObjectDis, "DispoasalConfigFile");
    if (configArray == nullptr || !cJSON_IsArray(configArray)) {
        HILOGE("parse json failed");
        cJSON_Delete(jsonObjectDis);
        return false;
    }
    bool ifBundlename = false;
    for (int i = 0; i < cJSON_GetArraySize(configArray); ++i) {
        cJSON *item = cJSON_GetArrayItem(configArray, i);
        if (item != nullptr && cJSON_GetObjectItem(item, "bundleName") != nullptr &&
            cJSON_GetObjectItem(item, "bundleName")->type == cJSON_String &&
            cJSON_GetObjectItem(item, "bundleName")->valuestring == bundleName) {
            ifBundlename = true;
        }
    }

    if (!ifBundlename) {
        AppendIntoDisposalConfigFile(bundleName);
        HILOGI("AppendIntoDisposalConfigFile OK");
    }

    cJSON_Delete(jsonObjectDis);
    return true;
}

vector<string> BJsonDisposalConfig::GetBundleNameFromConfigFile()
{
    lock_guard<mutex> autoLock(fileMutex_);
    string filePath = PATH_BUNDLE_BACKUP_HOME + CONFIG_NAME;
    vector<string> bundleNameList;
    ifstream inFile(filePath);
    if (!inFile.is_open()) {
        HILOGE("open json failed");
        return {};
    }
    string jsonString((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    
    cJSON *jsonObjectDis = cJSON_Parse(jsonString.c_str());
    if (jsonObjectDis == nullptr) {
        HILOGE("parse json failed");
        return {};
    }

    cJSON *configArray = cJSON_GetObjectItem(jsonObjectDis, "DispoasalConfigFile");
    if (configArray == nullptr || !cJSON_IsArray(configArray)) {
        HILOGE("parse json failed");
        cJSON_Delete(jsonObjectDis);
        return {};
    }
    for (int i = 0; i < cJSON_GetArraySize(configArray); ++i) {
        cJSON *item = cJSON_GetArrayItem(configArray, i);
        if (item != nullptr && cJSON_GetObjectItem(item, "bundleName") != nullptr &&
            cJSON_GetObjectItem(item, "bundleName")->type == cJSON_String) {
            bundleNameList.emplace_back(cJSON_GetObjectItem(item, "bundleName")->valuestring);
        }
    }
    cJSON_Delete(jsonObjectDis);
    return bundleNameList;
}

bool BJsonDisposalConfig::DeleteConfigFile()
{
    string filePath = PATH_BUNDLE_BACKUP_HOME + CONFIG_NAME;
    if (access(filePath.c_str(), F_OK) != 0) {
        HILOGE("File is not exist");
        return false;
    }
    if (remove(filePath.c_str()) != 0) {
        HILOGE("Delete DisposalConfigFile failed");
        return false;
    }
    HILOGI("All Restore Finished, Delete DisposalConfigFile OK");
    return true;
}
} // namespace OHOS::FileManagement::Backup
