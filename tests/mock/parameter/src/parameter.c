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

#include "parameter.h"

#include <string.h>

#include "securec.h"

static int g_bMockParameter = 0;

void SetMockParameter(bool para)
{
    g_bMockParameter = para;
}

bool GetMockParameter(void)
{
    return g_bMockParameter;
}

uint32_t FindParameter(const char *key)
{
    return 1;
}

int GetParameterValue(uint32_t handle, char *value, uint32_t len)
{
    errno_t ret = EOK;
    if (GetMockParameter()) {
        char src[] = "true";
        ret = strncpy_s(value, len, src, strlen(src));
    } else {
        char src[] = "false";
        ret = strncpy_s(value, len, src, strlen(src));
    }
    return ret == EOK ? 1 : -1;
}