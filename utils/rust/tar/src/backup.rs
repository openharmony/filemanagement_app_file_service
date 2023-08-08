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

use glob::{Paths, Pattern};
#[cfg(feature = "compress_lz4_flex")]
use lz4_flex::frame::FrameEncoder;
use std::error::Error;
use std::fs::File;
use std::io;
use std::path::{Path, PathBuf};
use tar::Builder;
use ylong_runtime::sync::mpsc::bounded::{BoundedReceiver, BoundedSender};

#[derive(Clone, Copy)]
#[non_exhaustive]
pub enum CompressAlgorithm {
    #[allow(dead_code)]
    None,

    #[cfg(feature = "compress_lz4_flex")]
    #[allow(dead_code)]
    Lz4Flex,
}

pub struct Options {
    /// The working directory to store the files, default is "backup".
    pub stash_dir: PathBuf,

    /// The threshold to determine if a file is huge in byte. A huge file will be transferred directly without
    /// archiving. Currently the default value is `usize::MAX`, which means no file is huge.
    pub threshold_huge_file: usize,

    /// The threshold to split the tar in byte. A new tar will be created if the size of the current tar exceeds this
    /// threshold. Currently the default value is `usize::MAX`, which means no tar will be split.
    pub threshold_split_tar: usize,

    /// The compress algorithm to use, default is `CompressAlgorithm::None`.
    pub compress_algorithm: CompressAlgorithm,
}

#[derive(Debug)]
pub struct Archive {
    #[allow(dead_code)]
    /// The path of the archive file
    path: PathBuf,

    #[allow(dead_code)]
    /// The type of the archive file
    archive_type: ArchiveType,

    #[allow(dead_code)]
    /// The size of the file's content in byte
    content_size: usize,
}

#[derive(Debug)]
pub enum ArchiveType {
    /// Files are backup-ed directly
    Plain,

    /// Files are backup-ed in a tar
    Tar,
}

struct BackupContext {
    option: Options,

    cur_tar: Option<Output>,
    cur_tar_size: usize,
    tar_cnt: usize,

    inputs: BoundedReceiver<PathBuf>,
    outputs: BoundedSender<Archive>,
}

enum Output {
    Uncompressed(Builder<File>),

    #[cfg(feature = "compress_lz4_flex")]
    CompressedLz4Flex(Builder<FrameEncoder<File>>),
}

pub async fn scan_files<S>(
    includes: Vec<S>,
    excludes: Vec<S>,
    sender: BoundedSender<PathBuf>,
) -> Result<(), Box<dyn Error>>
where
    S: AsRef<str>,
{
    let exclude_patterns = build_exclude_patterns(excludes)?;
    traverse_and_send(includes, exclude_patterns, sender).await
}

pub async fn backup_files(
    option: Options,
    paths_receiver: BoundedReceiver<PathBuf>,
    archives_sender: BoundedSender<Archive>,
) -> Result<(), Box<dyn Error>> {
    BackupContext::new(option, paths_receiver, archives_sender)
        .archive()
        .await?;

    Ok(())
}

fn build_exclude_patterns<S>(excludes: Vec<S>) -> Result<Vec<Pattern>, Box<dyn Error>>
where
    S: AsRef<str>,
{
    for exclude in &excludes {
        if !exclude.as_ref().starts_with("/") {
            let err = io::Error::new(
                io::ErrorKind::InvalidInput,
                "exclude pattern must be absolute",
            );
            return Err(Box::new(err));
        }
    }

    let excludes = excludes.iter().map(|exclude| {
        if let Some(stripped) = exclude.as_ref().strip_suffix("/") {
            stripped
        } else {
            exclude.as_ref()
        }
    });

    let exclude_patterns = excludes
        .into_iter()
        .map(|p| glob::Pattern::new(p.as_ref()))
        .collect::<Result<Vec<Pattern>, _>>()?;

    Ok(exclude_patterns)
}

async fn traverse_and_send<S>(
    includes: Vec<S>,
    excludes: Vec<Pattern>,
    sender: BoundedSender<PathBuf>,
) -> Result<(), Box<dyn Error>>
where
    S: AsRef<str>,
{
    let mut option = glob::MatchOptions::new();
    option.require_literal_separator = true;

    let includes = includes
        .iter()
        .map(|p| glob::glob_with(p.as_ref(), option))
        .collect::<Result<Vec<Paths>, _>>()?;

    for path in includes.into_iter().flatten() {
        let path = path?;
        if excludes.iter().any(|p| p.matches_path(&path)) {
            continue;
        }

        let is_path_exclude =
            |path: &Path| -> bool { excludes.iter().any(|p| p.matches_path(path)) };

        let metadata = path.metadata()?;
        if metadata.is_file() {
            if !is_path_exclude(&path.as_path()) {
                sender.send(path).await?;
            }
        } else if metadata.is_dir() {
            let walker = walkdir::WalkDir::new(path);
            for entry in walker
                .into_iter()
                .filter_entry(|e| !is_path_exclude(e.path()))
            {
                let entry = entry?;
                if entry.file_type().is_file() {
                    sender.send(entry.path().to_path_buf()).await?;
                }
            }
        }
    }

    Ok(())
}

