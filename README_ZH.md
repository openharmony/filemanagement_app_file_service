# 应用文件服务

## **简介**
应用文件服务是为应用提供文件分享和管理能力的服务，包含应用间文件分享、跨设备同应用文件分享以及跨设备跨应用文件分享的能力。
当前已具备基于分布式文件系统的跨设备同应用文件分享能力。

## **目录**
```
/foundation/filemanagement/app_file_service
│── interfaces                        // 接口声明
│  ├── innerkits                      // 对内接口声明
│  └── kits                           // 对外接口声明
```

## **说明**
### 接口说明
**表1** 应用文件服务接口说明
| **接口名**                                                                                                                                       | **说明**                                                  |
| ------------------------------------------------------------------------------------------------------------------------------------------------ | --------------------------------------------------------- |
| createSharePath(fd: number, cid: string, callback: AsyncCallback\<string>): void <br> createSharePath(fd: number, cid: string): Promise\<string> | 将文件fd与设备cid传递给分布式文件系统，创建跨设备分享路径 |
### 使用说明
createSharePath接口为分享文件fd创建能够跨设备访问的分布式路径，异步返回创建结果，设备号cid用于指定分享设备。

示例：
```
import remotefileshare from '@ohos.remotefileshare'

remotefileshare.createSharePath(fd, cid, function(err, path) {
    // callback
});

remotefileshare.createSharePath(fd, cid).then(function(path) {
    // promise
});
```

# 备份恢复

## **简介**

备份恢复是为Openharmony设备上三方应用数据、系统应用数据、公共数据提供一套完整的数据备份和数据恢复解决方案。

备份恢复功能主要由三大部分组成：
 - 集成在克隆等系统应用中的[JS API](https://gitee.com/openharmony/docs/blob/weekly_20230502/zh-cn/application-dev/reference/apis/js-apis-file-backup.md)：负责触发备份/恢复数据。支持获取能力文件，触发备份应用数据，触发恢复应用数据，设置恢复应用数据时安装应用。
 - 集成在待备份恢复应用中的备份[服务扩展](https://gitee.com/openharmony/ability_ability_runtime#简介)：负责备份恢复具体应用的数据。应用开发者可通过配置备份恢复策略规则，配置备份恢复场景及过滤隐私等目录。
 - 具有独立进程的备份服务：主要负责调度备份恢复任务。具体而言，其具体职责包括获取及检查备份恢复能力、管理备份服务扩展的生命周期与并发程度、协调零拷贝传输文件、在恢复时可选择安装应用。

## **目录**
```
/foundation/filemanagement/app_file_service
│── frameworks                        // 框架层
|  └── native
│     └── backup_ext                  // 备份服务扩展
│── interfaces                        // 接口存放目录
|  ├── api
│  ├── inner_api                      // 内部接口声明
│  └── kits
|     └── js
|        └── backup                   // js外部接口
│── services
|   └── backup_sa                     // 备份恢复服务
│── tests                             // 测试用例
│── tools                             // 备份恢复工具
└── utils                             // 工具套
```

# **相关仓**

- [Linux内核5.10](https://gitee.com/openharmony/kernel_linux_5.10)
- [文件访问接口](https://gitee.com/openharmony/filemanagement_file_api)
- [元能力子系统](https://gitee.com/openharmony/ability_ability_runtime)
- [系统服务框架](https://gitee.com/openharmony/systemabilitymgr_safwk)