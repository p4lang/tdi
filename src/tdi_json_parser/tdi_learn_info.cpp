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

#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>

#include <tdi/common/tdi_json_parser/tdi_learn_info.hpp>

#include <tdi/common/tdi_utils.hpp>
#include <tdi/common/tdi_json_parser/tdi_cjson.hpp>

namespace tdi {

tdi_status_t LearnInfo::learnNameGet(std::string *name) const {
  if (name == nullptr) {
    LOG_ERROR("%s:%d %s ERROR Outparam passed null",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  *name = name_;
  return TDI_SUCCESS;
}

tdi_status_t LearnInfo::learnIdGet(tdi_id_t *id) const {
  if (id == nullptr) {
    LOG_ERROR("%s:%d %s ERROR Outparam passed null",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  *id = id_;
  return TDI_SUCCESS;
}

tdi_status_t LearnInfo::learnAnnotationsGet(AnnotationSet *annotations) const {
  for (const auto &annotation : this->annotations_) {
    (*annotations).insert(std::cref(annotation));
  }
  return TDI_SUCCESS;
}
tdi_status_t LearnInfo::dataFieldIdListGet(
    std::vector<tdi_id_t> *id_vec) const {
  if (id_vec == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  for (const auto &kv : learn_data_map_) {
    id_vec->push_back(kv.first);
  }
  std::sort(id_vec->begin(), id_vec->end());
  return TDI_SUCCESS;
}

tdi_status_t LearnInfo::dataFieldIdGet(const std::string &name,
                                       tdi_id_t *field_id) const {
  if (field_id == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  auto found = std::find_if(
      learn_data_map_.begin(),
      learn_data_map_.end(),
      [&name](const std::pair<const tdi_id_t, std::unique_ptr<DataFieldInfo>>
                  &map_item) { return (name == map_item.second->nameGet()); });
  if (found != learn_data_map_.end()) {
    *field_id = (*found).second->idGet();
    return TDI_SUCCESS;
  }

  LOG_ERROR("%s:%d %s Field \"%s\" not found in data field list",
            __func__,
            __LINE__,
            nameGet().c_str(),
            name.c_str());
  return TDI_OBJECT_NOT_FOUND;
}

tdi_status_t LearnInfo::dataFieldGet(
    const tdi_id_t &field_id, const DataFieldInfo **data_field_info) const {
  if (data_field_info == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  if (learn_data_map_.find(field_id) == learn_data_map_.end()) {
    LOG_ERROR("%s:%d %s Field \"%d\" not found in data field list",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return TDI_INVALID_ARG;
  }
  *data_field_info = learn_data_map_.at(field_id).get();
  return TDI_SUCCESS;
}

}  // namespace tdi
