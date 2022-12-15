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
/** @file tdi_attributes.h
 *
 *  @brief Contains TDI Attribute APIs
 */
#ifndef _TDI_ATTRIBUTES_H
#define _TDI_ATTRIBUTES_H

#include <tdi/common/tdi_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

tdi_status_t tdi_attributes_set_value(tdi_attributes_hdl *attributes,
                                      tdi_attributes_field_type_e type,
                                      const uint64_t value);

tdi_status_t tdi_attributes_get_value(const tdi_attributes_hdl *attributes,
                                      tdi_attributes_field_type_e type,
                                      uint64_t *value);

tdi_status_t tdi_attributes_set_value_ptr(tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                           const uint8_t *val,
                                           const size_t s);

tdi_status_t tdi_attributes_get_value_ptr(const tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                           const size_t size,
                                           uint8_t *val);
#ifdef __cplusplus
}
#endif

#endif  // _TDI_TABLE_ATTRIBUTES_H
