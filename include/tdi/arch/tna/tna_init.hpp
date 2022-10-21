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
/** @file tna_init.hpp
 *
 *  @brief Contains TNA specific Device and init defs
 */
#ifndef _TNA_INIT_HPP_
#define _TNA_INIT_HPP_

#include <functional>
#include <memory>
#include <mutex>

// tdi includes
#include <tdi/common/tdi_init.hpp>

// tna includes
#include <tdi/arch/tna/tna_defs.h>
#include <tdi/arch/tna/tna_target.hpp>

namespace tdi {
namespace tna {

/**
 * @brief Class which encapsulates static info of a TNA
 * device eg.
 */
class Device : public tdi::Device {
 public:
  Device(const tdi_dev_id_t &device_id,
         const tdi_arch_type_e &arch_type,
         const std::vector<tdi::ProgramConfig> &device_config,
         void *cookie)
      : tdi::Device(
            device_id, arch_type, device_config, cookie){};

  virtual tdi_status_t createSession(
      std::shared_ptr<tdi::Session> *session) const override;
  virtual tdi_status_t createTarget(
      std::unique_ptr<tdi::Target> *target) const override;

 private:
};

}  // namespace tna
}  // namespace tdi

#endif  // _TNA_INIT_HPP_
