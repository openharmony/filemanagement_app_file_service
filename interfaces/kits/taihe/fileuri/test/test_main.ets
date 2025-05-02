import fileUri from '@ohos.file.fileuri';

function main()
{
    test_GetUriFromPath_000();
    test_GetUriFromPath_001();
}

function test_GetUriFromPath_000()
{
    try {
      let path = "/data/storage/el2/base/haps/entry/files/sync.jpg";
      let uri = fileUri.getUriFromPath(path);
      let result = "file:///data/storage/el2/base/haps/entry/files/sync.jpg"
      console.log("test_GetUriFromPath_000 uri:" + uri);
      console.log("test_GetUriFromPath_000 result:" + result);
      if (uri == result) {
        console.log("test_GetUriFromPath_001 SUCCESS");
      }
    } catch (err) {
      console.log('test_FileUri_GetUriFromPath_000 has failed for ' + err);
    }
}

function test_GetUriFromPath_001()
{
    try {
      let uri = fileUri.getUriFromPath("");
      let result = "file://";
      console.log("test__GetUriFromPath_001 uri:" + uri);
      console.log("test__GetUriFromPath_001 result:" + result);
      if (uri == result) {
        console.log("test_GetUriFromPath_001 SUCCESS");
      }
    } catch (err) {
      console.log('test_GetUriFromPath_001 FAILED: ' + err);
    }
}