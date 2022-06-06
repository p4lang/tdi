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

// arch include
#include <tdi/arch/tna/tna_target.hpp>

// local includes
#include <tdi/common/tdi_utils.hpp>

namespace tdi {
namespace tna {

tdi_status_t Target::setValue(const tdi_target_e &target_field,
                              const uint64_t &value) {
  if (target_field == static_cast<tdi_target_e>(TDI_TNA_TARGET_PIPE_ID)) {
    this->pipe_id_ = value;
  } else if (target_field ==
             static_cast<tdi_target_e>(TDI_TNA_TARGET_DIRECTION)) {
    this->direction_ = static_cast<tna_direction_e>(value);
  } else {
    return tdi::Target::setValue(target_field, value);
  }
  return TDI_SUCCESS;
}

tdi_status_t Target::getValue(const tdi_target_e &target_field,
                              uint64_t *value) const {
  if (target_field == static_cast<tdi_target_e>(TDI_TNA_TARGET_PIPE_ID)) {
    *value = this->pipe_id_;
  } else if (target_field ==
             static_cast<tdi_target_e>(TDI_TNA_TARGET_DIRECTION)) {
    *value = static_cast<uint64_t>(this->direction_);
  } else {
    return tdi::Target::getValue(target_field, value);
  }
  return TDI_SUCCESS;
}

}  // namespace tna
}  // namespace tdi
