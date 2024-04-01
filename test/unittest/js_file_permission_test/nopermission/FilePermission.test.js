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
import fileShare from '@ohos.fileshare'
import fileuri from '@ohos.file.fileuri'
import fs from '@ohos.file.fs'

const DEVICE_TYPE_ERR = 801;
const PERMISSION_ERR = 201;

describe('FileShareJSTest', function () {
    beforeAll(async function () {
        console.info('beforeAll');
    })

    afterAll(async function () {
        console.info('afterAll');
    })

    /**
     * @tc.name:      persist_permission_test
     * @tc.desc:      Test persistPermission without system capability
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('persist_permission_test', 0, async function (done) {
        console.info(`persist_permission_test start`);
        let filePath = "/data/storage/el2/base/persist_permission_test.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.READ_MODE };
        let policies = [policyInfo];
        try {
            fileShare.persistPermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`persist_permission_test err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`persist_permission_test error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PERMISSION_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`persist_permission_test end`);
    })

    /**
     * @tc.name:      revoke_permission_test
     * @tc.desc:      Test revokePermission without system capability
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('revoke_permission_test', 0, async function (done) {
        console.info(`revoke_permission_test start`);
        let filePath = "/data/storage/el2/base/revoke_permission_test.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.READ_MODE };
        let policies = [policyInfo];
        try {
            fileShare.revokePermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`revoke_permission_test err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`revoke_permission_test error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PERMISSION_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`revoke_permission_test end`);
    })

    /**
     * @tc.name:      activate_permission_test
     * @tc.desc:      Test activatePermission without system capability
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('activate_permission_test', 0, async function (done) {
        console.info(`activate_permission_test start`);
        let filePath = "/data/storage/el2/base/activate_permission_test.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.READ_MODE };
        let policies = [policyInfo];
        try {
            fileShare.activatePermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`activate_permission_test err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`activate_permission_test error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PERMISSION_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`activate_permission_test end`);
    })

    /**
     * @tc.name:      deactivate_permission_test
     * @tc.desc:      Test deactivatePermission without system capability
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('deactivate_permission_test', 0, async function (done) {
        console.info(`deactivate_permission_test start`);
        let filePath = "/data/storage/el2/base/deactivate_permission_test.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.READ_MODE };
        let policies = [policyInfo];
        try {
            fileShare.deactivatePermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`deactivate_permission_test err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`deactivate_permission_test error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PERMISSION_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`deactivate_permission_test end`);
    })

    /**
     * @tc.name:      check_persistent_permission_test
     * @tc.desc:      Test checkPersistentPermission without system capability
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('check_persistent_permission_test', 0, async function (done) {
        console.info(`check_persistent_permission_test start`);
        let filePath = "/data/storage/el2/base/check_persistent_permission_test.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.READ_MODE };
        let policies = [policyInfo];
        try {
            fileShare.checkPersistentPermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`check_persistent_permission_test err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`check_persistent_permission_test error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PERMISSION_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`check_persistent_permission_test end`);
    })
});