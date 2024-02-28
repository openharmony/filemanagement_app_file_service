/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

import {describe, beforeAll, afterAll, it, expect} from 'deccjsunit/index'
import fileuri from '@ohos.file.fileuri'
import deviceInfo from '@ohos.deviceInfo'

describe('FileShareJSTest', function () {
    beforeAll(async function () {
        console.info('beforeAll');
    })

    afterAll(async function () {
        console.info('afterAll');
    })

    /**
     * @tc.name:      is_remote_uri_test_001
     * @tc.desc:      it is a remote URI test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I9010V
     */
    it('is_remote_uri_test_001', 0, async function () {
        console.info(`is_remote_uri_test_001 start`);
        try {
            let uri = "file://com.demo.a/data/storage/el2/distributedfiles/.remote_share/data/haps/entry/files/1.txt";
            uri += "?networkid=64799ecdf70788e396f454ff4a6e6ae4b09e20227c39c21f6e67a2aacbcef7b9";
            let fileUriObject = new fileuri.FileUri(uri);
            let ret = fileUriObject.isRemoteUri();
            expect(ret).assertTrue();
        } catch (error) {
            console.error(`IsRemoteUri Judge failed because: ${JSON.stringify(error)}`);
            expect(false).assertTrue();
        }
        console.info(`is_remote_uri_test_001 end`);
    })

    /**
     * @tc.name:      is_remote_uri_test_002
     * @tc.desc:      it is not a remote URI test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I9010V
     */
    it('is_remote_uri_test_002', 0, async function () {
        console.info(`is_remote_uri_test_002 start`);
        try {
            let uri = "file://com.demo.a/data/storage/el2/distributedfiles/.remote_share/data/haps/entry/files/2.txt";
            let fileUriObject = new fileuri.FileUri(uri);
            let ret = fileUriObject.isRemoteUri();
            console.log(`IsRemoteUri Judge result: ${JSON.stringify(ret)}`);
            expect(ret == false).assertTrue();
        } catch (error) {
            console.error(`IsRemoteUri Judge failed because: ${JSON.stringify(error)}`);
            expect(false).assertTrue();
        }
        console.info(`is_remote_uri_test_002 end`);
    })

    /**
     * @tc.name:      get_path_from_uri_test_001
     * @tc.desc:      get path from uri with same app test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I9010V
     */
    it('get_path_from_uri_test_001', 0, async function () {
        console.info(`get_path_from_uri_test_001 start`);
        try {
            let uri = "file://ohos.file.fileuri.test/data/storage/el2/base/files/getpathtest001.txt";
            let resultPath = "/data/storage/el2/base/files/getpathtest001.txt";
            let fileUriObject = new fileuri.FileUri(uri);
            let realPath = fileUriObject.path;
            console.info(`getPathFromUri success: ${JSON.stringify(realPath)}`);
            expect(resultPath == realPath).assertTrue();
        } catch (error) {
            console.error(`getPathFromUri failed:${JSON.stringify(error)}`);
            expect(false).assertTrue();
        }
        console.info(`get_path_from_uri_test_001 end`);
    })

    /**
     * @tc.name:      get_path_from_uri_test_002
     * @tc.desc:      get path from uri with different app test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I9010V
     */
    it('get_path_from_uri_test_002', 0, async function () {
        console.info(`get_path_from_uri_test_002 start`);
        try {
            let uri = "file://com.example.filesharea/data/storage/el2/base/files/getpathtest002.txt";
            let resultPath = "/data/storage/el2/share/r/com.example.filesharea/data/storage/el2/base/files/getpathtest002.txt";
            let fileUriObject = new fileuri.FileUri(uri);
            let realPath = fileUriObject.path;
            console.info(`getPathFromUri success: ${JSON.stringify(realPath)}`);
            expect(resultPath == realPath).assertTrue();
        } catch (error) {
            console.error(`getPathFromUri failed:${JSON.stringify(error)}`);
            expect(false).assertTrue();
        }
        console.info(`get_path_from_uri_test_002 end`);
    })

    /**
     * @tc.name:      get_path_from_uri_test_003
     * @tc.desc:      get path from uri with different app test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I9010V
     */
    it('get_path_from_uri_test_003', 0, async function () {
        console.info(`get_path_from_uri_test_003 start`);
        try {
            let uri = "file://docs/storage/Users/currentUser/Documents/getpathtest003.txt";
            if (deviceInfo.deviceType != '2in1') {
                let resultPath = "/data/storage/el2/share/r/docs/storage/Users/currentUser/Documents/getpathtest003.txt";
                let fileUriObject = new fileuri.FileUri(uri);
                let realPath = fileUriObject.path;
                console.info(`getPathFromUri success: ${JSON.stringify(realPath)}`);
                expect(resultPath == realPath).assertTrue();
            } else {
                let resultPath = "/storage/Users/currentUser/Documents/getpathtest003.txt";
                let fileUriObject = new fileuri.FileUri(uri);
                let realPath = fileUriObject.path;
                console.info(`getPathFromUri success: ${JSON.stringify(realPath)}`);
                expect(resultPath == realPath).assertTrue();
            }
        } catch (error) {
            console.error(`getPathFromUri failed:${JSON.stringify(error)}`);
            expect(false).assertTrue();
        }
        console.info(`get_path_from_uri_test_003 end`);
    })
});