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
/** @file tna_info.hpp
 *
 *  @brief Contains TDI Info classes for TNA
 *  Object
 */
#ifndef _TNA_INFO_HPP
#define _TNA_INFO_HPP

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_info.hpp>

#include <tdi/arch/tna/tna_defs.h>

/**
 * @brief Namespace for TDI
 */
namespace tdi {

namespace tdi_json {
namespace values {
namespace tna {
const std::string ATCAM = "ATCAM";
const std::string RANGE = "LPM";
}  // namespace tna
}  // namespace values
}  // namespace tdi_json

namespace tna {


class TdiInfoTnaMapper : public TdiInfoMapper {
 public:

  TdiInfoTnaMapper() {
    // Match types
    matchEnumMapAdd(tdi_json::values::tna::ATCAM,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_TNA_ATCAM));
    matchEnumMapAdd(tdi_json::values::tna::RANGE,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_TNA_RANGE));
  }

};

/**
 * @brief Class to help create the correct Table object with the
 * help of a map. Targets should override
 */
class TableFactory {
 public:
  std::unique_ptr<tdi::Table> makeTable(
      const tdi::TableInfo * /*table_info*/) const {
    // No tables in TNA currently. Will eventually have Selector, Action profile
    // etc
    return nullptr;
  };
};

}  // namespace tna
}  // namespace tdi

#endif
