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

namespace OHOS {
namespace DataShare {
using namespace std;
using namespace FileManagement::Backup;

int GetRowCount(int &count)
{
    GTEST_LOG_(INFO) << "DataShareResultSet GetRowCount is OK";
    return IDataShareResultSet::idsrs->GetRowCount(count);
}

int GoToNextRow()
{
    GTEST_LOG_(INFO) << "DataShareResultSet GoToNextRow is OK";
    return IDataShareResultSet::idsrs->GoToNextRow();
}

int GetColumnIndex(const std::string &columnName, int &columnIndex)
{
    GTEST_LOG_(INFO) << "DataShareResultSet GetColumnIndex is OK";
    return IDataShareResultSet::idsrs->GetColumnIndex(columnName, columnIndex);
}

int GetInt(int columnIndex, int &value)
{
    GTEST_LOG_(INFO) << "DataShareResultSet GetInt is OK";
    return IDataShareResultSet::idsrs->GetInt(columnIndex, value);
}

int GetLong(int columnIndex, int64_t &value)
{
    GTEST_LOG_(INFO) << "DataShareResultSet GetLong is OK";
    return IDataShareResultSet::idsrs->GetLong(columnIndex, value);
}
}
}