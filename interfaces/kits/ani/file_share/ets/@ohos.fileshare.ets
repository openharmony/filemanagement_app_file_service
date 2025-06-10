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
import hilog from '@ohos.hilog'

export namespace fileShare {
  loadLibrary("fileshare_ani")

  export enum OperationMode {
      READ_MODE = 0b1,
      WRITE_MODE = 0b10
  }
  export interface PolicyInfo {
      uri: string;
      operationMode: int;
  }

  class PolicyInfo_inner implements PolicyInfo {
      uri: string;
      operationMode: int;
  }

  export function grantUriPermission(uri: string, bundleName: string, flag: wantConstant.Flags,
    callback: AsyncCallback<void>): void {
    hilog.info(0x0000, 'grantUriPermission', "Start grantUriPermission in callback main thread.");
    let p1 = taskpool.execute(grantUriPermissionSync, uri, bundleName, flag);
    p1.then((err: NullishType) => {
      let error: BusinessError<void>;
      callback(error, undefined);
    });
    p1.catch((err: NullishType) => {
      hilog.info(0x0000, 'grantUriPermission', "grantUriPermission catch in callback thread.");
      let error =  err as BusinessError<void>;
      callback(error, undefined);
    });
  }

  export function grantUriPermission(uri: string, bundleName: string, flag: wantConstant.Flags): Promise<void> {
    hilog.info(0x0000, 'grantUriPermission', "Start grantUriPermission in promise main thread");
    let p = new Promise<void>((resolve: (value: PromiseLike<void>) => void, reject: (error: Error) => void): void => {
      let p1 = taskpool.execute(grantUriPermissionSync, uri, bundleName, flag);
      p1.then((e: NullishType): void => {
        resolve(Promise.resolve());
      });
      p1.catch((e: Error): void => {
        hilog.info(0x0000, 'grantUriPermission', "grantUriPermission catch in promise thread.");
        reject(e);
      });
    });
    return p;
  }

  export function grantUriPermissionSync(uri: string, bundleName: string, flag: wantConstant.Flags):int {
    hilog.info(0x0000, 'grantUriPermission', "Start grantUriPermissionSync in thread.");
    grantUriPermissionInner(uri, bundleName, flag);
    return 0;
  }
  native function grantUriPermissionInner(uri: string, bundleName: string, flag: wantConstant.Flags):void;
}
