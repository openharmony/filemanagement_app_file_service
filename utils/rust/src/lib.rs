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

//! Rust utils for cxx
#[cxx::bridge]
mod ffi{
    #![allow(dead_code)]
    extern "Rust"{
        fn canonicalize(path: String) -> Result<String>;
    }
}

use std::fs;
use std::io::{Error, ErrorKind};

fn canonicalize(path: String) -> Result<String, Error> {
  match fs::canonicalize(path) {
    Ok(abs_path) =>  {
      match abs_path.to_str() {
        Some(path) => Ok(path.to_string()),
        None => Err(Error::new(ErrorKind::Other, "canonicalize failed")),
        }
    },
    Err(_) => Err(Error::new(ErrorKind::Other, "canonicalize failed")),
  }
}