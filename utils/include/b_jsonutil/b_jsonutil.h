/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_JSON_UTIL_H
#define OHOS_FILEMGMT_BACKUP_JSON_UTIL_H

#include <string>
#include <map>

namespace OHOS::FileManagement::Backup {
class BJsonUtil {
public:
    typedef struct BundleDetailInfo {
        std::string backupScene;
        std::string broadCastType;
        std::string bundleName;
        std::string type;
        std::string detail;
        int bundleIndex;
        int32_t userId;
    }BundleDetailInfo;

    typedef struct BundleDataSize {
        std::string bundleName;
        int64_t dataSize = -1;
        int64_t incDataSize = -1;
    }BundleDataSize;

    /**
     * @brief 带有拼接字符的bundleName按照拼接字符进行分割
     *
     * @param bundleNameStr bundleName拼接index的字符串
     *
     * @return 分割好的结果赋值给结构体
     */
    static BundleDetailInfo ParseBundleNameIndexStr (const std::string &bundleNameStr);

    /**
     * @brief 将传进来的bundleNames的集合进行按照拼接字符分割处理
     *
     * @param bundleNames bundleName拼接index的字符串集合
     * @param details infos的集合
     * @param patternInfo 拼接的字符
     * @param realBundleNames 分割后真正的bundleNames
     * @param userId userId
     * @param isClearDataFlags 框架是否清理标志集合
     *
     * @return 包名和解析结果的对应关系集合
     *
     */
    static std::map<std::string, std::vector<BundleDetailInfo>> BuildBundleInfos(
        const std::vector<std::string> &bundleNames, const std::vector<std::string> &details,
        std::vector<std::string> &realBundleNames, int32_t userId,
        std::map<std::string, bool> &isClearDataFlags);

    /**
     * @brief 解析单个bundle对应的json串
     *
     * @param bundleInfo json串
     * @param bundleDetails 结构体对象
     * @param bundleDetailInfo bundle信息
     * @param isClearData 框架是否清理标志
     * @param userId userId
     *
     */
    static void ParseBundleInfoJson(const std::string &bundleInfo, std::vector<BundleDetailInfo> &bundleDetails,
        BJsonUtil::BundleDetailInfo bundleDetailInfo, bool &isClearData, int32_t userId);

    /**
     * @brief 根据业务类型和bundleName确定唯一的bundleInfo
     *
     * @param bundleNameDetailsMap 包名和当前包扩展信息解析结果的集合
     * @param bundleName 包名
     * @param jobType 业务类型broadcast或者unicast
     * @param bundleDetail 确定下来的bundleInfo
     *
     * @return 是否获取到
     *
     */
    static bool FindBundleInfoByName(std::map<std::string, std::vector<BundleDetailInfo>> &bundleNameDetailsMap,
        std::string &bundleName, const std::string &jobType, BundleDetailInfo &bundleDetail);

    /**
     * @brief 组建恢复错误信息的json
     *
     * @param jsonStr 组建结果
     * @param errCode 错误码
     * @param errMsg 错误信息
     *
     * @return 是否组建成功
     *
     */
    static bool BuildExtensionErrInfo(std::string &jsonStr, int errCode, std::string errMsg);

    /**
     * @brief 拼接包名和分身对应的索引
     *
     * @param bundleName 包名
     * @param bundleIndex 索引
     *
     * @return 拼接结果
     */
    static std::string BuildBundleNameIndexInfo(const std::string &bundleName, int bundleIndex);

    /**
     * @brief 组建恢复文件错误信息的json
     *
     * @param jsonStr 组建结果
     * @param errCode 错误码
     *
     * @return 是否组建成功
     *
     */
    static bool BuildExtensionErrInfo(std::string &jsonStr, std::map<std::string, std::vector<int>> errFileInfo);

    /**
     * @brief 组建App进度返回的信息
     *
     * @param jsonStr 组建结果
     * @param onProcessRet onProcess接口返回值
     *
     * @return 是否组建成功
     *
     */
    static bool BuildOnProcessRetInfo(std::string &jsonStr, std::string onProcessRet);

    /**
     * @brief 组建App进度返回的信息
     *
     * @param reportInfo 组建结果
     * @param path 报错文件
     * @param err 错误码
     *
     * @return 是否组建成功
     *
     */
    static bool BuildOnProcessErrInfo(std::string &reportInfo, std::string path, int err);

    /**
     * @brief 构建包含userId的detailInfo
     *
     * @param userId userId
     * @param detailInfo 包含userId的detailInfo
     *
     * @return 是否组建成功
     *
     */
    static bool BuildBundleInfoJson(int32_t userId, std::string &detailInfo);

    /**
     * @brief 判断传入的bundleinfo中是否包含unicast字段
     *
     * @param bundleinfo json串
     *
     * @return 是否包含unicast字段
     *
     */
    static bool HasUnicastInfo(std::string &bundleInfo);

    /**
     * @brief 解析备份框架版本号字段
     *
     * @return 备份恢复框架版本号
     *
     */
    static std::string ParseBackupVersion();

    /**
     * @brief 拼接session冲突时报错信息
     *
     * @param userId 用户id
     * @param callerName session创建方
     * @param activeTime session创建时间
     *
     * @return 拼接结果
     */
    static std::string BuildInitSessionErrInfo(int32_t userId, std::string callerName, std::string activeTime);

    /**
     * @brief 将已经扫描的结果转换成json串
     *
     * @param bundleDataList 存储扫描结果的结构体列表
     * @param listSize 当前需要返回的数量
     * @param scanning 当前正在扫描的包名
     * @param jsonStr 需要返回的结果
     *
     * @return 是否成功
     */
    static bool WriteToStr(std::vector<BundleDataSize> &bundleDataList,
                           size_t listSize,
                           std::string scanning,
                           std::string &jsonStr);

    /**
     * @brief 通过bundlename找sceneId
     *
     * @param bundleNameDetailsMap 包名和当前包扩展信息解析结果的集合
     * @param bundleName 包名
     * @param backupScene 存储backupScene的变量
     *
     * @return 是否成功
     */
    static bool FindBackupSceneByName(std::map<std::string, std::vector<BundleDetailInfo>> &bundleNameDetailsMap,
        std::string &bundleName, std::string &backupScene);
    
    /**
     * @brief 通过bundlename找BroadCastInfo
     *
     * @param bundleNameDetailsMap 包名和当前包扩展信息解析结果的集合
     * @param bundleName 包名
     * @param jobType 业务类型broadcast或者unicast
     * @param broadCastInfoMap 存储broadCastInfoMap的变量
     *
     * @return 是否成功
     */
    static bool FindBroadCastInfoByName(std::map<std::string, std::vector<BundleDetailInfo>> &bundleNameDetailsMap,
        std::string &bundleName, const std::string &jobType, std::map<std::string, std::string> &broadCastInfoMap);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_JSON_UTIL_H