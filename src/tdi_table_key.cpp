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

#include <tdi/common/tdi_table_key.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

tdi_status_t TableKey::setValue(const tdi_id_t &field_id,
                                const tdi::KeyFieldValue &&field_value) {
  return this->setValue(field_id, field_value);
}

tdi_status_t TableKey::setValue(const tdi_id_t & /*field_id*/,
                                const tdi::KeyFieldValue & /*field_value*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableKey::getValue(const tdi_id_t & /*field_id*/,
                                tdi::KeyFieldValue * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableKey::tableGet(const Table **table) const {
  *table = table_;
  return TDI_SUCCESS;
}

tdi_status_t TableKey::reset() {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

}  // namespace tdi