impl Default for Options {
    fn default() -> Self {
        Self {
            stash_dir: PathBuf::from("backup"),
            threshold_huge_file: usize::MAX, // TODO: fix me
            threshold_split_tar: usize::MAX, // TODO: fix me
            compress_algorithm: CompressAlgorithm::None,
        }
    }
}

impl Output {
    fn new(path: PathBuf, compress_algorithm: CompressAlgorithm) -> Result<Self, io::Error> {
        let prefix = path
            .parent()
            .expect(format!("failed to get parent of {:?}", path).as_str());
        if !prefix.exists() {
            std::fs::create_dir_all(prefix)
                .expect(format!("failed to create {:?}", prefix).as_str());
        }

        match compress_algorithm {
            CompressAlgorithm::None => Ok(Self::Uncompressed(Builder::new(File::create(path)?))),
            #[cfg(feature = "compress_lz4_flex")]
            CompressAlgorithm::Lz4Flex => Ok(Self::CompressedLz4Flex(Builder::new(
                FrameEncoder::new(File::create(path)?),
            ))),
        }
    }

    fn append_file(&mut self, achievable_path: &PathBuf) -> Result<usize, io::Error> {
        #[cfg(feature = "ohos")]
        assert_eq!(std::env::current_dir(), r#"/"#);

        let abs_path = achievable_path
            .canonicalize()
            .expect("File to append is not achievable");
        let relative_path = abs_path
            .strip_prefix(std::env::current_dir().expect("Cannot get current dir"))
            .expect("File to append is not in current dir");

        match self {
            Self::Uncompressed(builder) => {
                builder.append_path_with_name(&achievable_path, relative_path)?
            }

            #[cfg(feature = "compress_lz4_flex")]
            Self::CompressedLz4Flex(builder) => {
                builder.append_path_with_name(&achievable_path, relative_path)?
            }
        };
        Ok(std::fs::metadata(achievable_path)?.len() as usize)
    }

    fn finish(self) -> Result<(), io::Error> {
        match self {
            Self::Uncompressed(mut builder) => {
                builder.finish()?;
            }
            #[cfg(feature = "compress_lz4_flex")]
            Self::CompressedLz4Flex(builder) => {
                let encoder = builder.into_inner().expect("failed to get encoder");
                encoder.finish()?;
            }
        }
        Ok(())
    }
}

impl BackupContext {
    fn new(
        option: Options,
        inputs: BoundedReceiver<PathBuf>,
        outputs: BoundedSender<Archive>,
    ) -> Self {
        Self {
            option,
            cur_tar: None,
            cur_tar_size: 0,
            tar_cnt: 0,
            inputs,
            outputs,
        }
    }

    async fn archive(&mut self) -> Result<(), Box<dyn Error>> {
        while let Ok(path) = self.inputs.recv().await {
            assert!(path.exists());
            assert!(path.metadata()?.is_file());

            if let Some(archive) = self.archive_single_file(path).await? {
                self.outputs.send(archive).await?;
            }
        }

        if let Some(archive) = self.retrieve_cur_tar()? {
            self.outputs.send(archive).await?;
        }

        Ok(())
    }

    async fn archive_single_file(&mut self, path: PathBuf) -> Result<Option<Archive>, io::Error> {
        let file_size = path.metadata()?.len() as usize;

        if file_size > self.option.threshold_huge_file {
            return Ok(Some(Archive {
                path,
                archive_type: ArchiveType::Plain,
                content_size: file_size,
            }));
        }

        if self.cur_tar.is_none() {
            self.cur_tar = Some(Output::new(
                self.next_tar_path(),
                self.option.compress_algorithm,
            )?);
        }

        let cur_tar = self.cur_tar.as_mut().unwrap();
        self.cur_tar_size += cur_tar.append_file(&path)?;
        if self.cur_tar_size > self.option.threshold_split_tar {
            return self.retrieve_cur_tar();
        }

        Ok(None)
    }

    fn cur_tar_path(&self) -> PathBuf {
        let path = self.option.stash_dir.join(self.tar_cnt.to_string());
        match self.option.compress_algorithm {
            CompressAlgorithm::None => path.with_extension("tar"),

            #[cfg(feature = "compress_lz4_flex")]
            CompressAlgorithm::Lz4Flex => path.with_extension("tar.lz4"),
        }
    }

    fn next_tar_path(&mut self) -> PathBuf {
        self.tar_cnt += 1;
        self.cur_tar_path()
    }

    fn retrieve_cur_tar(&mut self) -> Result<Option<Archive>, io::Error> {
        if let None = self.cur_tar {
            return Ok(None);
        }
        let last_tar = self
            .cur_tar
            .take()
            .expect("last_tar is guaranteed to be Some");
        last_tar.finish()?;
        let archive = Archive {
            path: self.cur_tar_path(),
            archive_type: ArchiveType::Tar,
            content_size: self.cur_tar_size,
        };
        Ok(Some(archive))
    }
}
