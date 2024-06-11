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

#include "b_json/b_report_entity.h"

#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "sandbox_helper.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
namespace {
const char ATTR_SEP = ';';
const char LINE_SEP = '\n';
const char LINE_WRAP = '\r';
const int64_t HASH_BUFFER_SIZE = 4096; // 每次读取的siz
const int INFO_ALIGN_NUM = 2;
const int BUFFER_LENGTH = 2;
const string INFO_DIR = "dir";
const string INFO_HASH = "hash";
const string INFO_IS_INCREMENTAL = "isIncremental";
const string INFO_MODE = "mode";
const string INFO_MTIME = "mtime";
const string INFO_PATH = "path";
const string INFO_SIZE = "size";
const string INFO_ENCODE_FLAG = "encodeFlag";
} // namespace

static vector<string> SplitStringByChar(const string &str, const char &sep)
{
    vector<string> splits;
    string newStr = str;
    if (str.empty() ||  str.size() < 1) {
        return splits;
    }
    if (str.rfind(sep) == str.size() - 1) {
        newStr += sep;
    }
    stringstream ss(newStr);
    string res;
    while (getline(ss, res, sep)) {
        splits.push_back(res);
    }

    return splits;
}

static void ParseSplitsItem(const vector<string> &splits, const unordered_map<string, int> &keys,
    vector<string> &residue, string &path)
{
    size_t splitsLen = splits.size();
    for (size_t i = 0; i < splitsLen; i++) {
        if (i <= splitsLen - keys.size()) {
            path += splits[i] + ";";
        } else {
            residue.emplace_back(splits[i]);
        }
    }
}

static ErrCode ParseReportInfo(struct ReportFileInfo &fileStat,
                               const vector<string> &splits,
                               const unordered_map<string, int> &keys)
{
    // 根据数据拼接结构体
    // 处理path路径
    string path;
    vector<string> residue;
    try {
        // 识别path字段与其他字段
        ParseSplitsItem(splits, keys, residue, path);
        if (residue.size() != keys.size() - 1) {
            HILOGE("Error residue size");
            return EPERM;
        }
        if (keys.find(INFO_ENCODE_FLAG) != keys.end()) {
            fileStat.encodeFlag = residue[keys.find(INFO_ENCODE_FLAG)->second] == "1" ? true : false;
        }
        path = (path.length() > 0 && path[0] == '/') ? path.substr(1, path.length() - 1) : path;
        auto fileRawPath = (path.length() > 0) ? path.substr(0, path.length() - 1) : path;
        fileStat.filePath = BReportEntity::DecodeReportItem(fileRawPath, fileStat.encodeFlag);
        HILOGD("Briefings file %{public}s", fileStat.filePath.c_str());
        if (keys.find(INFO_MODE) != keys.end()) {
            fileStat.mode = residue[keys.find(INFO_MODE)->second];
        }
        if (keys.find(INFO_DIR) != keys.end()) {
            fileStat.isDir = residue[keys.find(INFO_DIR)->second] == "1" ? true : false;
        }
        if (keys.find(INFO_SIZE) != keys.end()) {
            stringstream sizeStr(residue[keys.find(INFO_SIZE)->second]);
            off_t size = 0;
            sizeStr >> size;
            fileStat.size = size;
        }
        if (keys.find(INFO_MTIME) != keys.end()) {
            stringstream mtimeStr(residue[keys.find(INFO_MTIME)->second]);
            off_t mtime = 0;
            mtimeStr >> mtime;
            fileStat.mtime = mtime;
        }
        if (keys.find(INFO_HASH) != keys.end()) {
            fileStat.hash = residue[keys.find(INFO_HASH)->second];
        }
        if (keys.find(INFO_IS_INCREMENTAL) != keys.end()) {
            fileStat.isIncremental = residue[keys.find(INFO_IS_INCREMENTAL)->second] == "1" ? true : false;
        }
        return ERR_OK;
    } catch (...) {
        HILOGE("Failed to ParseReportInfo");
        return EPERM;
    }
}

