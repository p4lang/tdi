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
/** @file tdi_operations.h
 *
 * @brief Contains TDI Table operations APIs
 */
#ifndef _TDI_OPERATIONS_H
#define _TDI_OPERATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

tdi_status_t tdi_operations_set_value(tdi_operations_hdl operations,
                                      tdi_operations_field_type_e type,
                                      const uint64_t value);

tdi_status_t tdi_operations_get_value(tdi_operations_hdl operations,
                                      tdi_operations_field_type_e type,
                                      uint64_t *value);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_OPERATIONS_H
