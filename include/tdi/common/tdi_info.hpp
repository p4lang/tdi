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
/** @file tdi_info.hpp
 *
 *  @brief Contains TDI Info APIs. Mostly to get Table and Learn Object
 *metadata
 */
#ifndef _TDI_INFO_HPP
#define _TDI_INFO_HPP

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_learn.hpp>
#include <tdi/common/tdi_table.hpp>

/**
 * @brief Namespace for TDI
 */
namespace tdi {

namespace tdi_json {
namespace values {
namespace core {
const std::string TABLE_KEY_MATCH_TYPE_EXACT = "Exact";
const std::string TABLE_KEY_MATCH_TYPE_TERNARY = "Ternary";
const std::string TABLE_KEY_MATCH_TYPE_LPM = "LPM";
}  // namespace core
}  // namespace values
}  // namespace tdi_json

class TdiInfoMapper {
 public:
  const std::map<std::string, tdi_table_type_e> &tableEnumMapGet() {
    return table_e_map_;
  };
  const std::map<std::string, tdi_match_type_e> &matchEnumMapGet() {
    return match_e_map_;
  };
  const std::map<std::string, tdi_operations_type_e> &operationsEnumMapGet() {
    return operations_e_map_;
  };
  const std::map<std::string, tdi_attributes_type_e> &attributesEnumMapGet() {
    return attributes_e_map_;
  };

  TdiInfoMapper() {
    // Match types
    matchEnumMapAdd(tdi_json::values::core::TABLE_KEY_MATCH_TYPE_EXACT,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_EXACT));
    matchEnumMapAdd(tdi_json::values::core::TABLE_KEY_MATCH_TYPE_TERNARY,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_TERNARY));
    matchEnumMapAdd(tdi_json::values::core::TABLE_KEY_MATCH_TYPE_LPM,
                    static_cast<tdi_match_type_e>(TDI_MATCH_TYPE_LPM));
  }
  virtual ~TdiInfoMapper(){};

 protected:
  tdi_status_t tableEnumMapAdd(const std::string &str,
                               const tdi_table_type_e &type) {
    if (table_e_map_.find(str) != table_e_map_.end()) {
      return TDI_ALREADY_EXISTS;
    }
    table_e_map_[str] = type;
    return TDI_SUCCESS;
  }
  tdi_status_t matchEnumMapAdd(const std::string &str,
                               const tdi_match_type_e &type) {
    if (match_e_map_.find(str) != match_e_map_.end()) {
      return TDI_ALREADY_EXISTS;
    }
    match_e_map_[str] = type;
    return TDI_SUCCESS;
  }
  tdi_status_t operationsEnumMapAdd(const std::string &str,
                                    const tdi_operations_type_e &type) {
    if (operations_e_map_.find(str) != operations_e_map_.end()) {
      return TDI_ALREADY_EXISTS;
    }
    operations_e_map_[str] = type;
    return TDI_SUCCESS;
  }
  tdi_status_t attributesEnumMapAdd(const std::string &str,
                                    const tdi_attributes_type_e &type) {
    if (attributes_e_map_.find(str) != attributes_e_map_.end()) {
      return TDI_ALREADY_EXISTS;
    }
    attributes_e_map_[str] = type;
    return TDI_SUCCESS;
  }

 private:
  std::map<std::string, tdi_table_type_e> table_e_map_;
  std::map<std::string, tdi_match_type_e> match_e_map_;
  std::map<std::string, tdi_operations_type_e> operations_e_map_;
  std::map<std::string, tdi_attributes_type_e> attributes_e_map_;
};

/**
 * @brief Class to maintain metadata of all tables and learn objects. Note that
 *    all the objects returned are representations of the actual HW tables.\n So
 *    TdiInfo doesn't provide ownership of any of its internal structures.
 *    Furthermore, all the metadata is read-only hence only const pointers are
 *    returned.<br>
 * <B>Creation: </B> Cannot be created. Can only be retrived using \ref
 * tdi::Device::tdiInfoGet()
 */
