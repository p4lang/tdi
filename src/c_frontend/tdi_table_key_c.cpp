/*******************************************************************************
 * BAREFOOT NETWORKS CONFIDENTIAL & PROPRIETARY
 *
 * Copyright (c) 2017-2018 Barefoot Networks, Inc.

 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property of
 * Barefoot Networks, Inc. and its suppliers, if any. The intellectual and
 * technical concepts contained herein are proprietary to Barefoot Networks,
 * Inc.
 * and its suppliers and may be covered by U.S. and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from
 * Barefoot Networks, Inc.
 *
 * No warranty, explicit or implicit is provided, unless granted under a
 * written agreement with Barefoot Networks, Inc.
 *
 * $Id: $ *
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <tdi/common/c_frontend/tdi_table_key.h>

#ifdef __cplusplus
}
#endif
#include <string.h>
#include <tdi/common/tdi_table_key.hpp>
//#include <tdi/common/tdi_table_key_obj.hpp>
//#include <tdi_common/tdi_table_key_impl.hpp>

/** Set */
/* Exact */
tdi_status_t tdi_key_field_set_value(tdi_table_key_hdl *key_hdl,
                                      const tdi_id_t field_id,
                                      const uint64_t value) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueExact <const long unsigned int> keyFieldValue(value);
  return key->setValue(field_id, keyFieldValue);
}
tdi_status_t tdi_key_field_set_value_ptr(tdi_table_key_hdl *key_hdl,
                                          const tdi_id_t field_id,
                                          const uint8_t *value,
                                          const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueExact <const unsigned char> keyFieldValue(value, size);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_string(tdi_table_key_hdl *key_hdl,
                                             const tdi_id_t field_id,
                                             const char *value) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueString<const char *> keyFieldValue(value);
  return key->setValue(field_id, keyFieldValue);
}
/* Ternary */
tdi_status_t tdi_key_field_set_value_and_mask(tdi_table_key_hdl *key_hdl,
                                               const tdi_id_t field_id,
                                               const uint64_t value,
                                               const uint64_t mask) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  
  //return key->setValueandMask(field_id, value, mask);
  //return key->setValue(field_id, value, mask);
  tdi::KeyFieldValueTernary <const long unsigned int> keyFieldValue(value, mask);
  return key->setValue(field_id, keyFieldValue);
}
tdi_status_t tdi_key_field_set_value_and_mask_ptr(tdi_table_key_hdl *key_hdl,
                                                   const tdi_id_t field_id,
                                                   const uint8_t *value1,
                                                   const uint8_t *mask,
                                                   const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueTernary <const unsigned char > keyFieldValue(value1, mask, size);
  //return key->setValueandMask(field_id, value1, mask, size);
  return key->setValue(field_id, keyFieldValue);
}
/* Range */
tdi_status_t tdi_key_field_set_value_range(tdi_table_key_hdl *key_hdl,
                                            const tdi_id_t field_id,
                                            const uint64_t start,
                                            const uint64_t end) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueRange<const long unsigned int> keyFieldValue(start, end);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_range_ptr(tdi_table_key_hdl *key_hdl,
                                                const tdi_id_t field_id,
                                                const uint8_t start,
                                                const uint8_t end,
                                                const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueRange<const unsigned char > keyFieldValue(&start, &end, size);
  return key->setValue(field_id, keyFieldValue);
}
/* LPM */
tdi_status_t tdi_key_field_set_value_lpm(tdi_table_key_hdl *key_hdl,
                                          const tdi_id_t field_id,
                                          const uint64_t value1,
                                          const uint16_t p_length) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueLPM <const uint64_t> keyFieldValue(value1, p_length);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_lpm_ptr(tdi_table_key_hdl *key_hdl,
                                              const tdi_id_t field_id,
                                              const uint8_t *value1,
                                              const uint16_t p_length,
                                              const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueLPM <const uint8_t> keyFieldValue(value1, p_length, size);
  return key->setValue(field_id, keyFieldValue);
}

/* Optional */
tdi_status_t tdi_key_field_set_value_optional(tdi_table_key_hdl *key_hdl,
                                               const tdi_id_t field_id,
                                               const uint64_t value1,
                                               const bool is_valid) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueOptional<const uint8_t> keyFieldValue(value1, is_valid);
  return key->setValue(field_id, keyFieldValue);
}

