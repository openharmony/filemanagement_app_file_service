/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import type { AsyncCallback } from '@ohos.base';
import { BusinessError } from '@ohos.base';
import type wantConstant from '@ohos.app.ability.wantConstant';

export namespace fileShare {
  loadLibrary("fileshare_ani")

  export function grantUriPermission(uri: string, bundleName: string, flag: wantConstant.Flags,
    callback: AsyncCallback<void>): void {
    console.log("Start grantUriPermission in callback main thread.");
    let p1 = taskpool.execute(grantUriPermissionSync, uri, bundleName, flag);
    p1.then((err: NullishType) => {
      let error: BusinessError<void>;
      callback(error, undefined);
    });
    p1.catch((err: NullishType) => {
      console.log("grantUriPermission catch in callback thread.");
      let error: BusinessError<void>;
      callback(error, undefined);
    });
  }

  export function grantUriPermission(uri: string, bundleName: string, flag: wantConstant.Flags): Promise<void> {
    console.log("Start grantUriPermission in promise main thread");
    let p = new Promise<void>((resolve: (value: undefined) => void, reject: (error: Object) => void): void => {
      let p1 = taskpool.execute(grantUriPermissionSync, uri, bundleName, flag);
      p1.then((e: NullishType): void => {
        resolve(undefined);
      });
      p1.catch((e: Error): void => {
        console.log("grantUriPermission catch in promise thread.");
        reject(e);
      });
    });
    return p;
  }

  export function grantUriPermissionSync(uri: string, bundleName: string, flag: wantConstant.Flags):int {
    console.log("Start grantUriPermissionSync in thread.");
    grantUriPermissionInner(uri, bundleName, flag);
    return 0;
  }
  native function grantUriPermissionInner(uri: string, bundleName: string, flag: wantConstant.Flags):void;
}
