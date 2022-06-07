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
const std::string TABLE_KEY_MATCH_TYPE_ATCAM = "ATCAM";
}  // namespace tna
}  // namespace values
}  // namespace tdi_json

namespace tna {

class TdiInfoMapper : public tdi::TdiInfoMapper {
 public:
  TdiInfoMapper() {
    // Match types
    // ATCAM match type treated as exact match
    matchEnumMapAdd(tdi_json::values::tna::TABLE_KEY_MATCH_TYPE_ATCAM,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_EXACT));
  }
};

/**
 * @brief Class to help create the correct Table object with the
 * help of a map. Targets should override
 */
class TableFactory : public tdi::TableFactory {
 public:
  virtual std::unique_ptr<tdi::Table> makeTable(
      const TdiInfo * /*tdi_info*/,
      const tdi::TableInfo * /*table_info*/) const override {
    // No tables in TNA currently. Will eventually have Selector, Action profile
    // etc
    return nullptr;
  };
};

}  // namespace tna
}  // namespace tdi

#endif
