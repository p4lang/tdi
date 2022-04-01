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
/** @file tdi_table_key.hpp
 *
 *  @brief Contains TDI Table Key APIs
 */
#ifndef _TDI_TABLE_KEY_HPP
#define _TDI_TABLE_KEY_HPP

#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <tdi/common/tdi_defs.h>

namespace tdi {

// Forward declarations
class Table;

/**
 * @brief Key Field Match Type. A key can have multiple fields,
 * each with a different match type
 */
enum tdi_match_type_core_e {
  TDI_MATCH_TYPE_EXACT = TDI_MATCH_TYPE_CORE,
  TDI_MATCH_TYPE_TERNARY,
  TDI_MATCH_TYPE_LPM,
};

class KeyFieldValue {
 public:
  virtual ~KeyFieldValue() = default;

 protected:
  KeyFieldValue(tdi_match_type_e match_type) : match_type_(match_type){};

 private:
  tdi_match_type_e match_type_;
};

template <class T>
class KeyFieldValueExact : public KeyFieldValue {
 public:
  KeyFieldValueExact(T &value)
      : KeyFieldValue(TDI_MATCH_TYPE_EXACT), value_(value){};
  KeyFieldValueExact(uint8_t *value_ptr, size_t &size)
      : KeyFieldValue(TDI_MATCH_TYPE_EXACT),
        value_ptr_(value_ptr),
        size_(size){};
  T value_ = 0;
  uint8_t *value_ptr_ = nullptr;
  size_t size_ = 0;
};

template <class T>
class KeyFieldValueTernary : public KeyFieldValue {
 public:
  KeyFieldValueTernary(T &value, T &mask)
      : KeyFieldValue(TDI_MATCH_TYPE_TERNARY), value_(value), mask_(mask){};
  KeyFieldValueTernary(uint8_t *value_ptr, uint8_t *mask_ptr, size_t &size)
      : KeyFieldValue(TDI_MATCH_TYPE_TERNARY),
        value_ptr_(value_ptr),
        mask_ptr_(mask_ptr),
        size_(size){};
  T value_ = 0;
  uint8_t *value_ptr_ = nullptr;
  T mask_ = 0;
  uint8_t *mask_ptr_ = nullptr;
  size_t size_ = 0;
};

template <class T>
class KeyFieldValueLPM : public KeyFieldValue {
 public:
  KeyFieldValueLPM(T &value, uint16_t &prefix_len)
      : KeyFieldValue(TDI_MATCH_TYPE_LPM),
        value_(value),
        prefix_len_(prefix_len){};
  KeyFieldValueLPM(uint8_t *value_ptr, uint16_t &prefix_len, size_t &size)
      : KeyFieldValue(TDI_MATCH_TYPE_LPM),
        value_ptr_(value_ptr),
        prefix_len_(prefix_len),
        size_(size){};
  T value_ = 0;
  uint8_t *value_ptr_ = nullptr;
  uint16_t prefix_len_ = 0;
  size_t size_ = 0;
};

/**
 * @brief Class to construct key for a table.<br>
 * <B>Creation: </B> Can only be created using \ref
 * tdi::Table::keyAllocate()
 */
class TableKey {
 public:
  TableKey(const Table *table) : table_(table){};
  virtual ~TableKey() = default;

  /**
   * @brief Set value. Only valid on fields of \ref tdi::KeyFieldType "EXACT"
   *
   * @param[in] field_id Field ID
   * @param[in] value   Value. Cannot be greater in value than what the field
   * size allows. For example, if the field size is 3 bits, passing in a value
   * of 14 will throw an error even though uint64_t can easily hold 14.
   *
   * @return Status of the API call
   */
  virtual tdi_status_t setValue(const tdi_id_t &field_id,
                                const tdi::KeyFieldValue &&field_value);

  virtual tdi_status_t setValue(const tdi_id_t &field_id,
                                const tdi::KeyFieldValue &field_value);

  /**
   * @brief Get value. Only valid on fields of \ref tdi::KeyFieldType "EXACT"
   *
   * @param[in] field_id Field ID
   * @param[out] value   Value.
   *
   * @return Status of the API call
   */
  virtual tdi_status_t getValue(const tdi_id_t &field_id,
                                tdi::KeyFieldValue *value) const;

  /**
   * @brief Get the Table Object associated with this Key Object
   *
   * @param[out] table Table Object
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tableGet(const tdi::Table **table) const;

  /**
   * @brief Reset the TableKey object
   *
   * @return
   */
  virtual tdi_status_t reset();

 protected:
  const Table *table_ = nullptr;
};

}  // namespace tdi

#endif  // _TDI_TABLE_KEY_HPP
