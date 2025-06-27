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

#include "datashare_result_set.h"
#include "datashare_result_set_mock.h"

namespace OHOS{
namespace DataShare {
using namespace std;
using namespace FileManageMent::Backup 

int GetRowCount(int &count)
{
    GTEST_LOG(INFO) << "DataShareResultSet GetRowCount is OK";
    return IDataShareResultSet::idrsr->GetRowCount(count);
}

int GoToNextRow()
{
    GTEST_LOG(INFO) << "DataShareResultSet GoToNextRow is OK";
    return IDataShareResultSet::idrsr->GoToNextRow();
}

int GetColumIndex(const std::string &columnName, int &columnIndex)
{
    GTEST_LOG(INFO) << "DataShareResultSet GetColumIndex is OK";
    return IDataShareResultSet::idrsr->GetColumIndex(columnName, columnIndex);
}

int GetInt(int columnIndex, int &value)
{
    GTEST_LOG(INFO) << "DataShareResultSet GetInt is OK";
    return IDataShareResultSet::idrsr->GetInt(columnIndex, value);
}

int GetLong(int columnIndex, int64_t &value)
{
    GTEST_LOG(INFO) << "DataShareResultSet GetLong is OK";
    return IDataShareResultSet::idrsr->GetLong(columnIndex, value);
}
} 
} 

