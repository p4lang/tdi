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

enum tdi_psa_match_type_e {
  TDI_PSA_MATCH_TYPE_OPTIONAL = TDI_MATCH_TYPE_ARCH,
};

namespace tdi {

template<class T = uint64_t>
class KeyFieldValueOptional: public KeyFieldValue {
 public:
  template <
      typename U = T,
      typename = typename std::enable_if<!std::is_pointer<U>::value>::type>
  KeyFieldValueOptional(U value, bool is_valid)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_PSA_MATCH_TYPE_OPTIONAL),
                      sizeof(value),
                      std::is_pointer<U>::value),
        value_(value),
        is_valid_(is_valid) {}
  template <typename U = T,
            typename = typename std::enable_if<std::is_pointer<U>::value>::type>
  KeyFieldValueOptional(U value, bool is_valid, size_t size)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_PSA_MATCH_TYPE_OPTIONAL),
                      size,
                      std::is_pointer<U>::value),
        value_(value),
        is_valid_(is_valid){}
  T value_ = 0;
  bool is_valid_ = 0;
};

} // tdi
#endif  // _PSA_TABLE_KEY_HPP
