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
const PARAMETER_TYPE_ERR = 401;
const OPERATION_NOT_PERMITTED = 13900001;

describe('FileShareJSTest', function () {
    beforeAll(async function () {
        console.info('beforeAll');
    })

    afterAll(async function () {
        console.info('afterAll');
    })

    /**
     * @tc.name:      persist_permission_test_001
     * @tc.desc:      normal uri and operationMode is READ_MODE test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('persist_permission_test_001', 0, async function (done) {
        console.info(`persist_permission_test_001 start`);
        let filePath = "/data/storage/el2/base/persist_permission_test_001.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.READ_MODE };
        let policies = [policyInfo];
        try {
            fileShare.persistPermission(policies).then(() => {
                expect(true).assertTrue();
                done();
            }).catch((err) => {
                console.info(`persist_permission_test_001 err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`persist_permission_test_001 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`persist_permission_test_001 end`);
    })

    /**
     * @tc.name:      persist_permission_test_002
     * @tc.desc:      normal uris and have a uri is not exist test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('persist_permission_test_002', 0, async function (done) {
        console.info(`persist_permission_test_002 start`);
        let filePath = "/data/storage/el2/base/persist_permission_test_002.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfoA = { uri: uri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let fileUri = "/data/storage/el2/persist_permission_test_002.txt";
        let policyInfoB = { uri: fileUri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policies = [policyInfoA, policyInfoB];
        try {
            fileShare.persistPermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`persist_permission_test_002 err : ${err.code}`);
                if (err.code == OPERATION_NOT_PERMITTED) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                done();
            })
        } catch (error) {
            console.info(`persist_permission_test_002 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`persist_permission_test_002 end`);
    })

    /**
     * @tc.name:      persist_permission_test_003
     * @tc.desc:      persist_permission with no param test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('persist_permission_test_003', 0, async function (done) {
        console.info(`persist_permission_test_003 start`);
        try {
            fileShare.persistPermission().then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`persist_permission_test_003 err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`persist_permission_test_003 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PARAMETER_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`persist_permission_test_003 end`);
    })

    /**
     * @tc.name:      persist_permission_test_004
     * @tc.desc:      persist_permission with operation not permitted test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('persist_permission_test_004', 0, async function (done) {
        console.info(`persist_permission_test_004 start`);
        let uri = "/data/storage/el2/base/persist_permission_test_004.txt";
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policies = [policyInfo];
        try {
            fileShare.persistPermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`persist_permission_test_004 err : ${err.code}`);
                if (err.code == OPERATION_NOT_PERMITTED) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                done();
            })
        } catch (error) {
            console.info(`persist_permission_test_004 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`persist_permission_test_004 end`);
    })

    /**
     * @tc.name:      revoke_permission_test_001
     * @tc.desc:      normal uri and operationMode is READ_MODE test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('revoke_permission_test_001', 0, async function (done) {
        console.info(`revoke_permission_test_001 start`);
        let filePath = "/data/storage/el2/base/revoke_permission_test_001.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.READ_MODE };
        let policies = [policyInfo];
        try {
            await fileShare.persistPermission(policies);
        } catch (error) {
            console.info(`revoke_permission_test_001 persistPermission error : ${error.code}`);
        }
        try {
            fileShare.revokePermission(policies).then(() => {
                expect(true).assertTrue();
                done();
            }).catch((err) => {
                console.info(`revoke_permission_test_001 err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`revoke_permission_test_001 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`revoke_permission_test_001 end`);
    })
    
    /**
     * @tc.name:      revoke_permission_test_002
     * @tc.desc:      normal uris and have a uri is not exist test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('revoke_permission_test_002', 0, async function (done) {
        console.info(`revoke_permission_test_002 start`);
        let filePath = "/data/storage/el2/base/revoke_permission_test_002.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfoA = { uri: uri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policiesA = [policyInfoA];
        try {
            await fileShare.persistPermission(policiesA);
        } catch (error) {
            console.info(`revoke_permission_test_002 persistPermission error : ${error.code}`);
        }
        let fileUri = "/data/storage/el2/revoke_permission_test_002.txt";
        let policyInfoB = { uri: fileUri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policies = [policyInfoA, policyInfoB];
        try {
            fileShare.revokePermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`revoke_permission_test_002 err : ${err.code}`);
                if (err.code == OPERATION_NOT_PERMITTED) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                done();
            })
        } catch (error) {
            console.info(`revoke_permission_test_002 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`revoke_permission_test_002 end`);
    })

    /**
     * @tc.name:      revoke_permission_test_003
     * @tc.desc:      revoke_permission with no param test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('revoke_permission_test_003', 0, async function (done) {
        console.info(`revoke_permission_test_003 start`);
        try {
            fileShare.revokePermission().then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`revoke_permission_test_003 err: ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`revoke_permission_test_003 error: ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PARAMETER_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`revoke_permission_test_003 end`);
    })

    /**
     * @tc.name:      revoke_permission_test_004
     * @tc.desc:      revoke_permission with operation not permitted test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('revoke_permission_test_004', 0, async function (done) {
        console.info(`revoke_permission_test_004 start`);
        let filePath = "/data/storage/el2/base/revoke_permission_test_004.txt";
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
                console.info(`revoke_permission_test_004 err : ${err.code}`);
                if (err.code == OPERATION_NOT_PERMITTED) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                done();
            })
        } catch (error) {
            console.info(`revoke_permission_test_004 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`revoke_permission_test_004 end`);
    })

    /**
     * @tc.name:      activate_permission_test_001
     * @tc.desc:      normal uri and operationMode is READ_MODE test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('activate_permission_test_001', 0, async function (done) {
        console.info(`activate_permission_test_001 start`);
        let filePath = "/data/storage/el2/base/activate_permission_test_001.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.READ_MODE };
        let policies = [policyInfo];
        try {
            await fileShare.persistPermission(policies);
        } catch (error) {
            console.info(`activate_permission_test_001 persistPermission error : ${error.code}`);
        }
        try {
            fileShare.activatePermission(policies).then(() => {
                expect(true).assertTrue();
                done();
            }).catch((err) => {
                console.info(`activate_permission_test_001 err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`activate_permission_test_001 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            }  else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`activate_permission_test_001 end`);
    })
    
    /**
     * @tc.name:      activate_permission_test_002
     * @tc.desc:      normal uris and have a uri is not exist test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('activate_permission_test_002', 0, async function (done) {
        console.info(`activate_permission_test_002 start`);
        let filePath = "/data/storage/el2/base/activate_permission_test_002.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfoA = { uri: uri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policiesA = [policyInfoA];
        try {
            await fileShare.persistPermission(policiesA);
        } catch (error) {
            console.info(`activate_permission_test_002 persistPermission error : ${error.code}`);
        }
        let fileUri = "/data/storage/el2/activate_permission_test_002.txt";
        let policyInfoB = { uri: fileUri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policies = [policyInfoA, policyInfoB];
        try {
            fileShare.activatePermission(policies).then(() => {
                console.info(`activate_permission_test_002 fail`);
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`activate_permission_test_002 err : ${err.code}`);
                if (err.code == OPERATION_NOT_PERMITTED) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                done();
            })
        } catch (error) {
            console.info(`activate_permission_test_002 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`activate_permission_test_002 end`);
    })
    
    /**
     * @tc.name:      activate_permission_test_003
     * @tc.desc:      persist_permission with no param test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('activate_permission_test_003', 0, async function (done) {
        console.info(`activate_permission_test_003 start`);
        try {
            fileShare.activatePermission().then(() => {
                console.info(`activate_permission_test_003 success`);
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`activate_permission_test_003 err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`activate_permission_test_003 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PARAMETER_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`activate_permission_test_003 end`);
    })

    /**
     * @tc.name:      activate_permission_test_004
     * @tc.desc:      activate_permission with operation not permitted test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('activate_permission_test_004', 0, async function (done) {
        console.info(`activate_permission_test_004 start`);
        let filePath = "/data/storage/el2/base/activate_permission_test_004.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policies = [policyInfo];
        try {
            fileShare.activatePermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`activate_permission_test_004 err : ${err.code}`);
                if (err.code == OPERATION_NOT_PERMITTED) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                done();
            })
        } catch (error) {
            console.info(`activate_permission_test_004 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`activate_permission_test_004 end`);
    })

    /**
     * @tc.name:      deactivate_permission_test_001
     * @tc.desc:      normal uri and operationMode is READ_MODE test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('deactivate_permission_test_001', 0, async function (done) {
        console.info(`deactivate_permission_test_001 start`);
        let filePath = "/data/storage/el2/base/deactivate_permission_test_001.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.READ_MODE };
        let policies = [policyInfo];
        try {
            await fileShare.persistPermission(policies);
        } catch (error) {
            console.info(`deactivate_permission_test_001 persistPermission error : ${error.code}`);
        }
        try {
            await fileShare.activatePermission(policies);
        } catch (error) {
            console.info(`deactivate_permission_test_001 activatePermission error : ${error.code}`);
        }
        try {
            fileShare.deactivatePermission(policies).then(() => {
                expect(true).assertTrue();
                done();
            }).catch((err) => {
                console.info(`deactivate_permission_test_001 err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`deactivate_permission_test_001 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`deactivate_permission_test_001 end`);
    })
    
    /**
     * @tc.name:      deactivate_permission_test_002
     * @tc.desc:      normal uris and have a uri is not exist test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('deactivate_permission_test_002', 0, async function (done) {
        console.info(`deactivate_permission_test_002 start`);
        let filePath = "/data/storage/el2/base/deactivate_permission_test_002.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfoA = { uri: uri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policiesA = [policyInfoA];
        try {
            await fileShare.persistPermission(policiesA);
        } catch (error) {
            console.info(`deactivate_permission_test_002 persistPermission error : ${error.code}`);
        }
        try {
            await fileShare.activatePermission(policiesA);
        } catch (error) {
            console.info(`deactivate_permission_test_002 activatePermission error : ${error.code}`);
        }
        let fileUri = "/data/storage/el2/deactivate_permission_test_002.txt";
        let policyInfoB = { uri: fileUri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policies = [policyInfoA, policyInfoB];
        try {
            fileShare.deactivatePermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`deactivate_permission_test_002 err : ${err.code}`);
                if (err.code == OPERATION_NOT_PERMITTED) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                done();
            })
        } catch (error) {
            console.info(`deactivate_permission_test_002 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`deactivate_permission_test_002 end`);
    })
    
    /**
     * @tc.name:      deactivate_permission_test_003
     * @tc.desc:      persist_permission with no param test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('deactivate_permission_test_003', 0, async function (done) {
        console.info(`deactivate_permission_test_003 start`);
        try {
            fileShare.deactivatePermission().then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`deactivate_permission_test_003 err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`deactivate_permission_test_003 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PARAMETER_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`deactivate_permission_test_003 end`);
    })

    /**
     * @tc.name:      deactivate_permission_test_004
     * @tc.desc:      deactivate_permission with operation not permitted test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('deactivate_permission_test_004', 0, async function (done) {
        console.info(`deactivate_permission_test_004 start`);
        let filePath = "/data/storage/el2/base/deactivate_permission_test_004.txt";
        let uriObject = new fileuri.FileUri(filePath);
        let uri = uriObject.toString();
        let fd = await fs.open(uri, fs.OpenMode.CREATE);
        await fs.close(fd);
        let policyInfo = { uri: uri, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policies = [policyInfo];
        try {
            fileShare.deactivatePermission(policies).then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`deactivate_permission_test_004 err : ${err.code}`);
                if (err.code == OPERATION_NOT_PERMITTED) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
                done();
            })
        } catch (error) {
            console.info(`deactivate_permission_test_004 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`deactivate_permission_test_004 end`);
    })

    /**
     * @tc.name:      check_persistent_permission_test_001
     * @tc.desc:      check_persistent_permission with write mode and permitted test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('check_persistent_permission_test_001', 0, async function (done) {
        console.info(`check_persistent_permission_test_001 start`);
        let filePathA = "/data/storage/el2/base/check_persistent_permission_test_001.txt";
        let uriObjectA = new fileuri.FileUri(filePathA);
        let uriA = uriObjectA.toString();
        let fdA = await fs.open(uriA, fs.OpenMode.CREATE);
        await fs.close(fdA);
        let policyInfoA = { uri: uriA, operationMode: fileShare.OperationMode.WRITE_MODE };
        let policiesA = [policyInfoA];
        try {
            await fileShare.persistPermission(policiesA);
        } catch (error) {
            console.info(`check_persistent_permission_test_001 persistPermission error : ${error.code}`);
        }
        let policyInfoB = { uri: "/data/test_002.txt", operationMode: fileShare.OperationMode.WRITE_MODE };
        let policies = [policyInfoA, policyInfoB];
        try {
            fileShare.checkPersistentPermission(policies).then((result) => {
                expect(result[0]).assertTrue();
                expect(result[1] == false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`check_persistent_permission_test_001 err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`check_persistent_permission_test_001 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`check_persistent_permission_test_001 end`);
    })

    /**
     * @tc.name:      check_persistent_permission_test_002
     * @tc.desc:      check_persistent_permission with no param test
     * @tc.type:      FUNC test
     * @tc.require:   issueI#I956FN
     */
    it('check_persistent_permission_test_002', 0, async function (done) {
        console.info(`check_persistent_permission_test_002 start`);
        try {
            fileShare.checkPersistentPermission().then(() => {
                expect(false).assertTrue();
                done();
            }).catch((err) => {
                console.info(`check_persistent_permission_test_002 err : ${err.code}`);
                expect(false).assertTrue();
                done();
            })
        } catch (error) {
            console.info(`check_persistent_permission_test_002 error : ${error.code}`);
            if (error.code == DEVICE_TYPE_ERR) {
                expect(true).assertTrue();
            } else if (error.code == PARAMETER_TYPE_ERR) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        }
        console.info(`check_persistent_permission_test_002 end`);
    })
});