/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef OHOS_FILEMGMT_BACKUP_CJSON_MOCK_H
#define OHOS_FILEMGMT_BACKUP_CJSON_MOCK_H

#include <cJSON.h>
#include <gmock/gmock.h>

namespace OHOS::FileManagement::Backup {
class CJson {
public:
    CJson() = default;
    virtual ~CJson() = default;

public:
    virtual cJSON *cJSON_CreateArray() = 0;
    virtual cJSON *cJSON_CreateObject() = 0;
    virtual char *cJSON_Print(const cJSON *item) = 0;
    virtual cJSON *cJSON_Parse(const char *value) = 0;
    virtual cJSON *cJSON_GetObjectItem(const cJSON *const object, const char *const string) = 0;
    virtual void cJSON_Delete(cJSON *item) = 0;
    virtual cJSON_bool cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item) = 0;
    virtual int cJSON_GetArraySize(const cJSON *array) = 0;
    virtual cJSON* cJSON_GetArrayItem(const cJSON* array, int index) = 0;
    virtual void cJSON_free(void* object) = 0;
    virtual cJSON_bool cJSON_AddItemToArray(cJSON *array, cJSON *item) = 0;
    virtual cJSON *cJSON_AddStringToObject(cJSON *const object, const char *const name, const char *const string) = 0;
    virtual cJSON_bool cJSON_IsArray(const cJSON * const item) = 0;

public:
    static inline std::shared_ptr<CJson> cJsonPtr = nullptr;
};

class CJsonMock : public CJson {
public:
    MOCK_METHOD3(cJSON_AddItemToObject, cJSON_bool(cJSON *object, const char *string, cJSON *item));
    MOCK_METHOD1(cJSON_Delete, void(cJSON *item));
    MOCK_METHOD0(cJSON_CreateObject, cJSON *());
    MOCK_METHOD0(cJSON_CreateArray, cJSON *());
    MOCK_METHOD1(cJSON_Print, char *(const cJSON *item));
    MOCK_METHOD1(cJSON_Parse, cJSON *(const char *value));
    MOCK_METHOD2(cJSON_GetObjectItem, cJSON *(const cJSON *const object, const char *const string));
    MOCK_METHOD1(cJSON_GetArraySize, int(const cJSON *array));
    MOCK_METHOD2(cJSON_GetArrayItem, cJSON *(const cJSON* array, int index));
    MOCK_METHOD2(cJSON_AddItemToArray, cJSON_bool(cJSON *array, cJSON *item));
    MOCK_METHOD3(cJSON_AddStringToObject,
                 cJSON *(cJSON *const object, const char *const name, const char *const string));
    MOCK_METHOD1(cJSON_free, void(void* object));
    MOCK_METHOD1(cJSON_IsArray, cJSON_bool(const cJSON * const item));
};
} // namespace OHOS::FileManagement::Backup
#endif