class TdiInfo {
 public:
  /**
   * @brief Destructor destroys all metadata contained in this object.
   */
  virtual ~TdiInfo() = default;

  /**
   * @brief Get all the tdi::Table objs.
   *
   * @param[out] table_vec_ret Vector of tdi::Table obj pointers
   *
   * @return Status of the API call
   */
  tdi_status_t tablesGet(std::vector<const tdi::Table *> *table_vec_ret) const;
  /**
   * @brief Get a tdi::Table obj from its fully qualified name
   *
   * @param[in] name Fully qualified P4 table name
   * @param[out] table_ret tdi::Table obj pointer
   *
   * @return Status of the API call
   */
  tdi_status_t tableFromNameGet(const std::string &name,
                                const tdi::Table **table_ret) const;
  /**
   * @brief Get a tdi::Table obj from its ID
   *
   * @param[in] id ID of the tdi::Table obj
   * @param[out] table_ret tdi::Table obj pointer
   *
   * @return Status of the API call
   */
  tdi_status_t tableFromIdGet(const tdi_id_t &id,
                              const tdi::Table **table_ret) const;

  /**
   * @brief Get all the tdi::Learn Objs
   *
   * @param[out] learn_vec_ret Vector of tdi::Learn Obj pointers
   *
   * @return Status of the API call
   */
  tdi_status_t learnsGet(std::vector<const tdi::Learn *> *learn_vec_ret) const;
  /**
   * @brief Get a tdi::Learn Object from its fully qualified name
   *
   * @param[in] name Fully qualified P4 Learn Obj name
   * @param[out] learn_ret tdi::Learn Obj Pointer
   *
   * @return Status of the API call
   */
  tdi_status_t learnFromNameGet(std::string name,
                                const tdi::Learn **learn_ret) const;
  /**
   * @brief Get a tdi::Learn Obj from its ID
   *
   * @param[in] id ID of the tdi::Learn Obj
   * @param[out] learn_ret tdi::Learn Obj pointer
   *
   * @return Status of the API call
   */
  tdi_status_t learnFromIdGet(tdi_id_t id, const tdi::Learn **learn_ret) const;

  // from tdi_info_impl.hpp
  std::unique_ptr<const TdiInfo> static makeTdiInfo(
      const tdi_dev_id_t &dev_id, const ProgramConfig &program_config);
  tdi_status_t tdiInfoTablesDependentOnThisTableGet(
      const tdi_id_t &tbl_id,
      std::vector<tdi_id_t> *table_vec_ret) const;
  tdi_status_t tdiInfoTablesThisTableDependsOnGet(
      const tdi_id_t &tbl_id,
      std::vector<tdi_id_t> *table_vec_ret) const;

  // from the tdi_info_impl.hpp
  // Map to indicate what tables depend on a particular table
  std::map<tdi_id_t, std::vector<tdi_id_t>>
      tables_dependent_on_this_table_map;

 private:
  /* Main P4_info map. object_name --> tdi_info object */
  std::map<std::string, std::unique_ptr<tdi::Table>> tableMap;
  // This is the map which is to be queried when a name lookup for a table
  // happens. Multiple names can point to the same table because multiple
  // names can exist for a table. Example, switchingress.forward and forward
  // both are valid for a table if no conflicts with other table is present
  std::map<std::string, const tdi::Table *> fullTableMap;

  /* Reverse map in case lookup from ID is needed*/
  std::map<tdi_id_t, std::string> tableIdMap;

  // Learn Map
  std::map<std::string, std::unique_ptr<tdi::Learn>> learnMap;
  std::map<std::string, const tdi::Learn *> fullLearnMap;
  std::map<tdi_id_t, std::string> learnIdMap;

  // Set of optimized out table names. Tables that may be present
  // in TDI.json but Device decided not to have a table object present
  // for it at all.
  std::set<std::string> invalid_table_names;
};

}  // namespace tdi

#endif
