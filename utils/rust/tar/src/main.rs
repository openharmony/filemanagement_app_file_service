/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 
use clap::Parser;
use std::path::PathBuf;
use ylong_runtime::{block_on, sync::mpsc::bounded::bounded_channel};

mod backup;

#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Tar file saving path
    #[clap(short, long)]
    stash_dir: String,

    /// Files and directories that you want to back up
    #[clap(short, long)]
    includes: Vec<String>,

    /// includes exceptions that do not need to be backed up
    #[clap(short, long)]
    excludes: Vec<String>,
}

fn backup_main() {
    let mut args = Args::parse();
    args.excludes.push(args.stash_dir.clone());
    println!("{:#?}", args);

    let (paths_to_backed_tx, paths_to_backed_rx) = bounded_channel(100);
    let (outputs_tx, mut outputs_rx) = bounded_channel(100);

    let handles = vec![
        ylong_runtime::spawn(async move {
            let _ = backup::scan_files(args.includes, args.excludes, paths_to_backed_tx)
                .await
                .unwrap();
        }),
        ylong_runtime::spawn(async move {
            let option = backup::Options {
                stash_dir: PathBuf::from(args.stash_dir),
                ..Default::default()
            };
            let _ = backup::backup_files(option, paths_to_backed_rx, outputs_tx)
                .await
                .unwrap();
        }),
        ylong_runtime::spawn(async move {
            while let Ok(archive) = outputs_rx.recv().await {
                println!("output: {:?}", archive);
            }
        }),
    ];
    for handle in handles {
        block_on(handle).unwrap();
    }
}

fn main() {
    backup_main();
}
