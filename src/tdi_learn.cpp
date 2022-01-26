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
#include <vector>
#include <cstring>
#include <algorithm>

#include <tdi/common/tdi_learn.hpp>
#include "tdi_utils.hpp"

namespace tdi {

tdi_status_t LearnInfo::learnIdGet(tdi_id_t *id) const {
  *id = learn_id_;
  return TDI_SUCCESS;
}

tdi_status_t LearnInfo::learnNameGet(std::string *name) const {
  *name = learn_name_;
  return TDI_SUCCESS;
}


tdi_status_t LearnInfo::learnFieldIdListGet(
    std::vector<tdi_id_t> *id) const {
  if (id == nullptr) {
    LOG_ERROR("%s:%d Please allocate memory for out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  for (auto const &item : lrn_fields_) {
    id->push_back(item.second->getFieldId());
  }
  return TDI_SUCCESS;
}

tdi_status_t LearnInfo::learnFieldIdGet(const std::string &name,
                                          tdi_id_t *field_id) const {
  if (field_id == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto found = std::find_if(
      lrn_fields_.begin(),
      lrn_fields_.end(),
      [&name](const std::pair<const tdi_id_t,
                              std::unique_ptr<const LearnFieldInfo>> &map_item) {
        return (name == map_item.second->getName());
      });
  if (found != lrn_fields_.end()) {
    *field_id = (*found).second->getFieldId();
    return TDI_SUCCESS;
  }
  LOG_ERROR("%s:%d Field \"%s\" not found", __func__, __LINE__, name.c_str());
  return TDI_OBJECT_NOT_FOUND;
}

tdi_status_t LearnFieldInfo::learnFieldSizeGet(size_t *s) const {
  *s = size_;
  return TDI_SUCCESS;
}

tdi_status_t LearnFieldInfo::learnFieldIsPtrGet(bool *is_ptr) const {
  *is_ptr = is_ptr_;
  return TDI_SUCCESS;
}

tdi_status_t LearnFieldInfo::learnFieldNameGet(std::string *name) const {
  *name = name_;
  return TDI_SUCCESS;
}

//from tdi_learn_impl.cpp
const LearnField *Learn::getLearnField(
    const tdi_id_t &field_id) const {
  if (lrn_fields.find(field_id) == lrn_fields.end()) {
    LOG_ERROR("%s:%d Field-Id %d not found", __func__, __LINE__, field_id);
    return nullptr;
  }
  return lrn_fields.at(field_id).get();
}

}  // namespace tdi
