import fileShare from '@ohos.fileshare';

/**
* @tc.number SUB_BASIC_FM_FileAPI_FileShare_ActivatePermission_0100
* @tc.name FileShare_activatePermission_001
* @tc.desc Test activatePermission() interfaces
* Test normal uri and OperationMode is READ_MODE.
* @tc.size MEDIUM
* @tc.type Function
* @tc.level Level 0
* @tc.require
*/
function FileShare_activatePermission_001()
{
    let fileStr = "/data/storage/el2/base/haps/entry/files/FileShare_activatePermission_001.txt";
    let uri = fileStr;
    let policyInfo : fileShare.PolicyInfo = {
        uri: uri, 
        operationMode: fileShare.OperationMode.READ_MODE,
    };
    let policies = [policyInfo];

    fileShare.activatePermission(policies).then(() => {
        console.log("FileShare_activatePermission_001 successfully");
    }).catch((err) => {
        console.log("FileShare_activatePermission_001 failed with error message: " + err.message + ", error code: " + err.code);
    });

}


/**
* @tc.number SUB_BASIC_FM_FileAPI_FileShare_ActivatePermission_0200
* @tc.name FileShare_activatePermission_002
* @tc.desc Test activatePermission() interfaces
* Test normal uri and OperationMode is READ_MODE and WRITE_MODE .
* @tc.size MEDIUM
* @tc.type Function
* @tc.level Level 0
* @tc.require
*/
function FileShare_activatePermission_002()
{

    let fileStr = "/data/storage/el2/base/haps/entry/files/FileShare_activatePermission_002.txt";
    let uri = fileStr;
    let policyInfo: fileShare.PolicyInfo ={
        uri: uri, 
        operationMode: fileShare.OperationMode.READ_MODE | fileShare.OperationMode.WRITE_MODE,
    };
    let policies = [policyInfo];

    fileShare.activatePermission(policies).then(() => {
        console.info("FileShare_activatePermission_002 successfully");
    }).catch((err) => {
        console.info("FileShare_activatePermission_002 failed with error message: " + err.message + ", error code: " + err.code);
    });
}

/**
* @tc.number SUB_BASIC_FM_FileAPI_FileShare_ActivatePermission_0300
* @tc.name FileShare_activatePermission_003
* @tc.desc Test activatePermission() interfaces
* Test INVALID_PATH uri and OperationMode is READ_MODE.
* @tc.size MEDIUM
* @tc.type Function
* @tc.level Level 3
* @tc.require
*/
function FileShare_activatePermission_003()
{

    let fileStr1 = "/data/storage/el2/base/haps/entry/files/FileShare_activatePermission_003.txt";
    let uri1 = fileStr1;
    let policyInfo1 : fileShare.PolicyInfo = {
        uri: uri1, 
        operationMode: fileShare.OperationMode.READ_MODE,
    };

    let fileStr2 = "/data/storage/el2/base/haps/entry/files/FileShare_activatePermission_00301.txt";
    let uri2 = fileStr2;
    let policyInfo2 : fileShare.PolicyInfo = {
        uri: uri2, 
        operationMode: fileShare.OperationMode.READ_MODE,
    };
    let policies = [policyInfo1, policyInfo2];

    fileShare.activatePermission(policies).then(() => {
        console.info("FileShare_activatePermission_003 successfully");
    }).catch((err) => {
        console.info("FileShare_activatePermission_003 failed with error message: " + err.message + ", error code: " + err.code);
    });

}

export function testActivatePermission()
{
    console.log("FileShare_activatePermission_001 test begin");
    FileShare_activatePermission_001();
    console.log("FileShare_activatePermission_001 test end");

    console.log("FileShare_activatePermission_002 test begin");
    FileShare_activatePermission_002();
    console.log("FileShare_activatePermission_002 test end");

    console.log("FileShare_activatePermission_003 test begin");
    FileShare_activatePermission_003();
    console.log("FileShare_activatePermission_003 test end");
}