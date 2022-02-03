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

#include <memory>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <vector>
#include <exception>
#include <regex>

#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

tdi_status_t TdiInfo::tablesGet(
    std::vector<const Table *> *table_vec_ret) const {
  if (table_vec_ret == nullptr) {
    LOG_ERROR("%s:%d nullptr arg passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  for (auto const &item : tableMap) {
    table_vec_ret->push_back(item.second.get());
  }
  return TDI_SUCCESS;
}

tdi_status_t TdiInfo::tableFromNameGet(
    const std::string &name, const Table **table_ret) const {
  if (invalid_table_names.find(name) != invalid_table_names.end()) {
    LOG_ERROR("%s:%d Table \"%s\" was optimized out",
              __func__,
              __LINE__,
              name.c_str());
    return TDI_INVALID_ARG;
  }
  auto it = this->fullTableMap.find(name);
  if (it == this->fullTableMap.end()) {
    LOG_ERROR("%s:%d Table \"%s\" not found", __func__, __LINE__, name.c_str());
    return TDI_OBJECT_NOT_FOUND;
  } else {
    *table_ret = it->second;
    return TDI_SUCCESS;
  }
}

tdi_status_t TdiInfo::tableFromIdGet(
    const tdi_id_t &id, const Table **table_ret) const {
  auto it = tableIdMap.find(id);
  if (it == tableIdMap.end()) {
    LOG_ERROR("%s:%d Table_id \"%d\" not found", __func__, __LINE__, id);
    return TDI_OBJECT_NOT_FOUND;
  } else {
    return tableFromNameGet(it->second, table_ret);
  }
}

tdi_status_t TdiInfo::learnsGet(
    std::vector<const Learn *> *learn_vec_ret) const {
  if (learn_vec_ret == nullptr) {
    LOG_ERROR("%s:%d nullptr arg passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  for (auto &item : learnMap) {
    learn_vec_ret->push_back(item.second.get());
  }
  return TDI_SUCCESS;
}

tdi_status_t TdiInfo::learnFromNameGet(
    std::string name, const Learn **learn_ret) const {
  if (this->fullLearnMap.find(name) == this->fullLearnMap.end()) {
    LOG_ERROR(
        "%s:%d Learn Obj \"%s\" not found", __func__, __LINE__, name.c_str());
    return TDI_OBJECT_NOT_FOUND;
  }
  *learn_ret = this->fullLearnMap.at(name);
  return TDI_SUCCESS;
}

tdi_status_t TdiInfo::learnFromIdGet(
    tdi_id_t id, const Learn **learn_ret) const {
  if (this->learnIdMap.find(id) == this->learnIdMap.end()) {
    LOG_ERROR("%s:%d Learn_id \"%d\" not found", __func__, __LINE__, id);
    return TDI_OBJECT_NOT_FOUND;
  }
  return this->learnFromNameGet(learnIdMap.at(id), learn_ret);
}

}  // namespace tdi
