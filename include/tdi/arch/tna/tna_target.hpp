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
/** @file tna_defs.hpp
 *
 *  @brief Contains TNA specific common defs like Target
 */
#ifndef _TNA_DEFS_HPP_
#define _TNA_DEFS_HPP_

#include <functional>
#include <memory>
#include <mutex>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_target.hpp>

// tna include
#include <tdi/arch/tna/tna_defs.h>
#include <tdi/arch/tna/tna_init.hpp>

namespace tdi {
namespace tna {

class Device;

enum tna_target_e {
  TNA_TARGET_PIPE_ID = TDI_TARGET_ARCH,
  TNA_TARGET_DIRECTION,
};

/**
 * @brief Can be constructed by \ref tna::Device::createTarget()
 */
class Target : public tdi::Target {
 public:
  virtual ~Target() = default;
  virtual tdi_status_t setValue(const tdi_target_e & /*target*/,
                        const uint32_t & /*value*/) override;
  virtual tdi_status_t getValue(const tdi_target_e & /*target*/,
                        uint32_t * /*value*/) override;

 protected:
  Target(const tdi_dev_id_t &dev_id,
         const tna_pipe_id_t &pipe_id,
         const tna_direction_e& direction) :
    tdi::Target(dev_id), pipe_id_(pipe_id), direction_(direction){};
 private:
  friend class tdi::tna::Device;
  tna_pipe_id_t pipe_id_;
  tna_direction_e direction_;
};

}  // tna
}  // tdi

#endif  // _TNA_DEFS_HPP_
