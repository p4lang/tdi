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
/** @file tdi_learn_info.hpp
 *
 *  @brief Contains TDI Learn Info APIs
 */
#ifndef _TDI_LEARN_INFO_HPP
#define _TDI_LEARN_INFO_HPP

#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>

namespace tdi {

// Classes that need to be overridden by targets in order for them to
// target-specific information in the info

class LearnContextInfo {};

/**
 * @brief In memory representation of tdi.json Learn
 */
class LearnInfo {
 public:
  /**
   * @brief Get name of the learn
   *
   * @param[out] name Name of the learn
   *
   * @return Status of the API call
   */
  tdi_status_t learnNameGet(std::string *name) const;

  /**
   * @brief Get ID of the learn
   *
   * @param[out] id ID of the learn
   *
   * @return Status of the API call
   */
  tdi_status_t learnIdGet(tdi_id_t *id) const;

  /**
   * @brief Get a set of annotations on a Learn
   *
   * @param[out] annotations Set of annotations on a Learn
   *
   * @return Status of the API call
   */
  tdi_status_t learnAnnotationsGet(AnnotationSet *annotations) const;

  /**
   * @brief Get vector of DataField IDs. Only applicable for learns
   * without Action IDs
   *
   * @param[out] id Vector of IDs
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldIdListGet(std::vector<tdi_id_t> *id) const;

  /**
   * @brief Get the field ID of a Data Field from a name.
   *
   * @param[in] name Name of a Data field
   * @param[out] field_id Field ID
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldIdGet(const std::string &name,
                              tdi_id_t *field_id) const;

  /**
   * @brief Get the data Field info object from tdi_id.
   *
   * @param[in] id id of a Data field
   * @param[out] data_field_info DataFieldInfo object
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldGet(const tdi_id_t &field_id,
                            const DataFieldInfo **data_field_info) const;

  const std::string &nameGet() const { return name_; }
  void learnContextInfoSet(
      std::unique_ptr<LearnContextInfo> learn_context_info) {
    learn_context_info_ = std::move(learn_context_info);
  };

 private:
  LearnInfo(tdi_id_t id,
            std::string name,
            std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> learn_data_map,
            std::set<Annotation> annotations)
      : id_(id),
        name_(name),
        learn_data_map_(std::move(learn_data_map)),
        annotations_(annotations){};
  tdi_id_t id_;
  std::string name_;
  std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> learn_data_map_;
  std::set<Annotation> annotations_{};
  mutable std::unique_ptr<LearnContextInfo> learn_context_info_;
  friend class TdiInfoParser;
};

}  // namespace tdi

#endif  // _TDI_LEARN_HPP
