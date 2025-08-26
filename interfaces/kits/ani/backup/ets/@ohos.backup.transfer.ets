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

export namespace backup {
    interface Cleaner {
        clean(): void
    }
    class IncrBackupSessionCleaner implements Cleaner {
        static {
            loadLibrary("ani_session_transfer");
        }
        private session: long = 0
        private callbacks: long = 0
        constructor(session: long, callbacks: long) {
            this.session = session;
            this.callbacks = callbacks;
        }
        native clean(): void
    }

    class BackupSessionCleaner implements Cleaner {
        static {
            loadLibrary("ani_session_transfer");
        }
        private session: long = 0
        private callbacks: long = 0
        constructor(session: long, callbacks: long) {
            this.session = session;
            this.callbacks = callbacks;
        }
        native clean(): void
    }

    class RestoreSessionCleaner implements Cleaner {
        static {
            loadLibrary("ani_session_transfer");
        }
        private session: long = 0
        private incrSession: long = 0
        private callbacks: long = 0
        constructor(session: long, incrSession: long, callbacks: long) {
            this.session = session;
            this.incrSession = incrSession;
            this.callbacks = callbacks;
        }
        native clean(): void
    }

    export function callback(cleaner: Cleaner): void {
        cleaner.clean();
    }
    let destroyRegister = new FinalizationRegistry<Cleaner>(callback);
    let unregisterToken = new object()

    class BIncrementalData {
        public bundleName: string = '';
        public lastIncrementalTime: long = 0;
        public manifestFd: int = 0;
        public backupParameters: string = '';
        public backupPriority: int = 0;
    }

    class RestoreSession {
        public session: long = 0;
        public incrSession: long = 0;
        public callbacks: long = 0;
        private cleaner: RestoreSessionCleaner | null = null;
        constructor(session: long, incrSession: long, callbacks: long) {
            this.session = session;
            this.incrSession = incrSession;
            this.callbacks = callbacks;
            this.registerCleaner();
        }
        registerCleaner(): void {
            this.cleaner = new RestoreSessionCleaner(this.session, this.incrSession, this.callbacks);
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
    }

    class RestoreSessionTransfer {
        static {
            loadLibrary("ani_session_transfer");
        }
        private static native transferStaticSession(input: ESValue): Object;
        private static native transferDynamicSession(input: Object): ESValue;
        static transferStatic(input: Any): Object { // 1.1(dynamic) => 1.2(static)
            return RestoreSessionTransfer.transferStaticSession(ESValue.wrap(input));
        }
        static transferDynamic(input: Object): Any { // 1.2 => 1.1
            return RestoreSessionTransfer.transferDynamicSession(input);
        }
    }

    class BackupSession {
        public session: long = 0;
        public callbacks: long = 0;
        private cleaner: BackupSessionCleaner | null = null;
        constructor(session: long, callbacks: long) {
            this.session = session;
            this.callbacks = callbacks;
            this.registerCleaner();
        }
        registerCleaner(): void {
            this.cleaner = new BackupSessionCleaner(this.session, this.callbacks);
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
    }

    class BackupSessionTransfer {
        static {
            loadLibrary("ani_session_transfer");
        }
        private static native transferStaticSession(input: ESValue): Object;
        private static native transferDynamicSession(input: Object): ESValue;
        static transferStatic(input: Any): Object { // 1.1(dynamic) => 1.2(static)
            return BackupSessionTransfer.transferStaticSession(ESValue.wrap(input));
        }
        static transferDynamic(input: Object): Any { // 1.2 => 1.1
            return BackupSessionTransfer.transferDynamicSession(input);
        }
    }

    class IncrementalBackupSession {
        static {
            loadLibrary("ani_session_transfer");
        }

        public native getLocalCapabilities(): int;
        public native getBackupDataSize(isPreciseScan: boolean, bundleNames: Array<BIncrementalData>): int;
        public native appendBundles(bundleInfos: Array<BIncrementalData>): int;
        public native release(): int;
        public native cancel(bundleName: string): int;
        public native cleanBundleTempDir(bundleName: string): int;

        public session: long = 0;
        public callbacks: long = 0;
        private cleaner: IncrBackupSessionCleaner | null = null;
        constructor(session: long, callbacks: long) {
            this.session = session;
            this.callbacks = callbacks;
            this.registerCleaner();
        }

        registerCleaner(): void {
            this.cleaner = new IncrBackupSessionCleaner(this.session, this.callbacks);
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
    }

    class IncrBackupSessionTransfer {
        static {
            loadLibrary("ani_session_transfer");
        }

        private static native transferStaticSession(input: ESValue): Object;
        private static native transferDynamicSession(input: Object): ESValue;
        static transferStatic(input: Any): Object { // 1.1(dynamic) => 1.2(static)
            return IncrBackupSessionTransfer.transferStaticSession(ESValue.wrap(input));
        }
        static transferDynamic(input: Object): Any { // 1.2 => 1.1
            return IncrBackupSessionTransfer.transferDynamicSession(input);
        }
    }
}