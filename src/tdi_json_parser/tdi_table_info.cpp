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

#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>

#include <tdi/common/tdi_utils.hpp>
#include <tdi/common/tdi_json_parser/tdi_cjson.hpp>

namespace tdi {

bool Annotation::operator<(const Annotation &other) const {
  return (this->full_name_ < other.full_name_);
}
bool Annotation::operator==(const Annotation &other) const {
  return (this->full_name_ == other.full_name_);
}
bool Annotation::operator==(const std::string &other_str) const {
  return (this->full_name_ == other_str);
}
tdi_status_t Annotation::fullNameGet(std::string *full_name) const {
  *full_name = full_name_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableNameGet(std::string *name) const {
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

tdi_status_t TableInfo::tableIdGet(tdi_id_t *id) const {
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

tdi_status_t TableInfo::tableTypeGet(tdi_table_type_e *table_type) const {
  if (nullptr == table_type) {
    LOG_ERROR("%s:%d Outparam passed is nullptr", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  *table_type = table_type_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableSizeGet(size_t *size) const {
  if (nullptr == size) {
    LOG_ERROR("%s:%d Outparam passed is nullptr", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  *size = size_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableHasConstDefaultAction(
    bool *has_const_default_action) const {
  *has_const_default_action = this->has_const_default_action_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableIsConst(bool *is_const) const {
  *is_const = this->is_const_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableAnnotationsGet(AnnotationSet *annotations) const {
  for (const auto &annotation : this->annotations_) {
    (*annotations).insert(std::cref(annotation));
  }
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableApiSupportedGet(TableApiSet *tableApis) const {
  for (const auto &api : this->table_apis_) {
    tableApis->insert(std::cref(api));
  }
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableAttributesSupported(
    std::set<tdi_attributes_type_e> *type_set) const {
  *type_set = attributes_type_set_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableOperationsSupported(
    std::set<tdi_operations_type_e> *op_type_set) const {
  *op_type_set = operations_type_set_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::keyFieldIdListGet(std::vector<tdi_id_t> *id_vec) const {
  if (id_vec == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  for (const auto &kv : table_key_map_) {
    id_vec->push_back(kv.first);
  }
  std::sort(id_vec->begin(), id_vec->end());
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::keyFieldIdGet(const std::string &name,
                                      tdi_id_t *field_id) const {
  if (field_id == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  auto found = std::find_if(
      table_key_map_.begin(),
      table_key_map_.end(),
      [&name](const std::pair<const tdi_id_t, std::unique_ptr<KeyFieldInfo>>
                  &map_item) { return (name == map_item.second->nameGet()); });
  if (found != table_key_map_.end()) {
    *field_id = (*found).second->idGet();
    return TDI_SUCCESS;
  }

  LOG_ERROR("%s:%d %s Field \"%s\" not found in key field list",
            __func__,
            __LINE__,
            nameGet().c_str(),
            name.c_str());
  return TDI_OBJECT_NOT_FOUND;
}

tdi_status_t TableInfo::keyFieldGet(const tdi_id_t &field_id,
                                    const KeyFieldInfo **key_field_info) const {
  if (key_field_info == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  if (table_key_map_.find(field_id) == table_key_map_.end()) {
    LOG_ERROR("%s:%d %s Field \"%d\" not found in key field list",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return TDI_INVALID_ARG;
  }
  *key_field_info = table_key_map_.at(field_id).get();
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::dataFieldIdListGet(
    const tdi_id_t &action_id, std::vector<tdi_id_t> *id_vec) const {
  if (id_vec == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  if (action_id) {
    if (table_action_map_.find(action_id) == table_action_map_.end()) {
      LOG_ERROR("%s:%d %s Action Id %d Not Found",
                __func__,
                __LINE__,
                nameGet().c_str(),
                action_id);
      return TDI_OBJECT_NOT_FOUND;
    }
    for (const auto &kv : table_action_map_.at(action_id)->data_fields_) {
      id_vec->push_back(kv.first);
    }
  }
  // Also include common data fields
  for (const auto &kv : table_data_map_) {
    id_vec->push_back(kv.first);
  }
  std::sort(id_vec->begin(), id_vec->end());
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::dataFieldIdListGet(
    std::vector<tdi_id_t> *id_vec) const {
  return this->dataFieldIdListGet(0, id_vec);
}
#if 0

tdi_status_t TableInfo::dataFieldIdGet(const std::string &name,
                                       tdi_id_t *field_id) const {
  return this->dataFieldIdGet(name, 0, field_id);
}

tdi_status_t TableInfo::dataFieldIdGet(const std::string &name,
                                       const tdi_id_t &action_id,
                                       tdi_id_t *field_id) const {
  if (field_id == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const DataFieldInfo *field;
  std::vector<tdi_id_t> empty;
  auto status = getDataField(name, action_id, empty, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field name %s Action ID %d not found",
              __func__,
              __LINE__,
              name.c_str(),
              action_id);
    return status;
  }
  *field_id = field->idGet();
  return TDI_SUCCESS;
}

tdi_status_t KeyFieldInfo::keyFieldTypeGet(KeyFieldType *field_type) const {
  if (field_type == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  *field_type = key_fields.at(field_id)->getType();
  return TDI_SUCCESS;
}

tdi_status_t KeyFieldInfo::keyFieldDataTypeGet(const tdi_id_t &field_id,
                                               DataType *data_type) const {
  if (data_type == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  if (key_fields.find(field_id) == key_fields.end()) {
    LOG_ERROR("%s:%d %s Field ID %d not found",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return TDI_OBJECT_NOT_FOUND;
  }
  *data_type = key_fields.at(field_id)->getDataType();
  return TDI_SUCCESS;
}

tdi_status_t Table::keyFieldSizeGet(const tdi_id_t &field_id,
                                    size_t *size) const {
  if (size == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  if (key_fields.find(field_id) == key_fields.end()) {
    LOG_ERROR("%s:%d %s Field ID %d not found",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return TDI_OBJECT_NOT_FOUND;
  }
  *size = key_fields.at(field_id)->getSize();
  return TDI_SUCCESS;
}

tdi_status_t Table::keyFieldIsPtrGet(const tdi_id_t &field_id,
                                     bool *is_ptr) const {
  if (is_ptr == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  if (key_fields.find(field_id) == key_fields.end()) {
    LOG_ERROR("%s:%d %s Field ID %d not found",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return TDI_OBJECT_NOT_FOUND;
  }
  *is_ptr = key_fields.at(field_id)->isPtr();
  return TDI_SUCCESS;
}

tdi_status_t Table::keyFieldNameGet(const tdi_id_t &field_id,
                                    std::string *name) const {
  if (key_fields.find(field_id) == key_fields.end()) {
    LOG_ERROR("%s:%d %s Field ID %d not found",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return TDI_OBJECT_NOT_FOUND;
  }
  *name = key_fields.at(field_id)->getName();
  return TDI_SUCCESS;
}

tdi_status_t Table::keyFieldAllowedChoicesGet(
    const tdi_id_t &field_id,
    std::vector<std::reference_wrapper<const std::string>> *choices) const {
  DataType data_type;
  tdi_status_t sts = this->keyFieldDataTypeGet(field_id, &data_type);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  if (data_type != DataType::STRING) {
    LOG_ERROR("%s:%d %s This API is valid only for fields of type STRING",
              __func__,
              __LINE__,
              this->nameGet().c_str());
    return TDI_INVALID_ARG;
  }
  choices->clear();
  for (const auto &iter : this->key_fields.at(field_id)->getEnumChoices()) {
    choices->push_back(std::cref(iter));
  }
  return TDI_SUCCESS;
}

uint32_t Table::dataFieldListSize() const { return this->dataFieldListSize(0); }

uint32_t Table::dataFieldListSize(const tdi_id_t &action_id) const {
  if (action_info_list.find(action_id) != action_info_list.end()) {
    return action_info_list.at(action_id)->data_fields.size() +
           common_data_fields.size();
  }
  return common_data_fields.size();
}

tdi_status_t Table::dataFieldSizeGet(const tdi_id_t &field_id,
                                     size_t *size) const {
  return this->dataFieldSizeGet(field_id, 0, size);
}

tdi_status_t Table::dataFieldSizeGet(const tdi_id_t &field_id,
                                     const tdi_id_t &action_id,
                                     size_t *size) const {
  if (size == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  if (field->isContainerValid()) {
    *size = field->containerSizeGet();
  } else {
    *size = field->getSize();
  }
  return TDI_SUCCESS;
}

tdi_status_t Table::dataFieldIsPtrGet(const tdi_id_t &field_id,
                                      bool *is_ptr) const {
  return this->dataFieldIsPtrGet(field_id, 0, is_ptr);
}

tdi_status_t Table::dataFieldIsPtrGet(const tdi_id_t &field_id,
                                      const tdi_id_t &action_id,
                                      bool *is_ptr) const {
  if (is_ptr == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  *is_ptr = field->isPtr();
  return TDI_SUCCESS;
}

tdi_status_t Table::dataFieldMandatoryGet(const tdi_id_t &field_id,
                                          bool *is_mandatory) const {
  return this->dataFieldMandatoryGet(field_id, 0, is_mandatory);
}

tdi_status_t Table::dataFieldMandatoryGet(const tdi_id_t &field_id,
                                          const tdi_id_t &action_id,
                                          bool *is_mandatory) const {
  if (is_mandatory == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  *is_mandatory = field->isMandatory();
  return TDI_SUCCESS;
}

tdi_status_t Table::dataFieldReadOnlyGet(const tdi_id_t &field_id,
                                         bool *is_read_only) const {
  return this->dataFieldReadOnlyGet(field_id, 0, is_read_only);
}

tdi_status_t Table::dataFieldReadOnlyGet(const tdi_id_t &field_id,
                                         const tdi_id_t &action_id,
                                         bool *is_read_only) const {
  if (is_read_only == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  *is_read_only = field->isReadOnly();
  return TDI_SUCCESS;
}

tdi_status_t Table::dataFieldOneofSiblingsGet(
    const tdi_id_t &field_id, std::set<tdi_id_t> *oneof_siblings) const {
  return this->dataFieldOneofSiblingsGet(field_id, 0, oneof_siblings);
}

tdi_status_t Table::dataFieldOneofSiblingsGet(
    const tdi_id_t &field_id,
    const tdi_id_t &action_id,
    std::set<tdi_id_t> *oneof_siblings) const {
  if (oneof_siblings == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  *oneof_siblings = field->oneofSiblings();
  return TDI_SUCCESS;
}

tdi_status_t Table::dataFieldNameGet(const tdi_id_t &field_id,
                                     std::string *name) const {
  return this->dataFieldNameGet(field_id, 0, name);
}

tdi_status_t Table::dataFieldNameGet(const tdi_id_t &field_id,
                                     const tdi_id_t &action_id,
                                     std::string *name) const {
  if (name == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  *name = field->getName();
  return TDI_SUCCESS;
}

tdi_status_t Table::dataFieldDataTypeGet(const tdi_id_t &field_id,
                                         DataType *type) const {
  return this->dataFieldDataTypeGet(field_id, 0, type);
}

tdi_status_t Table::dataFieldDataTypeGet(const tdi_id_t &field_id,
                                         const tdi_id_t &action_id,
                                         DataType *type) const {
  if (type == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  *type = field->getDataType();
  return TDI_SUCCESS;
}

tdi_status_t Table::dataFieldAllowedChoicesGet(
    const tdi_id_t &field_id,
    std::vector<std::reference_wrapper<const std::string>> *choices) const {
  return dataFieldAllowedChoicesGet(field_id, 0, choices);
}

tdi_status_t Table::dataFieldAllowedChoicesGet(
    const tdi_id_t &field_id,
    const tdi_id_t &action_id,
    std::vector<std::reference_wrapper<const std::string>> *choices) const {
  if (choices == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  if (field->getDataType() != DataType::STRING &&
      field->getDataType() != DataType::STRING_ARR) {
    LOG_ERROR("%s:%d %s API valid only for fields of type STRING field ID:%d",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return TDI_INVALID_ARG;
  }
  choices->clear();
  for (const auto &iter : field->getEnumChoices()) {
    choices->push_back(std::cref(iter));
  }
  return TDI_SUCCESS;
}

tdi_status_t Table::dataFieldAnnotationsGet(const tdi_id_t &field_id,
                                            AnnotationSet *annotations) const {
  return this->dataFieldAnnotationsGet(field_id, 0, annotations);
}

tdi_status_t Table::dataFieldAnnotationsGet(const tdi_id_t &field_id,
                                            const tdi_id_t &action_id,
                                            AnnotationSet *annotations) const {
  if (annotations == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  for (const auto &annotation : field->getAnnotations()) {
    (*annotations).insert(std::cref(annotation));
  }
  return TDI_SUCCESS;
}

tdi_status_t Table::defaultDataValueGet(const tdi_id_t &field_id,
                                        const tdi_id_t action_id,
                                        uint64_t *default_value) const {
  if (default_value == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  *default_value = field->defaultValueGet();
  return TDI_SUCCESS;
}

tdi_status_t Table::defaultDataValueGet(const tdi_id_t &field_id,
                                        const tdi_id_t action_id,
                                        float *default_value) const {
  if (default_value == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *tableDataField = nullptr;
  tdi_status_t status = TDI_SUCCESS;
  status = getDataField(field_id, action_id, &tableDataField);

  if (status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR : Field id %d not found for action id %d",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id,
              action_id);
    return status;
  }

  *default_value = tableDataField->defaultFlValueGet();

  return TDI_SUCCESS;
}

tdi_status_t Table::defaultDataValueGet(const tdi_id_t &field_id,
                                        const tdi_id_t action_id,
                                        std::string *default_value) const {
  if (default_value == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *tableDataField = nullptr;
  tdi_status_t status = TDI_SUCCESS;
  status = getDataField(field_id, action_id, &tableDataField);

  if (status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR : Field id %d not found for action id %d",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id,
              action_id);
    return status;
  }

  *default_value = tableDataField->defaultStrValueGet();

  return TDI_SUCCESS;
}

tdi_status_t Table::actionIdGet(const std::string &name,
                                tdi_id_t *action_id) const {
  if (action_id == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!this->actionIdApplicable()) {
    LOG_TRACE("%s:%d Not supported", __func__, __LINE__);
    return TDI_NOT_SUPPORTED;
  }
  auto action_found = std::find_if(
      action_info_list.begin(),
      action_info_list.end(),
      [&name](const std::pair<const tdi_id_t,
                              std::unique_ptr<tdi_info_action_info_t>>
                  &action_map_pair) {
        return action_map_pair.second->name == name;
      });
  if (action_found != action_info_list.end()) {
    *action_id = (*action_found).second->action_id;
    return TDI_SUCCESS;
  }
  LOG_TRACE("%s:%d Action_ID for action %s not found",
            __func__,
            __LINE__,
            name.c_str());
  return TDI_OBJECT_NOT_FOUND;
}

bool Table::actionIdApplicable() const { return false; }

tdi_status_t Table::actionNameGet(const tdi_id_t &action_id,
                                  std::string *name) const {
  if (!this->actionIdApplicable()) {
    LOG_TRACE("%s:%d Not supported", __func__, __LINE__);
    return TDI_NOT_SUPPORTED;
  }
  if (action_info_list.find(action_id) == action_info_list.end()) {
    LOG_TRACE("%s:%d Action Id %d Not Found", __func__, __LINE__, action_id);
    return TDI_OBJECT_NOT_FOUND;
  }
  *name = action_info_list.at(action_id)->name;
  return TDI_SUCCESS;
}

tdi_status_t Table::actionIdListGet(std::vector<tdi_id_t> *id_vec) const {
  if (!this->actionIdApplicable()) {
    LOG_TRACE("%s:%d Not supported", __func__, __LINE__);
    return TDI_NOT_SUPPORTED;
  }
  if (id_vec == nullptr) {
    LOG_TRACE("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  if (action_info_list.empty()) {
    LOG_TRACE("%s:%d Table has no action IDs", __func__, __LINE__);
    return TDI_OBJECT_NOT_FOUND;
  }
  for (const auto &kv : action_info_list) {
    id_vec->push_back(kv.first);
  }
  return TDI_SUCCESS;
}

tdi_status_t Table::actionAnnotationsGet(const tdi_id_t &action_id,
                                         AnnotationSet *annotations) const {
  // For the table to support actions, it needs to only override
  // actionIdApplicable() and all these action functions will remain
  // common
  if (!this->actionIdApplicable()) {
    LOG_TRACE("%s:%d Not supported", __func__, __LINE__);
    return TDI_NOT_SUPPORTED;
  }
  if (action_info_list.find(action_id) == action_info_list.end()) {
    LOG_TRACE("%s:%d Action Id %d Not Found", __func__, __LINE__, action_id);
    return TDI_OBJECT_NOT_FOUND;
  }
  for (const auto &annotation :
       this->action_info_list.at(action_id)->annotations) {
    (*annotations).insert(std::cref(annotation));
  }
  return TDI_SUCCESS;
}

tdi_status_t Table::tableOperationsExecute(
    const TableOperations &table_ops) const {
  auto table_ops_impl = static_cast<const TableOperationsImpl *>(&table_ops);
  if (table_ops_impl->tableGet()->table_id_get() != table_id_get()) {
    LOG_ERROR("%s:%d %s Table mismatch. Sent in %s expected %s",
              __func__,
              __LINE__,
              nameGet().c_str(),
              table_ops_impl->tableGet()->nameGet().c_str(),
              nameGet().c_str());
  }
  switch (table_ops_impl->getAllowedOp()) {
    case TableOperationsType::REGISTER_SYNC:
      return table_ops_impl->registerSyncExecute();
    case TableOperationsType::COUNTER_SYNC:
      return table_ops_impl->counterSyncExecute();
    case TableOperationsType::HIT_STATUS_UPDATE:
      return table_ops_impl->hitStateUpdateExecute();
    default:
      LOG_ERROR("%s:%d %s Table operation is not allowed",
                __func__,
                __LINE__,
                nameGet().c_str());
      return TDI_INVALID_ARG;
  }
  return TDI_NOT_SUPPORTED;
}

tdi_status_t Table::dataFieldTypeGet(const tdi_id_t &field_id,
                                     std::set<DataFieldType> *ret_type) const {
  return this->dataFieldTypeGet(field_id, 0, ret_type);
}

tdi_status_t Table::dataFieldTypeGet(const tdi_id_t &field_id,
                                     const tdi_id_t &action_id,
                                     std::set<DataFieldType> *ret_type) const {
  if (ret_type == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return status;
  }
  *ret_type = field->getTypes();
  return TDI_SUCCESS;
}

tdi_status_t Table::getKeyField(const tdi_id_t &field_id,
                                const TableKeyField **field) const {
  if (key_fields.find(field_id) == key_fields.end()) {
    LOG_ERROR("%s:%d Field ID %d not found", __func__, __LINE__, field_id);
    return TDI_OBJECT_NOT_FOUND;
  }
  *field = key_fields.at(field_id).get();
  return TDI_SUCCESS;
}

tdi_status_t Table::getDataField(const tdi_id_t &field_id,
                                 const TableDataField **field) const {
  return this->getDataField(field_id, 0, field);
}

tdi_status_t Table::getDataField(const tdi_id_t &field_id,
                                 const tdi_id_t &action_id,
                                 const TableDataField **field) const {
  std::vector<tdi_id_t> empty;
  return this->getDataField(field_id, action_id, empty, field);
}

tdi_status_t Table::getDataField(const tdi_id_t &field_id,
                                 const tdi_id_t &action_id,
                                 const std::vector<tdi_id_t> &container_id_vec,
                                 const TableDataField **field) const {
  *field = nullptr;
  if (action_info_list.find(action_id) != action_info_list.end()) {
    *field =
        this->getDataFieldHelper(field_id,
                                 container_id_vec,
                                 0,
                                 action_info_list.at(action_id)->data_fields);
    if (*field) return TDI_SUCCESS;
  }
  // We need to search the common data too
  *field = this->getDataFieldHelper(
      field_id, container_id_vec, 0, common_data_fields);
  if (*field) return TDI_SUCCESS;
  // Logging only warning so as to not overwhelm logs. Users supposed to check
  // error code if something wrong
  LOG_WARN("%s:%d Data field ID %d actionID %d not found",
           __func__,
           __LINE__,
           field_id,
           action_id);
  return TDI_OBJECT_NOT_FOUND;
}

const TableDataField *Table::getDataFieldHelper(
    const tdi_id_t &field_id,
    const std::vector<tdi_id_t> &container_id_vec,
    const uint32_t depth,
    const std::map<tdi_id_t, std::unique_ptr<TableDataField>> &field_map)
    const {
  if (field_map.find(field_id) != field_map.end()) {
    return field_map.at(field_id).get();
  }
  // iterate all fields and recursively search for the data field
  // if this field is a container field and it exists in the contaienr
  // set provided by caller.
  // If container ID vector is empty, then just try and find first
  // instance
  for (const auto &p : field_map) {
    if (((container_id_vec.size() >= depth + 1 &&
          container_id_vec[depth] == p.first) ||
         container_id_vec.empty()) &&
        p.second.get()->isContainerValid()) {
      auto field = this->getDataFieldHelper(field_id,
                                            container_id_vec,
                                            depth + 1,
                                            p.second.get()->containerMapGet());
      if (field) return field;
    }
  }
  return nullptr;
}

tdi_status_t Table::getDataField(const std::string &field_name,
                                 const tdi_id_t &action_id,
                                 const std::vector<tdi_id_t> &container_id_vec,
                                 const TableDataField **field) const {
  *field = nullptr;
  if (action_info_list.find(action_id) != action_info_list.end()) {
    *field = this->getDataFieldHelper(
        field_name,
        container_id_vec,
        0,
        action_info_list.at(action_id)->data_fields_names);
    if (*field) return TDI_SUCCESS;
  }
  // We need to search the common data too
  *field = this->getDataFieldHelper(
      field_name, container_id_vec, 0, common_data_fields_names);
  if (*field) return TDI_SUCCESS;
  // Logging only warning so as to not overwhelm logs. Users supposed to check
  // error code if something wrong
  // TODO change this to WARN
  LOG_TRACE("%s:%d Data field name %s actionID %d not found",
            __func__,
            __LINE__,
            field_name.c_str(),
            action_id);
  return TDI_OBJECT_NOT_FOUND;
}

const TableDataField *Table::getDataFieldHelper(
    const std::string &field_name,
    const std::vector<tdi_id_t> &container_id_vec,
    const uint32_t depth,
    const std::map<std::string, TableDataField *> &field_map) const {
  if (field_map.find(field_name) != field_map.end()) {
    return field_map.at(field_name);
  }
  // iterate all fields and recursively search for the data field
  // if this field is a container field and it exists in the contaienr
  // vector provided by caller.
  // If container ID vector is empty, then just try and find first
  // instance
  for (const auto &p : field_map) {
    if (((container_id_vec.size() >= depth + 1 &&
          container_id_vec[depth] == p.second->idGet()) ||
         container_id_vec.empty()) &&
        p.second->isContainerValid()) {
      auto field = this->getDataFieldHelper(field_name,
                                            container_id_vec,
                                            depth + 1,
                                            p.second->containerNameMapGet());
      if (field) return field;
    }
  }
  return nullptr;
}

void Table::addDataFieldType(const tdi_id_t &field_id,
                             const DataFieldType &type) {
  // This method adds a dataField Type to the given field id
  auto dataField = common_data_fields.find(field_id);
  if (dataField == common_data_fields.end()) {
    LOG_ERROR("%s:%d %s Field ID %d not found",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    TDI_ASSERT(0);
  }
  TableDataField *field = dataField->second.get();
  field->addDataFieldType(type);
  return;
}

tdi_id_t Table::table_id_get() const {
  tdi_id_t id;
  auto tdi_status = tableIdGet(&id);
  if (tdi_status == TDI_SUCCESS) {
    return id;
  }
  return 0;
}
const std::string &Table::key_field_name_get(const tdi_id_t &field_id) const {
  if (key_fields.find(field_id) == key_fields.end()) {
    LOG_ERROR("%s:%d %s Field ID %d not found",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return tdiNullStr;
  }
  return key_fields.at(field_id)->getName();
}

const std::string &Table::data_field_name_get(const tdi_id_t &field_id) const {
  return this->data_field_name_get(field_id, 0);
}

const std::string &Table::data_field_name_get(const tdi_id_t &field_id,
                                              const tdi_id_t &action_id) const {
  const TableDataField *field;
  auto status = getDataField(field_id, action_id, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d Action ID %d not found",
              __func__,
              __LINE__,
              field_id,
              action_id);
    return tdiNullStr;
  }
  return field->getName();
}

const std::string &Table::action_name_get(const tdi_id_t &action_id) const {
  if (action_info_list.find(action_id) == action_info_list.end()) {
    LOG_ERROR("%s:%d %s Action Id %d Not Found",
              __func__,
              __LINE__,
              nameGet().c_str(),
              action_id);
    return tdiNullStr;
  }
  return action_info_list.at(action_id)->name;
}

bool Table::validateTable_from_keyObj(const TableKeyObj &key) const {
  tdi_id_t table_id;
  tdi_id_t table_id_of_key_obj;

  const Table *table_from_key = nullptr;
  tdi_status_t tdi_status = this->tableIdGet(&table_id);
  if (tdi_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id, err %d",
              __func__,
              __LINE__,
              this->nameGet().c_str(),
              tdi_status);
    TDI_DBGCHK(0);
    return false;
  }

  tdi_status = key.tableGet(&table_from_key);
  if (tdi_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table object from key object, err %d",
              __func__,
              __LINE__,
              this->nameGet().c_str(),
              tdi_status);
    TDI_DBGCHK(0);
    return false;
  }

  tdi_status = table_from_key->tableIdGet(&table_id_of_key_obj);
  if (tdi_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id from key object, err %d",
              __func__,
              __LINE__,
              this->nameGet().c_str(),
              tdi_status);
    TDI_DBGCHK(0);
    return false;
  }

  if (table_id == table_id_of_key_obj) {
    return true;
  }
  return false;
}

bool Table::validateTable_from_dataObj(const TableDataObj &data) const {
  tdi_id_t table_id;
  tdi_id_t table_id_of_data_obj;

  tdi_status_t tdi_status = this->tableIdGet(&table_id);
  if (tdi_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id, err %d",
              __func__,
              __LINE__,
              this->nameGet().c_str(),
              tdi_status);
    TDI_DBGCHK(0);
    return false;
  }

  const Table *table_from_data = nullptr;
  tdi_status = data.getParent(&table_from_data);
  if (tdi_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table object from data object, err %d",
              __func__,
              __LINE__,
              this->nameGet().c_str(),
              tdi_status);
    TDI_DBGCHK(0);
    return false;
  }

  tdi_status = table_from_data->tableIdGet(&table_id_of_data_obj);
  if (tdi_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id from data object, err %d",
              __func__,
              __LINE__,
              this->nameGet().c_str(),
              tdi_status);
    TDI_DBGCHK(0);
    return false;
  }

  if (table_id == table_id_of_data_obj) {
    return true;
  }
  return false;
}

tdi_status_t Table::getKeyStringChoices(
    const std::string &key_str, std::vector<std::string> &choices) const {
  tdi_id_t key_id;
  auto status = this->keyFieldIdGet(key_str, &key_id);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d %s Error in finding fieldId for %s",
              __func__,
              __LINE__,
              nameGet().c_str(),
              key_str.c_str());
    return status;
  }

  std::vector<std::reference_wrapper<const std::string>> key_str_choices;
  status = this->keyFieldAllowedChoicesGet(key_id, &key_str_choices);
  if (status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s Failed to get string choices for key field Id %d",
              __func__,
              __LINE__,
              nameGet().c_str(),
              key_id);
    return status;
  }

  for (auto key_str_choice : key_str_choices) {
    choices.push_back(key_str_choice.get());
  }

  return status;
}

tdi_status_t Table::getDataStringChoices(
    const std::string &data_str, std::vector<std::string> &choices) const {
  tdi_id_t data_id;
  auto status = this->dataFieldIdGet(data_str, &data_id);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d %s Error in finding fieldId for %s",
              __func__,
              __LINE__,
              nameGet().c_str(),
              data_str.c_str());
    return status;
  }

  std::vector<std::reference_wrapper<const std::string>> data_str_choices;
  status = this->dataFieldAllowedChoicesGet(data_id, &data_str_choices);
  if (status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s Failed to get string choices for data field Id %d",
              __func__,
              __LINE__,
              nameGet().c_str(),
              data_id);
    return status;
  }

  for (auto data_str_choice : data_str_choices) {
    choices.push_back(data_str_choice.get());
  }

  return status;
}

#endif

}  // namespace tdi
