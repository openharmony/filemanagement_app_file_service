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

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_FACTORY_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_FACTORY_H

/**
 * @file b_tarball.h
 * @brief 使用指定实现打包/解包，本层在进行打包/解包前还负责防御路径穿越攻击
 *
 */
#include <functional>
#include <memory>
#include <string_view>

#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
class BTarballFactory final : protected NoCopyable {
public:
    /**
     * @brief 打包器仿函数集合
     * 当前迫于时间所限，不得不采用命令行实现。后续必然要实现自研打包协议，从而满足在同一个进程进行打包的诉求。
     * 在降低调用成本后，就可以实现逐个向tarball追加文件的append方法。append方法是优化方法，和tar二选一即可。
     * 然而纯虚函数必须得在子类实现，这一性质使得基于继承的设计模式无法方便地选择合适的打包方法，为了解决这个问题，
     * 这里采用组合的方式实现。现在在外层简单地判断相应仿函数是否为空，就知道如何进行选择了。
     */
    struct Impl {
        /**
         * @brief 打包
         *
         * @param _1 进入该参数指定路径打包文件。
         * An absolute path is required.
         * @param _2 _1中需要打包的路径。
         * 要求输入相对路径，除禁止路径穿越外无其余要求，不填默认全部打包
         * @param _3 The part that does not need to be packed in the path to be packed.
         * 要求输入相对路径，路径穿越场景无实际意义因此予以禁止。可用于排除部分子目录
         */
        std::function<void(std::string_view, std::vector<std::string_view>, std::vector<std::string_view>)> tar;

        /**
         * @brief 解包
         *
         * @param _1 用于存储解包文件的根目录
         * An absolute path is required.
         */
        std::function<void(std::string_view)> untar;
    };

public:
    /**
     * @brief 打包器工厂方法
     *
     * @param implType 打包器实现方式，可选择'cmdline'
     * @param tarballPath Absolute path of the file package。Cannot contain extra slashes, must be suffixed with .tar
     * @return std::unique_ptr<Impl> 打包器仿函数集合
     */
    static std::unique_ptr<Impl> Create(std::string_view implType, std::string_view tarballPath);

public:
    BTarballFactory() = delete;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_FACTORY_H