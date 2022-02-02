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

// Forward declarations
class Learn;
class Table;

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
