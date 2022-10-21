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
#include <tdi/arch/tna/tna_defs.h>
#include <tdi/arch/tna/tna_init.hpp>

//local includes
#include <tdi/common/tdi_utils.hpp>

namespace tdi {
namespace tna {

tdi_status_t Device::createSession(std::shared_ptr<tdi::Session> * /*session*/) const {
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Device::createTarget(std::unique_ptr<tdi::Target> *target) const {
  *target = std::unique_ptr<tdi::Target>(new tdi::tna::Target(this->device_id_,
                                                              TNA_DEV_PIPE_ALL,
                                                              TNA_DIRECTION_ALL));
  return TDI_SUCCESS;
}

} // tna
}  // tdi
