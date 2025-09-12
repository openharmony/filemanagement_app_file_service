/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <cstdlib>

#include <dirent.h>
#include <fcntl.h>

#include <errors.h>
#include <file_ex.h>
#include <gtest/gtest.h>
#include <memory>

#include "hisysevent_mock.h"
#include "b_radar/radar_app_statistic.h"
#include "b_radar/radar_total_statistic.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace testing::ext;

#define HiSysEventWrite(domain, eventName, type, ...) HiSysEventMock::HiSysEventWriteMock(eventName)

class BRadarTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp()
    {
        appStatistic_ = std::make_shared<RadarAppStatistic>();
    };
    void TearDown() {};
protected:
    std::shared_ptr<RadarAppStatistic> appStatistic_ = nullptr;
    std::shared_ptr<RadarTotalStatistic> totalStatistic_ = nullptr;
};

HWTEST_F(BRadarTest, DURATION_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BRadarTest-begin DURATION_0100";
    try {
        Duration d1 = {5, 10};
        EXPECT_EQ(d1.GetSpan(), 5);
        Duration d2 = {0, 10};
        EXPECT_EQ(d2.GetSpan(), 0);
        Duration d3 = {10, 5};
        EXPECT_EQ(d3.GetSpan(), 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BRadarTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BRadarTest-end DURATION_0100";
}

HWTEST_F(BRadarTest, RadarErrorCode_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BRadarTest-begin RadarErrorCode_0100";
    try {
        RadarError err(BError{BError::Codes::EXT_INVAL_ARG});
        EXPECT_EQ(err.error_, 13920);
        EXPECT_EQ(err.GenCode(), static_cast<int32_t>(((SUB_SYSTEM_ID & MASK_SYS) << MOVE_BIT_SYS)
            | ((MODULE_UNKNOWN & MASK_MODULE) << MOVE_BIT_MODULE) | (13920 & MASK_ERROR)));
        err.UpdateByBError(BError(139000020));
        EXPECT_EQ(err.error_, 13942);
        RadarError err2(MODULE_ABILITY_MGR_SVC, BError(BError::Codes::OK));
        EXPECT_EQ(err2.GenCode(), 0);
        EXPECT_EQ(err2.moduleId_, MODULE_ABILITY_MGR_SVC);
        RadarError err3(MODULE_INIT, 139000041);
        EXPECT_EQ(err3.error_, 13941);
        RadarError err4(MODULE_INIT, 1390000043);
        EXPECT_EQ(err4.error_, 13943);
        EXPECT_EQ(0, err4.TransferErrCode(0));
        EXPECT_EQ(1, err4.TransferErrCode(-1));
        EXPECT_EQ(30099, err4.TransferErrCode(300000099));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BRadarTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BRadarTest-end RadarErrorCode_0100";
}

HWTEST_F(BRadarTest, TOTAL_CONSTRUCTOR_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BRadarTest-begin TOTAL_CONSTRUCTOR_0100";
    try {
        std::string caller = "ut_caller";
        RadarTotalStatistic totalStatistic1(BizScene::BACKUP, caller, Mode::INCREMENTAL);
        EXPECT_EQ(totalStatistic1.bizScene_, BizScene::BACKUP);
        EXPECT_EQ(totalStatistic1.mode_, Mode::INCREMENTAL);
        EXPECT_EQ(totalStatistic1.hostPkg_, caller);
        EXPECT_GT(totalStatistic1.uniqId_, 0);
        EXPECT_EQ(totalStatistic1.uniqId_, totalStatistic1.GetUniqId());
        RadarTotalStatistic totalStatistic2(BizScene::RESTORE, caller);
        EXPECT_EQ(totalStatistic2.bizScene_, BizScene::RESTORE);
        EXPECT_EQ(totalStatistic2.mode_, Mode::FULL);
        EXPECT_GT(totalStatistic2.uniqId_, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BRadarTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BRadarTest-end TOTAL_CONSTRUCTOR_0100";
}

HWTEST_F(BRadarTest, TOTAL_REPORT_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BRadarTest-begin TOTAL_REPORT_0100";
    try {
        std::string caller = "ut_caller";
        totalStatistic_ = std::make_shared<RadarTotalStatistic>(BizScene::BACKUP, caller);
        totalStatistic_->Report("TOTAL_REPORT_0100", 0);
        totalStatistic_->Report("TOTAL_REPORT_0100", MODULE_INIT, 10);
        totalStatistic_->Report("TOTAL_REPORT_0100", BError(), MODULE_INIT);
        EXPECT_TRUE(true);
        totalStatistic_ = std::make_shared<RadarTotalStatistic>(BizScene::RESTORE, caller);
        totalStatistic_->Report("TOTAL_REPORT_0100", 0);
        totalStatistic_->Report("TOTAL_REPORT_0100", MODULE_BMS, 20);
        totalStatistic_->Report("TOTAL_REPORT_0100", BError(BError::Codes::TOOL_INVAL_ARG), MODULE_INIT);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BRadarTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BRadarTest-end TOTAL_REPORT_0100";
}

HWTEST_F(BRadarTest, FileTypeStat001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BRadarTest-begin FileTypeStat001";
    try {
        FileTypeStat fileTypeStat;
        EXPECT_EQ(fileTypeStat.GetListSize(), TYPE_DEF_COUNT);
        fileTypeStat.UpdateStat("txt", 1024);
        EXPECT_EQ(fileTypeStat.GetListPtr()[0].count, 1);
        EXPECT_EQ(fileTypeStat.GetListPtr()[0].size, 1024);
        fileTypeStat.UpdateStat("jpg", 1024);
        EXPECT_EQ(fileTypeStat.GetListPtr()[1].count, 1);
        EXPECT_EQ(fileTypeStat.GetListPtr()[1].size, 1024);
        fileTypeStat.UpdateStat("wav", 1024);
        EXPECT_EQ(fileTypeStat.GetListPtr()[2].count, 1);
        EXPECT_EQ(fileTypeStat.GetListPtr()[2].size, 1024);
        fileTypeStat.UpdateStat("mov", 1024);
        EXPECT_EQ(fileTypeStat.GetListPtr()[3].count, 1);
        EXPECT_EQ(fileTypeStat.GetListPtr()[3].size, 1024);
        fileTypeStat.UpdateStat("rar", 1024);
        EXPECT_EQ(fileTypeStat.GetListPtr()[4].count, 1);
        EXPECT_EQ(fileTypeStat.GetListPtr()[4].size, 1024);
        fileTypeStat.UpdateStat("exe", 1024);
        EXPECT_EQ(fileTypeStat.GetListPtr()[5].count, 1);
        EXPECT_EQ(fileTypeStat.GetListPtr()[5].size, 1024);
        fileTypeStat.UpdateStat("com", 1024);
        EXPECT_EQ(fileTypeStat.GetListPtr()[6].count, 1);
        EXPECT_EQ(fileTypeStat.GetListPtr()[6].size, 1024);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BRadarTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BRadarTest-end FileTypeStat001";
}

HWTEST_F(BRadarTest, FileSizeStat001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BRadarTest-begin FileSizeStat001";
    try {
        FileSizeStat fileSizeStat;
        EXPECT_EQ(fileSizeStat.GetListSize(), SIZE_DEF_COUNT);
        fileSizeStat.UpdateStat(1024);
        EXPECT_EQ(fileSizeStat.GetListPtr()[0].count, 1);
        EXPECT_EQ(fileSizeStat.GetListPtr()[0].size, 1024);
        fileSizeStat.UpdateStat(ONE_MB);
        EXPECT_EQ(fileSizeStat.GetListPtr()[1].count, 1);
        EXPECT_EQ(fileSizeStat.GetListPtr()[1].size, ONE_MB);
        fileSizeStat.UpdateStat(ONE_MB);
        EXPECT_EQ(fileSizeStat.GetListPtr()[1].count, 2);
        EXPECT_EQ(fileSizeStat.GetListPtr()[1].size, TWO_MB);
        fileSizeStat.UpdateStat(TWO_MB);
        EXPECT_EQ(fileSizeStat.GetListPtr()[2].count, 1);
        EXPECT_EQ(fileSizeStat.GetListPtr()[2].size, TWO_MB);
        fileSizeStat.UpdateStat(TEN_MB);
        EXPECT_EQ(fileSizeStat.GetListPtr()[3].count, 1);
        EXPECT_EQ(fileSizeStat.GetListPtr()[3].size, TEN_MB);
        fileSizeStat.UpdateStat(HUNDRED_MB);
        EXPECT_EQ(fileSizeStat.GetListPtr()[4].count, 1);
        EXPECT_EQ(fileSizeStat.GetListPtr()[4].size, HUNDRED_MB);
        fileSizeStat.UpdateStat(ONE_GB);
        EXPECT_EQ(fileSizeStat.GetListPtr()[5].count, 1);
        EXPECT_EQ(fileSizeStat.GetListPtr()[5].size, ONE_GB);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BRadarTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BRadarTest-end FileSizeStat001";
}

HWTEST_F(BRadarTest, RADAR_APP_STAT_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BRadarTest-begin RADAR_APP_STAT_0100";
    try {
        appStatistic_->UpdateFileDist("txt", 1024);
        EXPECT_EQ(appStatistic_->fileSizeDist_.GetListPtr()[0].count, 1);
        EXPECT_EQ(appStatistic_->fileSizeDist_.GetListPtr()[0].size, 1024);
        EXPECT_EQ(appStatistic_->fileTypeDist_.GetListPtr()[0].count, 1);
        EXPECT_EQ(appStatistic_->fileTypeDist_.GetListPtr()[0].size, 1024);

        appStatistic_->SetUniqId(100);
        EXPECT_EQ(appStatistic_->uniqId_, 100);
        appStatistic_->sendRateZeroStart_ = 5;
        appStatistic_->UpdateSendRateZeroSpend();
        EXPECT_EQ(appStatistic_->sendRateZeroStart_, 0);

        appStatistic_->ReportBackup("RADAR_APP_STAT_0100", BError(BError::Codes::OK));
        appStatistic_->ReportRestore("RADAR_APP_STAT_0100", BError(BError::Codes::OK));
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BRadarTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BRadarTest-end RADAR_APP_STAT_0100";
}
} // namespace OHOS::FileManagement::Backup