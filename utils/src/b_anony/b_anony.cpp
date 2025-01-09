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

#include "b_anony/b_anony.h"

#include "securec.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

std::string GetAnonyString(const std::string &value)
{
    const size_t shortPlaintextLength = 20; // 字符串长度小于20，只明文1字节
    const size_t plaintextLength = 4; // 字符串长度大于20，明文4字节
    const size_t anonyLength = 3; // 字符串长度小于3, 则完全匿名
    std::string result;
    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < anonyLength) {
        return tmpStr;
    }

    if (strLen <= shortPlaintextLength) {
        result += value[0];
        result += tmpStr;
        result += value[strLen - 1];
    } else {
        result += value.substr(0, plaintextLength);
        result += tmpStr;
        result += value.substr(strLen - plaintextLength, plaintextLength);
    }

    return result;
}

std::string GetAnonyPath(const std::string &value)
{
    std::string res;
    size_t pos = 0;
    size_t found = value.find('/');
    while (found != std::string::npos) {
        if (pos != found) {
            res += GetAnonyString(value.substr(pos, found - pos));
        }
        res += '/';
        pos = found + 1;
        found = value.find('/', pos);
    }
    //处理文件名
    if (pos < value.length()) {
        found = value.find('.', pos);
        if (found != std::string::npos) {
            res += GetAnonyString(value.substr(pos, found - pos));
            res += value.substr(found);
        } else {
            res += GetAnonyString(value.substr(pos));
        }
    }
    return res;
}
}