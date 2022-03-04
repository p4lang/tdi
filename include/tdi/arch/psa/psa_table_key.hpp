/*
 * Copyright(c) 2021 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this software except as stipulated in the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/** @file psa_table_key.hpp
 *
 *  @brief Contains TDI PSA Table Key APIs
 */
#ifndef _PSA_TABLE_KEY_HPP
#define _PSA_TABLE_KEY_HPP
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_table_key.hpp>

namespace tdi {

template<class T>
class KeyFieldValueOptional: public KeyFieldValue {
 public:
  KeyFieldValueOptional(T &value, const bool &is_valid)
      : value_(value), is_valid_(is_valid) {};
  KeyFieldValueOptional(const uint8_t *value_ptr, const bool is_valid, const size_t size)
      : value_ptr_(value_ptr), is_valid_(is_valid), size_(size) {};
  T value_ = 0;
  const uint8_t *value_ptr_ = nullptr;
  bool is_valid_ = 0;
  size_t size_ = 0;
};
} // tdi
#endif  // _PSA_TABLE_KEY_HPP
