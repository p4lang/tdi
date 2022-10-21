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

#include <tdi/common/tdi_utils.hpp>

#include "tdi_dummy_info.hpp"
#include "tdi_dummy_init.hpp"

namespace tdi {
namespace tna {
namespace dummy {

Device::Device(const tdi_dev_id_t &device_id,
               const tdi_arch_type_e &arch_type,
               const std::vector<tdi::ProgramConfig> &device_config,
               void * /*target_options*/,
               void *cookie)
    : tdi::tna::Device(
          device_id, arch_type, device_config, cookie) {
  // Parse tdi json for every program
  for (const auto &program_config : device_config) {
    if (tdi_info_map_.find(program_config.prog_name_) != tdi_info_map_.end()) {
      LOG_ERROR("%s:%d Program for %s already exists",
                __func__,
                __LINE__,
                program_config.prog_name_.c_str());
      continue;
    }
    auto tdi_info_mapper = std::unique_ptr<tdi::TdiInfoMapper>(
        new tdi::tna::dummy::TdiInfoMapper());
    auto table_factory =
        std::unique_ptr<tdi::TableFactory>(new tdi::tna::dummy::TableFactory());

    auto tdi_info_parser = std::unique_ptr<TdiInfoParser>(
        new TdiInfoParser(std::move(tdi_info_mapper)));
    tdi_info_parser->parseTdiInfo(program_config.tdi_info_file_paths_);
    auto tdi_info = tdi::TdiInfo::makeTdiInfo(program_config.prog_name_,
                                              std::move(tdi_info_parser),
                                              table_factory.get());
    tdi_info_map_[program_config.prog_name_] = std::move(tdi_info);
  }
}

tdi_status_t Init::tdiModuleInit(void *target_options) {
  auto &dev_mgr_obj = DevMgr::getInstance();
  LOG_DBG("%s:%d TDI Device Add called", __func__, __LINE__);
  tdi::ProgramConfig program_cfg("dummy_tna", {}, {});

  return dev_mgr_obj.deviceAdd<tdi::tna::dummy::Device>(
      0, TDI_ARCH_TYPE_TNA, {program_cfg}, target_options, nullptr);
}

}  // namespace dummy
}  // namespace tna
}  // namespace tdi
