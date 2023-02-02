/*
* Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

import {AsyncCallback, Callback} from "./basic";

/**
 * Provides remote file share APIs
 *
 * @since 10
 * @sysCap N/A
 * @devices phone, tablet
 */
declare namespace Remotefileshare {
  /**
   * Create the remote share path of src share file.
   *
   * @since 10
   */
  function createSharePath(fd: number, cid: string, callback: AsyncCallback<string>): void;
  function createSharePath(fd: number, cid: string): Promise<string>;
}

export default Remotefileshare;
