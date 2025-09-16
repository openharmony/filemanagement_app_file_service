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

import uri from '@ohos.uri';

class Cleaner {
    static callback(cleaner: Cleaner): void {
        console.println("enter Cleaner.callback");
        cleaner.clean()
    }

    constructor(targetPtr: long) {
        this.targetPtr = targetPtr
    }

    native clean(): void

    private targetPtr: long = 0
};

class FinalizationAgent<T extends Object> {
    constructor(obj: T, ptr: long) {
        this.register(obj, ptr);
    }

    register(obj: T, ptr: long): void {
        if (this.unregisterToken) {
            this.unregister();
        }
        this.unregisterToken = {};
        this.cleaner = new Cleaner(ptr);
        finalizer.register(obj, this.cleaner!, this.unregisterToken);
    }

    unregister(): void {
        finalizer.unregister(this.unregisterToken);
    }

    private cleaner: Cleaner | null = null;
    private unregisterToken: object = {};
}

let finalizer = new FinalizationRegistry<Cleaner>(Cleaner.callback)

export default namespace fileUri {
    loadLibraryWithPermissionCheck("fileuri_ani", "@ohos.file.fileuri")

    export class FileUri {
        native constructor(uriOrPath: string);
        getFileUriEntity():long {
            return this.fileUriEntity_;
        }

        private fileUriEntity_: long = 0;
        private acquireFileUriEntity(fileUriEntity: long) {
            this.fileUriEntity_ = fileUriEntity;
            if (this.fzAgent) {
                this.fzAgent.unregister();
            }
            this.fzAgent = new FinalizationAgent<FileUri>(this, this.fileUriEntity_);
        }

        unregisterCleaner(): void {
            this.fzAgent.unregister();
        }

        private fzAgent: FinalizationAgent<FileUri>;
    };
    export native function getUriFromPath(path: string): string;
}