tdi_status_t tdi_key_field_set_value_optional_ptr(tdi_table_key_hdl *key_hdl,
                                                   const tdi_id_t field_id,
                                                   const uint8_t *value1,
                                                   const bool is_valid,
                                                   const size_t size) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueOptional<const uint8_t> keyFieldValue(value1, is_valid, size);
  return key->setValue(field_id, keyFieldValue);
}

/** Get */
/* Exact */
tdi_status_t tdi_key_field_get_value(const tdi_table_key_hdl *key_hdl,
                                      const tdi_id_t field_id,
                                      uint64_t *value) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueExact <long unsigned int> keyFieldValue(*value);
  return key->getValue(field_id, &keyFieldValue);
}

tdi_status_t tdi_key_field_get_value_ptr(const tdi_table_key_hdl *key_hdl,
                                          const tdi_id_t field_id,
                                          const size_t size,
                                          uint8_t *value) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueExact <uint8_t> keyFieldValue(value, size);
  return key->getValue(field_id, &keyFieldValue);
}

/* string */
tdi_status_t tdi_key_field_get_value_string_size(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    uint32_t *str_size) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  std::string str;
  tdi::KeyFieldValueString<std::string> keyFieldValue(str);
  tdi_status_t status = key->getValue(field_id, &keyFieldValue);
  *str_size = str.size();
  return status;
}
tdi_status_t tdi_key_field_get_value_string(const tdi_table_key_hdl *key_hdl,
                                             const tdi_id_t field_id,
                                             char *value) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  std::string str;
  tdi::KeyFieldValueString<char *> keyFieldValue(value);
  tdi_status_t status = key->getValue(field_id, &keyFieldValue);
  //strncpy(value, str.c_str(), str.size());
  return status;
}
/* Ternary */
tdi_status_t tdi_key_field_get_value_and_mask(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    uint64_t *value1,
    uint64_t *value2) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueTernary<long unsigned int> keyFieldValue(*value1, *value2);
  return key->getValue(field_id, &keyFieldValue);
}

tdi_status_t tdi_key_field_get_value_and_mask_ptr(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    const size_t size,
    uint8_t *value1,
    uint8_t *value2) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueTernary<unsigned char> keyFieldValue(value1, value2, size);
  return key->getValue(field_id, &keyFieldValue);
}

/* Range */
tdi_status_t tdi_key_field_get_value_range(const tdi_table_key_hdl *key_hdl,
                                            const tdi_id_t field_id,
                                            uint64_t *start,
                                            uint64_t *end) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueRange<long unsigned int> keyFieldValue(*start, *end);
  return key->getValue(field_id, &keyFieldValue);
}

tdi_status_t tdi_key_field_get_value_range_ptr(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    const size_t size,
    uint8_t *start,
    uint8_t *end) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueRange<unsigned char> keyFieldValue(start, end, size);
  return key->getValue(field_id, &keyFieldValue);
}

/* LPM */
tdi_status_t tdi_key_field_get_value_lpm(const tdi_table_key_hdl *key_hdl,
                                          const tdi_id_t field_id,
                                          uint64_t *start,
                                          const uint16_t *p_length) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueLPM<long unsigned int> keyFieldValue(*start, *p_length);
  return key->getValue(field_id, &keyFieldValue);
}

tdi_status_t tdi_key_field_get_value_lpm_ptr(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    const size_t size,
    uint8_t *value1,
    const uint16_t *p_length) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueLPM<unsigned char> keyFieldValue(value1, *p_length, size);
  return key->getValue(field_id, &keyFieldValue);
}

/* OPTIONAL */
tdi_status_t tdi_key_field_get_value_optional(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    uint64_t *value,
    bool *is_valid) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueOptional<long unsigned int> keyFieldValue(*value, *is_valid);
  return key->getValue(field_id, &keyFieldValue);
}

tdi_status_t tdi_key_field_get_value_optional_ptr(
    const tdi_table_key_hdl *key_hdl,
    const tdi_id_t field_id,
    const size_t size,
    uint8_t *value1,
    bool *is_valid) {
  auto key = reinterpret_cast<const tdi::TableKey *>(key_hdl);
  tdi::KeyFieldValueOptional<unsigned char> keyFieldValue(value1, *is_valid, size);
  return key->getValue(field_id, &keyFieldValue);
}
