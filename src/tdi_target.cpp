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
// tdi includes
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_init.hpp>
#include <tdi/common/tdi_defs.h>

// local includes
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

tdi_status_t Target::setValue(const tdi_target_e &target_e,
                              const uint64_t &value) {
  if (target_e == static_cast<tdi_target_e>(TDI_TARGET_DEV_ID)) {
    this->dev_id_ = value;
  } else {
    return TDI_INVALID_ARG;
  }
  return TDI_SUCCESS;
}

tdi_status_t Target::getValue(const tdi_target_e &target_e,
                              uint64_t *value) const {
  if (target_e == static_cast<tdi_target_e>(TDI_TARGET_DEV_ID)) {
    *value = this->dev_id_;
  } else {
    return TDI_INVALID_ARG;
  }
  return TDI_SUCCESS;
}

tdi_status_t Flags::setValue(const tdi_flags_e &flags_e, const bool &val) {
  if (val)
    this->flags_ |= 1ull << flags_e;
  else
    this->flags_ &= ~(1ull << flags_e);
  return TDI_SUCCESS;
}

tdi_status_t Flags::getValue(const tdi_flags_e &flags_e, bool *value) const {
  *value = (this->flags_ & (1ull << flags_e));
  return TDI_SUCCESS;
}

}  // namespace tdi
