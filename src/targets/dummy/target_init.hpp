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
/** @file target_init.hpp
 *
 *  @brief Contains common target-specific init function decls
 */
#ifndef _TARGET_INIT_HPP_
#define _TARGET_INIT_HPP_

#include <vector>
#include <memory>

// tdi includes
#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_init.hpp>

namespace tdi {
namespace tna {
namespace dummy {

/**
 * @brief Class to manage Device per dev_id.<br>
 * <B>Creation: </B> Singleton....
 */
class DevMgr : public tdi::DevMgr {
 public:

  /**
   * @brief Device Add function which creates a Device object and maintains it
   *
   * @param[in] device_id 
   * @param[in] cookie User defined cookie which platforms can use to
   * send any additional information they want to help with inititalization
   *
   * @return Status of API call
   */
  tdi_status_t deviceAdd(
      const tdi_dev_id_t &device_id,
      const tdi_arch_type_e &arch_type,
      std::vector<std::unique_ptr<tdi::ProgramConfig>> &device_config,
      void *cookie);

  tdi_status_t deviceRemove(const tdi_dev_id_t &device_id);

};  // DevMgr

} // dummy
} // tna
} // tdi

#endif  // _TARGET_INIT_HPP_
