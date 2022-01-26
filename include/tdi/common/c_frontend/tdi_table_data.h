/*******************************************************************************
 * BAREFOOT NETWORKS CONFIDENTIAL & PROPRIETARY
 *
 * Copyright (c) 2017-2021 Barefoot Networks, Inc.

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
 * $Id: $
 *
 ******************************************************************************/
/** @file tdi_table_data.h
 *
 * @brief Contains TDI Table Data APIs
 */
#ifndef _TDI_TABLE_DATA_H
#define _TDI_TABLE_DATA_H

#include <tdi/common/c_frontend/tdi_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data Types each Data field value can take. This is NOT Data field Type
 */
typedef enum tdi_data_field_type_ {
  INT_ARR = 0,
  BOOL_ARR = 1,
  UINT64 = 2,
  BYTE_STREAM = 3,
  FLOAT = 4,
  CONTAINER = 5,
  STRING = 6,
  BOOL = 7,
  STRING_ARR = 8,
} tdi_data_type_t;

/**
 * @brief Set value. Only valid on fields of size <= 64 bits
 *
 * @param[in] data_hdl   Data object handle
 * @param[in] field_id   Field ID
 * @param[in] val        Cannot be greater in value than what the field size
 *                       allows. For example, if the field size is 3 bits,
 *                       passing in a value of 14 will throw an error even
 *                       though uint64_t can easily hold 14.
 *
 * @return Status of the API call
 */

tdi_status_t tdi_data_field_set_value(tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       const uint64_t val);

