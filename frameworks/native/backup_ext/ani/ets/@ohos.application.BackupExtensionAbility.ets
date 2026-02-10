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

import BackupExtensionContext from '@ohos.file.BackupExtensionContext';

export interface BundleVersion {
  code: long;
  name: string;
}

export class BundleVersionInner implements BundleVersion {
  constructor(code: long, name: string) {
    this.code = code;
    this.name = name;
  }
  public code: long;
  public name: string = '';
}

export default class BackupExtensionAbility {
  public context: BackupExtensionContext = {};

  private etsCallOnBackupEx(backupInfo: string): string {
    let p = this.onBackupEx(backupInfo);
    if (p instanceof Promise) {
        return waitForCompletion(async () => {
          return await(p as Promise<string>);
        });
    } else {
        return p;
    }
  }

  private etsCallOnRestoreEx(bundleVersion: BundleVersion, restoreInfo: string): string {
    let p = this.onRestoreEx(bundleVersion, restoreInfo);
    if (p instanceof Promise) {
        return waitForCompletion(async () => {
          return await(p as Promise<string>);
        });
    } else {
        return p;
    }
  }

  private etsCallOnRelease(scenario: int): void {
    let p = this.onRelease(scenario);
    return waitForCompletion(async () => {
      return await(p as Promise<void>);
    });
  }

  private etsCallGetBackupCompatibilityInfo(extInfo: string): string {
    let p = this.getBackupCompatibilityInfo(extInfo);
    return waitForCompletion(async () => {
      return await p;
    })
  }

  private etsCallGetRestoreCompatibilityInfo(extInfo: string): string {
    let p = this.getRestoreCompatibilityInfo(extInfo);
    return waitForCompletion(async () => {
      return await p;
    });
  }

  onBackup(): void {}

  onBackupEx(backupInfo: string): string | Promise<string> {
    return '';
  }

  onRestore(bundleVersion: BundleVersion): void {}

  onRestoreEx(bundleVersion: BundleVersion, restoreInfo: string): string | Promise<string> {
    return '';
  }

  getBackupInfo(): string {
    return '';
  }

  onProcess(): string {
    return '';
  }

  onRelease(scenario: int): Promise<void> {
    return Promise.resolve();
  }

  getBackupCompatibilityInfo(extInfo: string): Promise<string> {
    let ret: string = '{"scenario": "backup"}';
    return Promise.resolve(ret);
  }

  getRestoreCompatibilityInfo(extInfo: string): Promise<string> {
    let ret: string = '{"scenario": "restore"}';
    return Promise.resolve(ret);
  }
}