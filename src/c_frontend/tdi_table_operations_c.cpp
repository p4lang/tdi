/*
 * Copyright(c) 2023 Intel Corporation.
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
#ifdef __cplusplus
extern "C" {
#endif
#include <tdi/common/tdi_defs.h>

#include <tdi/common/c_frontend/tdi_operations.h>

#ifdef __cplusplus
}
#endif
#include <tdi/common/tdi_operations.hpp>

tdi_status_t tdi_operations_set_value(tdi_operations_hdl *operations_hdl,
                                      tdi_operations_field_type_e type,
                                      const uint64_t value) {
  auto operations_field = reinterpret_cast<tdi::TableOperations *>(operations_hdl);
  return operations_field->setValue(type, value);
}

tdi_status_t tdi_operations_get_value(const tdi_operations_hdl *operations_hdl,
                                      tdi_operations_field_type_e type,
                                      uint64_t *value) {
  auto operations_field = reinterpret_cast<const tdi::TableOperations *>(operations_hdl);
  return operations_field->getValue(type, value);
}

#if 0 // TODO
tdi_status_t tdi_operations_set_value_ptr(tdi_operations_hdl *operations_hdl,
                                      tdi_operations_field_type_e type,
                                           const uint8_t *val,
                                           const size_t s) {
  auto operations_field = reinterpret_cast<tdi::TableOperations *>(operations_hdl);
  return operations_field->setValue(type, val, s);
}

tdi_status_t tdi_operations_get_value_ptr(const tdi_operations_hdl *operations_hdl,
                                      tdi_operations_field_type_e type,
                                           const size_t size,
                                           uint8_t *val) {
  auto operations_field = reinterpret_cast<const tdi::TableOperations *>(operations_hdl);
  return operations_field->getValue(type, size, val);
}
#endif