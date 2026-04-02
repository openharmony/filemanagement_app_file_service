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

#include "parcel.h"
#include <gtest/gtest.h>

namespace OHOS::FileManagement::Backup {
using namespace std;
namespace {
bool g_mockWriteUint32 = true;
bool g_mockWriteInt32 = true;
bool g_mockWriteUint64 = true;
bool g_mockWriteInt64 = true;
bool g_mockWriteString = true;
bool g_mockWriteParcelable = true;
bool g_mockReadParcelable = true;
uint8_t g_mockWriteStringCount = 0;
uint8_t g_mockWriteStringMax = 0;
std::vector<bool> g_mockSpecialSeq;
size_t seqCurId = 0;
} // namespace

void MockWriteUint32(bool state)
{
    g_mockWriteUint32 = state;
}

void MockWriteInt32(bool state)
{
    g_mockWriteInt32 = state;
}

void MockWriteUint64(bool state)
{
    g_mockWriteUint64 = state;
}

void MockWriteInt64(bool state)
{
    g_mockWriteInt64 = state;
}

void MockWriteString(bool state, uint8_t count)
{
    g_mockWriteString = state;
    g_mockWriteStringMax = count;
}

void MockWriteParcelable(bool state)
{
    g_mockWriteParcelable = state;
}

void MockReadParcelable(bool state)
{
    g_mockReadParcelable = state;
}

void ResetParcelState()
{
    g_mockWriteUint32 = true;
    g_mockWriteInt32 = true;
    g_mockWriteUint64 = true;
    g_mockWriteInt64 = true;
    g_mockWriteString = true;
    g_mockWriteParcelable = true;
    g_mockReadParcelable = true;
    g_mockWriteStringCount = 0;
    g_mockWriteStringMax = 0;
    g_mockSpecialSeq.clear();
    seqCurId = 0;
}

bool GetMockReadParcelableState()
{
    return g_mockReadParcelable;
}

void SetSpecialParcelSequence(const std::vector<bool> &sequence)
{
    g_mockSpecialSeq = sequence;
    seqCurId = 0;
}

void Parcel::SetEasyMode(bool value)
{
    easyMode_ = value;
}

void Parcel::Reset()
{
    easyMode_ = true;
    std::queue<std::any>().swap(values_); // clear
}

bool Parcel::WriteUint32(uint32_t value)
{
    bool isSuccess = g_mockWriteUint32;
    if (easyMode_) {
        return isSuccess;
    }
    if (seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    }
    if (isSuccess) {
        values_.push(value);
        GTEST_LOG_(INFO) << "WriteUint32: " << value;
    }
    return isSuccess;
}

bool Parcel::WriteInt32(int32_t value)
{
    bool isSuccess = g_mockWriteInt32;
    if (easyMode_) {
        return isSuccess;
    }
    if (seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    }
    if (isSuccess) {
        values_.push(value);
        GTEST_LOG_(INFO) << "WriteInt32: " << value;
    }
    return isSuccess;
}

bool Parcel::WriteUint64(uint64_t value)
{
    bool isSuccess = g_mockWriteUint64;
    if (easyMode_) {
        return isSuccess;
    }
    if (seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    }
    if (isSuccess) {
        values_.push(value);
        GTEST_LOG_(INFO) << "WriteUint64: " << value;
    }
    return isSuccess;
}

bool Parcel::WriteInt64(int64_t value)
{
    bool isSuccess = g_mockWriteInt64;
    if (easyMode_) {
        return isSuccess;
    }
    if (seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    }
    if (isSuccess) {
        values_.push(value);
        GTEST_LOG_(INFO) << "WriteInt64: " << value;
    }
    return isSuccess;
}

bool Parcel::WriteString(const string & value)
{
    bool isSuccess = g_mockWriteString;
    if (!easyMode_ && seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    } else if (easyMode_ && g_mockWriteStringCount < g_mockWriteStringMax) {
        g_mockWriteStringCount++;
        isSuccess = !g_mockWriteString;
    }
    if (!easyMode_ && isSuccess) {
        values_.push(value);
        GTEST_LOG_(INFO) << "WriteString: " << value;
    }
    return isSuccess;
}

bool Parcel::WriteParcelable(const Parcelable *)
{
    return g_mockWriteParcelable;
}

bool Parcel::ReadString(string &value)
{
    bool isSuccess = g_mockWriteString;
    if (!easyMode_ && seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    } else if (easyMode_ && g_mockWriteStringCount < g_mockWriteStringMax) {
        g_mockWriteStringCount++;
        isSuccess = !g_mockWriteString;
    }
    if (!easyMode_ && isSuccess) {
        try {
            value = std::any_cast<std::string>(values_.front());
            values_.pop();
            GTEST_LOG_(INFO) << "ReadString: " << value;
        } catch (const std::bad_any_cast& e) {
            GTEST_LOG_(INFO) << "ReadString any_cast error: " << e.what();
            isSuccess = false;
        } catch (std::exception& e) {
            GTEST_LOG_(INFO) << "ReadString error: " << e.what();
            isSuccess = false;
        } catch (...) {
            GTEST_LOG_(INFO) << "ReadString error";
            isSuccess = false;
        }
    }
    return isSuccess;
}

bool Parcel::ReadUint32(uint32_t &value)
{
bool isSuccess = g_mockWriteUint32;
    if (easyMode_) {
        return isSuccess;
    }
    if (seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    }
    if (isSuccess) {
        try {
            value = std::any_cast<uint32_t>(values_.front());
            values_.pop();
            GTEST_LOG_(INFO) << "ReadUint32: " << value;
        } catch (const std::bad_any_cast& e) {
            GTEST_LOG_(INFO) << "ReadUint32 any_cast error: " << e.what();
            isSuccess = false;
        } catch (std::exception& e) {
            GTEST_LOG_(INFO) << "ReadUint32 error: " << e.what();
            isSuccess = false;
        } catch (...) {
            GTEST_LOG_(INFO) << "ReadUint32 error";
            isSuccess = false;
        }
    }
    return isSuccess;
}

bool Parcel::ReadInt32(int32_t &value)
{
    bool isSuccess = g_mockWriteInt32;
    if (easyMode_) {
        return isSuccess;
    }
    if (seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    }
    if (isSuccess) {
        try {
            value = std::any_cast<int32_t>(values_.front());
            values_.pop();
            GTEST_LOG_(INFO) << "ReadInt32: " << value;
        } catch (const std::bad_any_cast& e) {
            GTEST_LOG_(INFO) << "ReadInt32 any_cast error: " << e.what();
            isSuccess = false;
        } catch (std::exception& e) {
            GTEST_LOG_(INFO) << "ReadInt32 error: " << e.what();
            isSuccess = false;
        } catch (...) {
            GTEST_LOG_(INFO) << "ReadInt32 error";
            isSuccess = false;
        }
    }
    return isSuccess;
}

bool Parcel::ReadUint64(uint64_t &value)
{
    bool isSuccess = g_mockWriteUint64;
    if (easyMode_) {
        return isSuccess;
    }
    if (seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    }
    if (isSuccess) {
        try {
            value = std::any_cast<uint64_t>(values_.front());
            values_.pop();
            GTEST_LOG_(INFO) << "ReadUint64: " << value;
        } catch (const std::bad_any_cast& e) {
            GTEST_LOG_(INFO) << "ReadUint64 any_cast error: " << e.what();
            isSuccess = false;
        } catch (std::exception& e) {
            GTEST_LOG_(INFO) << "ReadUint64 error: " << e.what();
            isSuccess = false;
        } catch (...) {
            GTEST_LOG_(INFO) << "ReadUint64 error";
            isSuccess = false;
        }
    }
    return isSuccess;
}

bool Parcel::ReadInt64(int64_t &value)
{
    bool isSuccess = g_mockWriteInt64;
    if (easyMode_) {
        return isSuccess;
    }
    if (seqCurId < g_mockSpecialSeq.size()) {
        isSuccess = g_mockSpecialSeq[seqCurId];
        seqCurId++;
    }
    if (isSuccess) {
        try {
            value = std::any_cast<int64_t>(values_.front());
            values_.pop();
            GTEST_LOG_(INFO) << "ReadInt64: " << value;
        } catch (const std::bad_any_cast& e) {
            GTEST_LOG_(INFO) << "ReadInt64 any_cast error: " << e.what();
            isSuccess = false;
        } catch (std::exception& e) {
            GTEST_LOG_(INFO) << "ReadInt64 error: " << e.what();
            isSuccess = false;
        } catch (...) {
            GTEST_LOG_(INFO) << "ReadInt64 error";
            isSuccess = false;
        }
    }
    return isSuccess;
}
} // namespace OHOS::FileManagement::Backup