static void DealLine(unordered_map<string, int> &keys,
                     int &num,
                     const string &line,
                     unordered_map<string, struct ReportFileInfo> &infos)
{
    if (line.empty()) {
        return;
    }

    string currentLine = line;
    if (currentLine[currentLine.length() - 1] == LINE_WRAP) {
        currentLine.pop_back();
    }

    vector<string> splits = SplitStringByChar(currentLine, ATTR_SEP);
    if (num < INFO_ALIGN_NUM) {
        if (num == 1) {
            for (int j = 0; j < (int)splits.size(); j++) {
                keys.emplace(splits[j], j - 1);
            }
        }
        num++;
    } else {
        struct ReportFileInfo fileState;
        auto code = ParseReportInfo(fileState, splits, keys);
        if (code != ERR_OK) {
            HILOGE("ParseReportInfo err:%{public}d, %{public}s", code, currentLine.c_str());
        } else {
            infos.try_emplace(fileState.filePath, fileState);
        }
    }
}

static struct ReportFileInfo StorageDealLine(unordered_map<string, int> &keys, int &num, const string &line)
{
    if (line.empty()) {
        return {};
    }

    string currentLine = line;
    if (currentLine[currentLine.length() - 1] == LINE_WRAP) {
        currentLine.pop_back();
    }

    vector<string> splits = SplitStringByChar(currentLine, ATTR_SEP);
    if (num < INFO_ALIGN_NUM) {
        if (num == 1) {
            for (int j = 0; j < (int)splits.size(); j++) {
                keys.emplace(splits[j], j - 1);
            }
        }
        num++;
    } else {
        struct ReportFileInfo fileState;
        auto code = ParseReportInfo(fileState, splits, keys);
        if (code != ERR_OK) {
            HILOGE("ParseReportInfo err:%{public}d, %{public}s", code, currentLine.c_str());
        } else {
            return fileState;
        }
    }
    return {};
}

unordered_map<string, struct ReportFileInfo> BReportEntity::GetReportInfos() const
{
    unordered_map<string, struct ReportFileInfo> infos {};

    char buffer[HASH_BUFFER_SIZE];
    ssize_t bytesRead;
    string currentLine;
    unordered_map<string, int> keys;

    int num = 0;
    while ((bytesRead = read(srcFile_, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            if (buffer[i] == LINE_SEP) {
                DealLine(keys, num, currentLine, infos);
                currentLine.clear();
            } else {
                currentLine += buffer[i];
            }
        }
    }

    // 处理文件中的最后一行
    if (!currentLine.empty()) {
        DealLine(keys, num, currentLine, infos);
    }

    return infos;
}

bool BReportEntity::GetStorageReportInfos(struct ReportFileInfo &fileStat)
{
    char buffer[BUFFER_LENGTH];
    ssize_t bytesRead;
    string currentLine;
    static unordered_map<string, int> keys;
    static int num = 0;

    if ((bytesRead = read(srcFile_, buffer, 1)) <= 0) {
        keys = {};
        num = 0;
        return false;
    }
    do {
        if (buffer[0] != LINE_SEP && buffer[0] != '\0') {
            currentLine += buffer[0];
        } else {
            currentLine += LINE_SEP;
            fileStat = StorageDealLine(keys, num, currentLine);
            return true;
        }
    } while ((bytesRead = read(srcFile_, buffer, 1)) > 0);
    currentLine += LINE_SEP;
    fileStat = StorageDealLine(keys, num, currentLine);
    return true;
}

void BReportEntity::CheckAndUpdateIfReportLineEncoded(std::string &path)
{
    if (path.empty()) {
        return;
    }

    unordered_map<string, struct ReportFileInfo> infos = GetReportInfos();
    if (infos.size() == 1) {
        auto info = infos.begin();
        if (info->second.encodeFlag) {
            path = info->first;
        }
    } else {
        HILOGE("Invalid item sizes in report, current is %{public}zu", infos.size());
    }
}

string BReportEntity::EncodeReportItem(const string &reportItem, bool enableEncode)
{
    string encodeItem;
    if (enableEncode) {
        encodeItem = AppFileService::SandboxHelper::Encode(reportItem);
    } else {
        encodeItem = reportItem;
    }
    return encodeItem;
}

string BReportEntity::DecodeReportItem(const string &reportItem, bool enableEncode)
{
    string decodeItem;
    if (enableEncode) {
        decodeItem = AppFileService::SandboxHelper::Decode(reportItem);
    } else {
        decodeItem = reportItem;
    }
    return decodeItem;
}
} // namespace OHOS::FileManagement::Backup