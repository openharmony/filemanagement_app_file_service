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

#include "timer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace OHOS {
namespace Utils {

Timer::Timer(const std::string &name, int timeoutMs)
{
    GTEST_LOG_(INFO) << "Timer " << name << " ,timeoutMs " << timeoutMs;
    name_ = name;
}

Timer::~Timer() {}

uint32_t Timer::Setup()
{
    GTEST_LOG_(INFO) << "Timer Setup";
    return 1;
}

uint32_t Timer::Register(const TimerCallback &callback, uint32_t interval, bool once)
{
    GTEST_LOG_(INFO) << "Timer Register " << interval;
    callback();
    return 1;
}

void Timer::Shutdown(bool useJoin)
{
    GTEST_LOG_(INFO) << "Timer Shutdown " << useJoin;
}

void Timer::Unregister(uint32_t timerId)
{
    GTEST_LOG_(INFO) << "Timer Unregister " << timerId;
}
} // namespace Utils
} // namespace OHOS
