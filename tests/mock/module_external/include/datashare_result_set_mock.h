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

#ifndef OHOS_FILEMGMT_BACKUP_DATASHARE_RESULT_SET_MOCK_H
#define OHOS_FILEMGMT_BACKUP_DATASHARE_RESULT_SET_MOCK_H

#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "datashare_result_set.h"

namespace OHOS {
using namespace DataShare;
namespace FileManagement::Backup {

class IDataShareResultSet {
public:
    IDataShareResultSet() = default;
    virtual ~IDataShareResultSet() = default;
    virtual int GetRowCount(int &count) = 0;
    virtual int GoToNextRow() = 0;
    virtual int GetColumnIndex(const std::string &columnName, int &columnIndex) = 0;
    virtual int GetInt(int columnIndex, int &value) = 0;
    virtual int GetLong(int columnIndex, int64_t &value) = 0;
public:
    static inline std::shared_ptr<IDataShareResultSet> idsrs = nullptr;
};

class DataShareResultSetMock : public IDataShareResultSet {
public:
    MOCK_METHOD(int, GetRowCount, (int &count), (override));
    MOCK_METHOD(int, GoToNextRow, ());
    MOCK_METHOD(int, GetColumnIndex, (const std::string &columnName, int &columnIndex));
    MOCK_METHOD(int, GetInt, (int columnIndex, int &value), (override));
    MOCK_METHOD(int, GetLong, (int columnIndex, int64_t &value), (override));
};
}
}
#endif
