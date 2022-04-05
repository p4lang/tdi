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
  TDI_MATCH_TYPE_RANGE,
};

/**
 * @brief Thin wrapper over Key Field values. Users are free to access the
 * public data members and manipulate if necessary.
 * Each derived class has 2 constructors, One is a templatized version of
 * primitive data types. The other one is byte array version. Both
 * TableKey::setValue and TableKey::getValue() expect memory to be allocated by
 * user if byte array versions are being used.
 *
 * Note for Target driver developers: match type is being saved so as to save
 * from dynamic casting and figure out what match type was used.
 *
 */
class KeyFieldValue {
 public:
  virtual ~KeyFieldValue() = default;
  KeyFieldValue() = delete;
  const tdi_match_type_e &matchTypeGet() const { return match_type_; };

 protected:
  KeyFieldValue(tdi_match_type_e match_type, const size_t &size)
      : match_type_(match_type), size_(size){};

 private:
  const tdi_match_type_e match_type_;

 public:
  // Size in bytes
  // Accessible publicly
  size_t size_{0};
};

template <class T = uint64_t>
class KeyFieldValueExact : public KeyFieldValue {
 public:
  KeyFieldValueExact(const T &value)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_EXACT),
                      sizeof(value)),
        value_(value){};
  KeyFieldValueExact(const uint8_t *value_ptr, const size_t &size)
      : KeyFieldValue(TDI_MATCH_TYPE_EXACT, size), value_ptr_(value_ptr){};
  T value_ = 0;
  uint8_t *value_ptr_ = nullptr;
};

template <class T = uint64_t>
class KeyFieldValueTernary : public KeyFieldValue {
 public:
  KeyFieldValueTernary(const T &value, const T &mask)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_TERNARY),
                      sizeof(value)),
        value_(value),
        mask_(mask){};
  KeyFieldValueTernary(const uint8_t *value_ptr,
                       const uint8_t *mask_ptr,
                       const size_t &size)
      : KeyFieldValue(TDI_MATCH_TYPE_TERNARY, size),
        value_ptr_(value_ptr),
        mask_ptr_(mask_ptr){};
  T value_ = 0;
  uint8_t *value_ptr_ = nullptr;
  T mask_ = 0;
  uint8_t *mask_ptr_ = nullptr;
};

template <class T = uint64_t>
class KeyFieldValueLPM : public KeyFieldValue {
 public:
  KeyFieldValueLPM(const T &value, const uint16_t &prefix_len)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_LPM),
                      sizeof(value)),
        value_(value),
        prefix_len_(prefix_len){};
  KeyFieldValueLPM(const uint8_t *value_ptr,
                   const uint16_t &prefix_len,
                   const size_t &size)
      : KeyFieldValue(TDI_MATCH_TYPE_LPM, size),
        value_ptr_(value_ptr),
        prefix_len_(prefix_len){};
  T value_ = 0;
  uint8_t *value_ptr_ = nullptr;
  uint16_t prefix_len_ = 0;
};

template <class T = uint64_t>
class KeyFieldValueRange : public KeyFieldValue {
 public:
  KeyFieldValueRange(const T &low, const T &high)
      : KeyFieldValue(static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_RANGE),
                      sizeof(low)),
        low_(low),
        high_(high){};
  KeyFieldValueRange(const uint8_t *low_ptr,
                     const uint8_t *high_ptr,
                     const size_t &size)
      : KeyFieldValue(TDI_MATCH_TYPE_LPM, size),
        low_ptr_(low_ptr),
        high_ptr_(high_ptr){};
  T low_ = 0;
  T high_ = 0;
  uint8_t *low_ptr_ = nullptr;
  uint8_t *high_ptr_ = nullptr;
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
  tdi_status_t setValue(const tdi_id_t &field_id,
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
