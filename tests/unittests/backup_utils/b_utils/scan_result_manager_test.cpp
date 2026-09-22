/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "b_resources/b_constants.h"
#include "b_utils/scan_result_manager.h"

namespace OHOS::FileManagement::Backup {
class ScanResultManagerTest : public testing::Test {};

HWTEST_F(ScanResultManagerTest, FileInfoTypes_001, testing::ext::TestSize.Level1)
{
    struct stat sta = {};
    FileInfo file("file", "/data/file", sta, true);
    CompatibleFileInfo compatible("file", "/data/file", sta, true, "/restore/file");
    AncoFileInfo anco("anco", "/data/anco", sta, true);
    AncoCompatibleFileInfo compatibleAnco("anco", "/data/anco", sta, true, "/restore/anco");
    SmallFileInfo small("/data/small", 1);
    CompatibleSmallFileInfo compatibleSmall("/data/small", 1, "/restore/small");
    SpecialFileInfo special("special", "/data/special", sta, true, UniqueFd(BConstants::INVALID_FD_NUM));

    EXPECT_TRUE(file.GetRestorePath().empty());
    EXPECT_EQ(compatible.GetRestorePath(), "/restore/file");
    EXPECT_TRUE(anco.GetRestorePath().empty());
    EXPECT_EQ(compatibleAnco.GetRestorePath(), "/restore/anco");
    EXPECT_TRUE(small.GetRestorePath().empty());
    EXPECT_EQ(compatibleSmall.GetRestorePath(), "/restore/small");
    EXPECT_TRUE(special.GetRestorePath().empty());
    EXPECT_EQ(special.GetFd(), BConstants::INVALID_FD_NUM);
}

HWTEST_F(ScanResultManagerTest, AddBigFile_001, testing::ext::TestSize.Level1)
{
    ScanResultManager manager;
    struct stat sta = {};
    manager.SetCallerBundleName(BConstants::BUNDLE_DATA_CLONE);
    manager.AddBigFile("/data/photo.txt", sta, true);
    manager.AddBigFile("/data/photo.txt", sta, false, "/restore/photo.txt");

    auto first = manager.GetFileInfo();
    auto second = manager.GetFileInfo();
    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    EXPECT_NE(first->filename_, second->filename_);
    EXPECT_TRUE(first->isLongPath_);
    EXPECT_TRUE(first->GetRestorePath().empty());
    EXPECT_FALSE(second->isLongPath_);
    EXPECT_EQ(second->GetRestorePath(), "/restore/photo.txt");
    EXPECT_TRUE(manager.GetFileInfo() == nullptr);

    manager.SetCallerBundleName("normal.bundle");
    manager.AddBigFile("/data/no_extension", sta, false);
    EXPECT_NE(manager.GetFileInfo(), nullptr);

    manager.SetCallerBundleName(BConstants::BUNDLE_DATA_CLONE);
    manager.AddBigFile("/data/no_extension_clone", sta, false);
    manager.AddBigFile("/data/already_anco.txt", sta, false);
    EXPECT_NE(manager.GetFileInfo(), nullptr);
    EXPECT_NE(manager.GetFileInfo(), nullptr);
}

HWTEST_F(ScanResultManagerTest, AddAncoFile_001, testing::ext::TestSize.Level1)
{
    ScanResultManager manager;
    struct stat sta = {};
    manager.SetCallerBundleName(BConstants::BUNDLE_DATA_CLONE);
    manager.AddAncoBigFile("/data/photo.jpg", "", sta);
    manager.AddAncoBigFile("/data/photo.jpg", "/restore/photo.jpg", sta);

    auto first = manager.GetFileInfo();
    auto second = manager.GetFileInfo();
    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    EXPECT_TRUE(first->isAncoFile_);
    EXPECT_TRUE(first->GetRestorePath().empty());
    EXPECT_EQ(second->GetRestorePath(), "/restore/photo.jpg");
    EXPECT_NE(first->filename_, second->filename_);

    manager.SetCallerBundleName("normal.bundle");
    manager.AddAncoBigFile("/data/no_extension", "", sta);
    EXPECT_NE(manager.GetFileInfo(), nullptr);

    manager.SetCallerBundleName(BConstants::BUNDLE_DATA_CLONE);
    manager.AddAncoBigFile("/data/no_extension_clone", "", sta);
    EXPECT_NE(manager.GetFileInfo(), nullptr);
}

HWTEST_F(ScanResultManagerTest, TarFilePacketControl_001, testing::ext::TestSize.Level1)
{
    ScanResultManager manager;
    manager.maxTarSize_.store(5);
    struct stat invalidSta = {};
    invalidSta.st_size = -1;
    manager.AddTarFile("invalid", "/data/invalid", invalidSta);
    EXPECT_FALSE(manager.HasFileReady());

    struct stat sta = {};
    sta.st_size = 6;
    manager.AddTarFile("tar", "/data/tar", sta);
    EXPECT_TRUE(manager.HasFileReady());
    EXPECT_TRUE(manager.stopPacket_.load());
    auto file = manager.GetFileInfo();
    ASSERT_NE(file, nullptr);
    EXPECT_FALSE(file->isBigFile_);
    EXPECT_FALSE(manager.stopPacket_.load());
    EXPECT_EQ(manager.currentTarSize_.load(), 0U);

    sta.st_size = 4;
    manager.AddAncoTarFile("anco", "/data/anco", sta);
    EXPECT_FALSE(manager.stopPacket_.load());
    auto anco = manager.GetFileInfo();
    ASSERT_NE(anco, nullptr);
    EXPECT_TRUE(anco->isAncoFile_);

    manager.pendingFileQueue_.push(nullptr);
    EXPECT_EQ(manager.GetFileInfo(), nullptr);
    EXPECT_EQ(manager.GetMaxTarSize(), 5U);

    ScanResultManager fullManager;
    fullManager.maxTarSize_.store(5);
    sta.st_size = 6;
    fullManager.AddTarFile("first", "/data/first", sta);
    fullManager.AddTarFile("second", "/data/second", sta);
    EXPECT_NE(fullManager.GetFileInfo(), nullptr);
    EXPECT_TRUE(fullManager.stopPacket_.load());
}

HWTEST_F(ScanResultManagerTest, CollectionsAndWaitFlags_001, testing::ext::TestSize.Level1)
{
    ScanResultManager manager;
    EXPECT_TRUE(manager.GetAllFiles().empty());
    EXPECT_TRUE(manager.GetAllSmallFiles().empty());

    manager.AddSmallFile("/data/a", 1);
    manager.AddSmallFile("/data/b", 2, "/restore/b");
    auto smallFiles = manager.GetAllSmallFiles();
    ASSERT_EQ(smallFiles.size(), 2U);
    EXPECT_TRUE(smallFiles[0]->GetRestorePath().empty());
    EXPECT_EQ(smallFiles[1]->GetRestorePath(), "/restore/b");
    EXPECT_TRUE(manager.GetAllSmallFiles().empty());

    struct stat sta = {};
    std::shared_ptr<IFileInfo> file = std::make_shared<FileInfo>("file", "/data/file", sta, true);
    manager.AddAllFile(file);
    auto allFiles = manager.GetAllFiles();
    ASSERT_EQ(allFiles.size(), 1U);
    EXPECT_TRUE(manager.GetAllFiles().empty());

    manager.SetCompletedFlag(false);
    EXPECT_FALSE(manager.IsProcessCompleted());
    manager.AddBigFile("/data/ready", sta, false);
    manager.WaitForFiles();
    manager.WaitForPacketFlag();
    manager.SetCompletedFlag(true);
    manager.WaitForCompleted();
    EXPECT_TRUE(manager.IsProcessCompleted());
    EXPECT_NE(manager.GetFileInfo(), nullptr);
    manager.WaitForFiles();
    manager.WaitForPacketFlag();
}
} // namespace OHOS::FileManagement::Backup
