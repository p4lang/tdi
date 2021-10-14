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

#include "target_init.hpp"

namespace tdi {
namespace tna {
namespace dummy {

tdi_status_t DevMgr::deviceAdd(
      const tdi_dev_id_t & /*device_id*/,
      const tdi_arch_type_e & /*arch_type*/,
      std::vector<std::unique_ptr<tdi::ProgramConfig>> & /*device_config*/,
      void * /*cookie*/) {
  return TDI_SUCCESS;
}

tdi_status_t DevMgr::deviceRemove(const tdi_dev_id_t & /*device_id*/) {
  return TDI_SUCCESS;
}


} // dummy
} // tna
} // tdi
