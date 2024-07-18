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

#include <cstdio>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "iservice_registry.h"
#include "message_parcel_mock.h"
#include "module_ipc/service_reverse_proxy.h"
#include "service_reverse_mock.h"
#include "test_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string FILE_NAME = "1.tar";
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME_MANIFEST = "manifest.rp";
const string RESULT_REPORT = "result_report";
} // namespace

class ServiceReverseProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};

public:
    static inline shared_ptr<ServiceReverseProxy> proxy_ = nullptr;
    static inline sptr<ServiceReverseMock> mock_ = nullptr;
    static inline shared_ptr<MessageParcelMock> messageParcelMock_ = nullptr;
};

void ServiceReverseProxyTest::SetUpTestCase(void)
{
    mock_ = sptr(new ServiceReverseMock());
    proxy_ = make_shared<ServiceReverseProxy>(mock_);
    messageParcelMock_ = make_shared<MessageParcelMock>();
    MessageParcelMock::messageParcel = messageParcelMock_;
}
void ServiceReverseProxyTest::TearDownTestCase()
{
    mock_ = nullptr;
    proxy_ = nullptr;
    MessageParcelMock::messageParcel = nullptr;
    messageParcelMock_ = nullptr;
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnFileReady_0100
 * @tc.desc: Test function of BackupOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnFileReady_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0100");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, fd, 0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnFileReady_0101
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnFileReady_0101
 * @tc.desc: Test function of BackupOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnFileReady_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnFileReady_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnFileReady_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnFileReady_0102
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnFileReady_0102
 * @tc.desc: Test function of BackupOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnFileReady_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnFileReady_0102";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnFileReady_0102";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnFileReady_0103
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnFileReady_0103
 * @tc.desc: Test function of BackupOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnFileReady_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnFileReady_0103";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnFileReady_0103";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100
 * @tc.desc: Test function of BackupOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnBundleStarted_0101
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnBundleStarted_0101
 * @tc.desc: Test function of BackupOnBundleStarted interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnBundleStarted_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnBundleStarted_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnBundleStarted_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100
 * @tc.desc: Test function of BackupOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnBundleFinished_0101
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnBundleFinished_0101
 * @tc.desc: Test function of BackupOnBundleFinished interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnBundleFinished_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnBundleFinished_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnBundleFinished_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100
 * @tc.desc: Test function of BackupOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0101
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0101
 * @tc.desc: Test function of BackupOnAllBundlesFinished interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnResultReport_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnResultReport_0100
 * @tc.desc: Test function of BackupOnResultReport interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnResultReport_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnResultReport_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        std::string bundleName = "app01";
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->BackupOnResultReport(RESULT_REPORT, bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnResultReport_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnResultReport_0101
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnResultReport_0101
 * @tc.desc: Test function of BackupOnResultReport interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnResultReport_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnResultReport_0101";
    try {
        std::string bundleName = "app01";
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->BackupOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->BackupOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(false));
            proxy_->BackupOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->BackupOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnResultReport_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100
 * @tc.desc: Test function of RestoreOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0101
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0101
 * @tc.desc: Test function of RestoreOnBundleStarted interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100
 * @tc.desc: Test function of RestoreOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0101
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0101
 * @tc.desc: Test function of RestoreOnBundleFinished interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100
 * @tc.desc: Test function of RestoreOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0101
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0101
 * @tc.desc: Test function of RestoreOnAllBundlesFinished interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnFileReady_0100
 * @tc.desc: Test function of RestoreOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnFileReady_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0200");
        int32_t errCode = 0;
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        if (fd < 0) {
            errCode = BError::GetCodeByErrno(errno);
        }
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, fd, errCode);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnFileReady_0101
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnFileReady_0101
 * @tc.desc: Test function of RestoreOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnFileReady_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnFileReady_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(false));
            proxy_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnFileReady_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnFileReady_0102
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnFileReady_0102
 * @tc.desc: Test function of RestoreOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnFileReady_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnFileReady_0102";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(false));
            proxy_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnFileReady_0102";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100
 * @tc.desc: Test function of IncrementalBackupOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0300");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        TestManager tm1("ServiceReverseProxyTest_GetFd_0301");
        std::string manifestFilePath = tm1.GetRootDirCurTest().append(FILE_NAME_MANIFEST);
        UniqueFd manifestFd(open(manifestFilePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, fd, manifestFd, 0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0101
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0101
 * @tc.desc: Test function of IncrementalBackupOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0102
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0102
 * @tc.desc: Test function of IncrementalBackupOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0102,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0102";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0102";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0103
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0103
 * @tc.desc: Test function of IncrementalBackupOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0103,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0103";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->IncrementalBackupOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnFileReady_0103";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100
 * @tc.desc: Test function of IncrementalBackupOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0101
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0101
 * @tc.desc: Test function of IncrementalBackupOnBundleStarted interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->IncrementalBackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnBundleStarted_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100
 * @tc.desc: Test function of IncrementalBackupOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0101
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0101
 * @tc.desc: Test function of IncrementalBackupOnBundleFinished interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->IncrementalBackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnBundleFinished_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100
 * @tc.desc: Test function of IncrementalBackupOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->IncrementalBackupOnAllBundlesFinished(BError(BError::Codes::OK));
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0101
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0101
 * @tc.desc: Test function of IncrementalBackupOnAllBundlesFinished interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalBackupOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->IncrementalBackupOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->IncrementalBackupOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalBackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalBackupOnAllBundlesFinished_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100
 * @tc.desc: Test function of IncrementalRestoreOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0101
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0101
 * @tc.desc: Test function of IncrementalRestoreOnBundleStarted interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->IncrementalRestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleStarted_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100
 * @tc.desc: Test function of IncrementalRestoreOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0101
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0101
 * @tc.desc: Test function of IncrementalRestoreOnBundleFinished interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->IncrementalRestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnBundleFinished_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100
 * @tc.desc: Test function of IncrementalRestoreOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->IncrementalRestoreOnAllBundlesFinished(BError(BError::Codes::OK));
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0101
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0101
 * @tc.desc: Test function of IncrementalRestoreOnAllBundlesFinished interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalRestoreOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->IncrementalRestoreOnAllBundlesFinished(BError(BError::Codes::OK));
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO)
        << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnAllBundlesFinished_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100
 * @tc.desc: Test function of IncrementalRestoreOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I911LB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0400");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        TestManager tm1("ServiceReverseProxyTest_GetFd_0401");
        std::string manifestFilePath = tm1.GetRootDirCurTest().append(FILE_NAME_MANIFEST);
        UniqueFd manifestFd(open(manifestFilePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, fd, manifestFd, 0);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0101
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0101
 * @tc.desc: Test function of IncrementalRestoreOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0101,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0101";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0102
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0102
 * @tc.desc: Test function of IncrementalRestoreOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0102,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0102";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, -1, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0102";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0103
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0103
 * @tc.desc: Test function of IncrementalRestoreOnFileReady interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0103,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0103";
    try {
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteBool(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteFileDescriptor(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->IncrementalRestoreOnFileReady(BUNDLE_NAME, FILE_NAME, 0, 0, 0);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by IncrementalRestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnFileReady_0103";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnResultReport_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnResultReport_0100
 * @tc.desc: Test function of RestoreOnResultReport interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnResultReport_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnResultReport_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        std::string bundleName = "app01";
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->RestoreOnResultReport(RESULT_REPORT, bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnResultReport_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnResultReport_0101
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnResultReport_0101
 * @tc.desc: Test function of RestoreOnResultReport interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnResultReport_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnResultReport_0101";
    try {
        std::string bundleName = "app01";
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->RestoreOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->RestoreOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(false));
            proxy_->RestoreOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->RestoreOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnResultReport_0101";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0100
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0100
 * @tc.desc: Test function of IncrementalRestoreOnResultReport interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0100";
    try {
        EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        std::string bundleName = "app01";
        EXPECT_TRUE(proxy_ != nullptr);
        proxy_->IncrementalRestoreOnResultReport(RESULT_REPORT, bundleName);
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0101
 * @tc.name: SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0101
 * @tc.desc: Test function of IncrementalRestoreOnResultReport interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I9OVHB
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0101,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0101";
    try {
        std::string bundleName = "app01";
        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(false));
            EXPECT_TRUE(proxy_ != nullptr);
            proxy_->IncrementalRestoreOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(false));
            proxy_->IncrementalRestoreOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }

        try {
            EXPECT_CALL(*messageParcelMock_, WriteInterfaceToken(_)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
            EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
            EXPECT_CALL(*mock_, SendRequest(_, _, _, _)).WillOnce(Return(-1));
            proxy_->IncrementalRestoreOnResultReport(RESULT_REPORT, bundleName);
            EXPECT_TRUE(false);
        } catch (BError &err) {
            EXPECT_EQ(err.GetRawCode(), BError::Codes::SA_BROKEN_IPC);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnResultReport.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_IncrementalRestoreOnResultReport_0101";
}
} // namespace OHOS::FileManagement::Backup