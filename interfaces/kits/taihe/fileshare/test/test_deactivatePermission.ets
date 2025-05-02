import fileShare from '@ohos.fileshare';

/**
* @tc.number SUB_BASIC_FM_FileAPI_FileShare_DeactivatePermission_0100
* @tc.name FileShare_deactivatePermission_001
* @tc.desc Test deactivatePermission() interfaces
* Test normal uri and OperationMode is READ_MODE.
* @tc.size MEDIUM
* @tc.type Function
* @tc.level Level 0
* @tc.require
*/
function FileShare_deactivatePermission_001()
{
    let fileStr = "/data/storage/el2/base/haps/entry/files/FileShare_deactivatePermission_001.txt";
    let uri = fileStr;
    let policyInfo : fileShare.PolicyInfo = {
        uri: uri, 
        operationMode: fileShare.OperationMode.READ_MODE,
    };
    let policies = [policyInfo];

    fileShare.deactivatePermission(policies).then(() => {
        console.log("FileShare_deactivatePermission_001 successfully");

    }).catch((err) => {
        console.log("FileShare_deactivatePermission_001 failed with error message " + err.message + ", error code: " + err.code);

    });

}

/**
* @tc.number SUB_BASIC_FM_FileAPI_FileShare_DeactivatePermission_0200
* @tc.name FileShare_deactivatePermission_002
* @tc.desc Test deactivatePermission() interfaces
* Test normal uri and OperationMode is READ_MODE and WRITE_MODE .
* @tc.size MEDIUM
* @tc.type Function
* @tc.level Level 0
* @tc.require
*/
function FileShare_deactivatePermission_002()
{
    let fileStr = "/data/storage/el2/base/haps/entry/files/FileShare_deactivatePermission_002.txt";
    let uri = fileStr;
    let policyInfo: fileShare.PolicyInfo ={
        uri: uri, 
        operationMode: fileShare.OperationMode.READ_MODE | fileShare.OperationMode.WRITE_MODE,
    };
    let policies = [policyInfo];

    fileShare.deactivatePermission(policies).then(() => {
        console.log("FileShare_deactivatePermission_002 successfully");
    }).catch((err) => {
        console.log("FileShare_deactivatePermission_002 failed with error message " + err.message + ", error code: " + err.code);
    });
}

/**
* @tc.number SUB_BASIC_FM_FileAPI_FileShare_DeactivatePermission_0300
* @tc.name FileShare_deactivatePermission_003
* @tc.desc Test deactivatePermission() interfaces
* Test INVALID_PATH uri and OperationMode is READ_MODE.
* @tc.size MEDIUM
* @tc.type Function
* @tc.level Level 3
* @tc.require
*/
function FileShare_deactivatePermission_003()
{
    let fileStr1 = "/data/storage/el2/base/haps/entry/files/FileShare_deactivatePermission_003.txt";
    let uri1 = fileStr1 ;
    let policyInfo1 : fileShare.PolicyInfo= {
        uri: uri1, 
        operationMode: fileShare.OperationMode.READ_MODE,
    };

    let fileStr2 = "/data/storage/el2/base/haps/entry/files/FileShare_deactivatePermission_00301.txt";
    let uri2 = fileStr2;
    let policyInfo2 : fileShare.PolicyInfo = {
        uri: uri2, 
        operationMode: fileShare.OperationMode.READ_MODE,
    };
    let policies = [policyInfo1, policyInfo2];

    fileShare.deactivatePermission(policies).then(() => {
        console.log("FileShare_deactivatePermission_003 successfully");
    }).catch((err) => {
        console.info("FileShare_deactivatePermission_003 failed with error message: " + err.message + ", error code: " + err.code);
    });

}

export function testDeactivatePermission()
{
    console.log("FileShare_deactivatePermission_001 test begin");
    FileShare_deactivatePermission_001();
    console.log("FileShare_deactivatePermission_001 test end");

    console.log("FileShare_deactivatePermission_002 test begin");
    FileShare_deactivatePermission_002();
    console.log("FileShare_deactivatePermission_002 test end");

    console.log("FileShare_deactivatePermission_003 test begin");
    FileShare_deactivatePermission_003();
    console.log("FileShare_deactivatePermission_003 test end");
}