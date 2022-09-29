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
#ifdef __cplusplus
extern "C" {
#endif
#include <tdi/common/tdi_defs.h>

#include <tdi/common/c_frontend/tdi_attributes.h>

#ifdef __cplusplus
}
#endif
#include <tdi/common/tdi_attributes.hpp>

tdi_status_t tdi_attributes_set_value(tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                      const uint64_t value) {
  auto attributes_field = reinterpret_cast<tdi::TableAttributes *>(attributes_hdl);
  return attributes_field->setValue(type, value);
}

tdi_status_t tdi_attributes_get_value(const tdi_attributes_hdl *attributes_hdl,
                                      tdi_attributes_field_type_e type,
                                      uint64_t *value) {
  auto attributes_field = reinterpret_cast<const tdi::TableAttributes *>(attributes_hdl);
  return attributes_field->getValue(type, value);
}