/**
 * @brief Set value. Valid only on fields with float type
 *
 * @param[in] data_hdl  Data object handle
 * @param[in] field_id  Field ID
 * @param[in] val       Float value
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_set_float(tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       const float val);

/**
 * @brief Set value. Valid on fields of all sizes
 *
 * @param[in] data_hdl Data object handle
 * @param[in] field_id Field ID
 * @param[in] val      Start address of the input Byte-array. The input needs
 *                     to be in network order with byte padding at the MSBs.
 * @param[in] s        Number of bytes of the byte-array. The input number of
 *                     bytes should ceil the size of the field which is in bits.
 *                     For example, if field size is 28 bits, then size should
 *                     be 4.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_set_value_ptr(tdi_table_data_hdl *data_hdl,
                                           const tdi_id_t field_id,
                                           const uint8_t *val,
                                           const size_t s);

/**
 * @brief Set value. Valid only on fields with integer array type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[in] val               An array representing the values to set
 * @param[in] num_array         Array length
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_set_value_array(tdi_table_data_hdl *data_hdl,
                                             const tdi_id_t field_id,
                                             const uint32_t *val,
                                             const uint32_t num_array);

/**
 * @brief Set value. Valid only on fields with bool array type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[in] val               An array representing the values to set
 * @param[in] num_array         Array length
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_set_value_bool_array(
    tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    const bool *val,
    const uint32_t num_array);

/**
 * @brief Set value. Valid only on fields with string array type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[in] val               An array representing the values to set
 * @param[in] num_array         Array length
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_set_value_str_array(tdi_table_data_hdl *data_hdl,
                                                 const tdi_id_t field_id,
                                                 const char *val);

/**
 * @brief Set value. Valid only on fields with container type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[in] val               Ptr array of data objects each representing
 *                              a container
 * @param[in] num_array         Array length
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_set_value_data_field_array(
    tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    tdi_table_data_hdl *val[],
    const uint32_t num_array);
/**
 * @brief Set value. Valid only on fields with bool type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[in] val               Bool value
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_set_bool(tdi_table_data_hdl *data_hdl,
                                      const tdi_id_t field_id,
                                      const bool val);

/**
 * @brief Set value. Valid only on fields with string type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[in] val               String value
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_set_string(tdi_table_data_hdl *data_hdl,
                                        const tdi_id_t field_id,
                                        const char *val);

/**
 * @brief Get value. Only valid on fields of size <= 64 bits
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] val              Pointer to the value to be filled in.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value(const tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       uint64_t *val);

/**
 * @brief Get value. Valid on fields of all sizes
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[in] size              Size of the byte array passed in. The size has
 *                              to be equal to the byte-padded size of the
 *                              field. For example field size of 12 bits needs
 *                              the size to be passed in as 2.
 * @param[out] val              Pointer to the value to be filled in. The array
 *                              is assumed to be able to hold "size" number of
 *                              bytes.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_ptr(const tdi_table_data_hdl *data_hdl,
                                           const tdi_id_t field_id,
                                           const size_t size,
                                           uint8_t *val);

/**
 * @brief Get value array size. Valid on fields of integer array type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] array_size       Size of the array for provided field_id.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size);

/**
 * @brief Get value. Valid on fields of integer array type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] val              Pointer to the vector to be filled in.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_array(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *val);

/**
 * @brief Get value array size. Valid on fields of bool array type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] array_size       Size of the array for provided field_id.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_bool_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size);

/**
 * @brief Get value. Valid on fields of bool array type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] val              Pointer to the vector to be filled in.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_bool_array(
    const tdi_table_data_hdl *data_hdl, const tdi_id_t field_id, bool *val);

/**
 * @brief Get value. Valid on fields of float type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] val              Pointer to the float value to be filled in.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_float(const tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       float *val);

/**
 * @brief Get value. Valid on fields of bool type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] val              Pointer to the bool value to be filled in.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_bool(const tdi_table_data_hdl *data_hdl,
                                      const tdi_id_t field_id,
                                      bool *val);
tdi_status_t tdi_data_field_get_value_str_array(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t size,
    char *val);

/**
 * @brief Get string array value size. Valid on fields of string array type.
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] str_size         Number of bytes required to fit all the strings
 *                              including the separators between them.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_str_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size);

/**
 * @brief Get string value size. Valid on fields of string type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] str_size         Number of bytes required to fit the string.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_string_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *str_size);

/**
 * @brief Get value. Valid on fields of string type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] val              String value
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_string(const tdi_table_data_hdl *data_hdl,
                                        const tdi_id_t field_id,
                                        char *val);

/**
 * @brief Get value. Valid on fields of uint64_t array type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] val              Pointer to the vector value to be filled in.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_u64_array(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint64_t *val);

/**
 * @brief Get value array size. Valid on fields of uint64_t array type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] array_size       Number of values present in the array.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_u64_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size);

/**
 * @brief Get value array size. Valid on fields of BfRtTableData type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] array_size       Number of values present in the array.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_data_field_array_size(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size);

/**
 * @brief Get value. Valid on fields of BfRtTableData type
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] data_hdl_ret     Pointer to the list of data object handles to be
 *                              filled in.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_get_value_data_field_array(
    const tdi_table_data_hdl *data_hdl,
    const tdi_id_t field_id,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Get actionId.
 *
 * @param[in] data_hdl          Data object handle
 * @param[out] action_id        Pointer to the action id to be filled in.
 *                              Applicable for data objects associated with
 *                              tables for which action id exist.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_action_id_get(const tdi_table_data_hdl *data_hdl,
                                     uint32_t *action_id);

/**
 * @brief Get whether the field is active or not.
 * If the data object was allocated with a certain set of fields, then only
 * those fields are considered active. For oneof fields:
 * 1. If allocation was done for all fields, then by default all oneof fields
 * are active in the beginning. The fields set with setValue will remain active
 * and other oneofs in their oneof-group will be turned off during setValue()
 * 2. If allocation was done for specific fields, then
 *  a. If all oneofs in a group were set, then behavior is just like above
 *  b. If only one of the oneofs were set, then only that field is expected
 *  to be set during setValue().
 *
 * @param[in] data_hdl          Data object handle
 * @param[in] field_id          Field ID
 * @param[out] is_active        Bool value indicating if a field is active
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_is_active(const tdi_table_data_hdl *data_hdl,
                                       const tdi_id_t field_id,
                                       bool *is_active);
#ifdef __cplusplus
}
#endif

#endif  // _TDI_TABLE_DATA_H
