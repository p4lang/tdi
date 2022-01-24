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

#include "tdi_table_info.hpp"

namespace tdi {

namespace {

  tdi_field_data_type_e stringToDataType(const std::string &type,
                                         const bool &repeated) {
  if (type == "bytes") {
    return TDI_FIELD_DATA_TYPE_BYTE_STREAM;
  } else if (type == "uint64" || type == "uint32" || type == "uint16" ||
             type == "uint8") {
    if (!repeated)
      return TDI_FIELD_DATA_TYPE_UINT64;
    else
      return TDI_FIELD_DATA_TYPE_INT_ARR;
  } else if (type == "bool") {
    if (!repeated)
      return TDI_FIELD_DATA_TYPE_BOOL;
    else
      return TDI_FIELD_DATA_TYPE_BOOL_ARR;
  } else if (type == "float") {
    return TDI_FIELD_DATA_TYPE_FLOAT;
  } else if (type == "string") {
    if (!repeated)
      return TDI_FIELD_DATA_TYPE_STRING;
    else
      return TDI_FIELD_DATA_TYPE_STRING_ARR;
  }
  return TDI_FIELD_DATA_TYPE_UNKNOWN;
}
// This function returns if a key field is a field slice or not
bool checkIsFieldSlice(const tdi::Cjson &key_field) {
  tdi::Cjson key_annotations = key_field["annotations"];
  for (const auto &annotation : key_annotations.getCjsonChildVec()) {
    std::string annotation_name = (*annotation)["name"];
    std::string annotation_value = (*annotation)["value"];
    if ((annotation_name == "isFieldSlice") && (annotation_value == "true")) {
      return true;
    }
  }
  return false;
}


} // anonymous namespace

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
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  *name = table_name_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableIdGet(tdi_id_t *id) const {
  if (id == nullptr) {
    LOG_ERROR("%s:%d %s ERROR Outparam passed null",
              __func__,
              __LINE__,
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  *id = table_id_;
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
  *size = table_size_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableHasConstDefaultAction(
    bool *has_const_default_action) const {
  *has_const_default_action = this->has_const_default_action_;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableIsConst(bool *is_const) const {
  *is_const = this->is_const_table_;
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
              table_name_get().c_str());
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
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  auto found = std::find_if(
      table_key_map_.begin(),
      table_key_map_.end(),
      [&name](const std::pair<const tdi_id_t, std::unique_ptr<KeyFieldInfo>>
                  &map_item) { return (name == map_item.second->getName()); });
  if (found != table_key_map_.end()) {
    *field_id = (*found).second->getId();
    return TDI_SUCCESS;
  }

  LOG_ERROR("%s:%d %s Field \"%s\" not found in key field list",
            __func__,
            __LINE__,
            table_name_get().c_str(),
            name.c_str());
  return TDI_OBJECT_NOT_FOUND;
}

tdi_status_t TableInfo::keyFieldGet(const tdi_id_t &field_id,
                                    const KeyFieldInfo **key_field_info) const {
  if (key_field_info == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  if (table_key_map_.find(field_id) == table_key_map_.end()) {
    LOG_ERROR("%s:%d %s Field \"%d\" not found in key field list",
              __func__,
              __LINE__,
              table_name_get().c_str(),
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
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  if (action_id) {
    if (table_action_map_.find(action_id) == table_action_map_.end()) {
      LOG_ERROR("%s:%d %s Action Id %d Not Found",
                __func__,
                __LINE__,
                table_name_get().c_str(),
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
  *field_id = field->getId();
  return TDI_SUCCESS;
}

tdi_status_t KeyFieldInfo::keyFieldTypeGet(KeyFieldType *field_type) const {
  if (field_type == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              table_name_get().c_str());
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
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  if (key_fields.find(field_id) == key_fields.end()) {
    LOG_ERROR("%s:%d %s Field ID %d not found",
              __func__,
              __LINE__,
              table_name_get().c_str(),
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
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  if (key_fields.find(field_id) == key_fields.end()) {
    LOG_ERROR("%s:%d %s Field ID %d not found",
              __func__,
              __LINE__,
              table_name_get().c_str(),
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
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  if (key_fields.find(field_id) == key_fields.end()) {
    LOG_ERROR("%s:%d %s Field ID %d not found",
              __func__,
              __LINE__,
              table_name_get().c_str(),
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
              table_name_get().c_str(),
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
              this->table_name_get().c_str());
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
              table_name_get().c_str(),
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
              table_name_get().c_str(),
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
              table_name_get().c_str(),
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
              table_name_get().c_str(),
              table_ops_impl->tableGet()->table_name_get().c_str(),
              table_name_get().c_str());
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
                table_name_get().c_str());
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
          container_id_vec[depth] == p.second->getId()) ||
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
              table_name_get().c_str(),
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
              table_name_get().c_str(),
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
              table_name_get().c_str(),
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
              this->table_name_get().c_str(),
              tdi_status);
    TDI_DBGCHK(0);
    return false;
  }

  tdi_status = key.tableGet(&table_from_key);
  if (tdi_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table object from key object, err %d",
              __func__,
              __LINE__,
              this->table_name_get().c_str(),
              tdi_status);
    TDI_DBGCHK(0);
    return false;
  }

  tdi_status = table_from_key->tableIdGet(&table_id_of_key_obj);
  if (tdi_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id from key object, err %d",
              __func__,
              __LINE__,
              this->table_name_get().c_str(),
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
              this->table_name_get().c_str(),
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
              this->table_name_get().c_str(),
              tdi_status);
    TDI_DBGCHK(0);
    return false;
  }

  tdi_status = table_from_data->tableIdGet(&table_id_of_data_obj);
  if (tdi_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id from data object, err %d",
              __func__,
              __LINE__,
              this->table_name_get().c_str(),
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
              table_name_get().c_str(),
              key_str.c_str());
    return status;
  }

  std::vector<std::reference_wrapper<const std::string>> key_str_choices;
  status = this->keyFieldAllowedChoicesGet(key_id, &key_str_choices);
  if (status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s Failed to get string choices for key field Id %d",
              __func__,
              __LINE__,
              table_name_get().c_str(),
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
              table_name_get().c_str(),
              data_str.c_str());
    return status;
  }

  std::vector<std::reference_wrapper<const std::string>> data_str_choices;
  status = this->dataFieldAllowedChoicesGet(data_id, &data_str_choices);
  if (status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s Failed to get string choices for data field Id %d",
              __func__,
              __LINE__,
              table_name_get().c_str(),
              data_id);
    return status;
  }

  for (auto data_str_choice : data_str_choices) {
    choices.push_back(data_str_choice.get());
  }

  return status;
}

#endif


static const std::string match_key_priority_field_name = "$MATCH_PRIORITY";

typedef struct key_size_ {
  size_t bytes;
  size_t bits;
} key_size_t;

void TdiInfoParser::parseFieldWidth(const tdi::Cjson &node,
                                   tdi_field_data_type_e &type,
                                   size_t &width,
                                   uint64_t &default_value,
                                   float &default_fl_value,
                                   std::string &default_str_value,
                                   std::vector<std::string> &choices) {
  auto type_str = std::string(node["type"]["type"]);
  bool repeated = false;
  if (node["repeated"].exists()) {
    repeated = node["repeated"];
  }
  type = stringToDataType(std::string(node["type"]["type"]),
                          repeated);
  if (type_str == "bytes") {
    width = static_cast<unsigned int>(node["type"]["width"]);
  } else if (type_str == "uint64") {
    width = 64;
  } else if (type_str == "uint32") {
    width = 32;
  } else if (type_str == "uint16") {
    width = 16;
  } else if (type_str == "uint8") {
    width = 8;
  } else if (type_str == "bool") {
    width = 1;
  } else if (type_str == "float") {
    width = 0;
    // If default value is listed, populate it, else its zero
    if (node["type"]["default_value"].exists()) {
      default_fl_value = static_cast<float>(node["type"]["default_value"]);
    } else {
      default_fl_value = 0.0;
    }
  } else if (type_str == "string") {
    width = 0;
    for (const auto &choice : node["type"]["choices"].getCjsonChildVec()) {
      choices.push_back(static_cast<std::string>(*choice));
    }
    // If string default value is listed populate it, else its empty
    if (node["type"]["default_value"].exists()) {
      default_str_value =
          static_cast<std::string>(node["type"]["default_value"]);
    } else {
      default_str_value = std::string("INVALID");
    }
  } else if (node["container"].exists()) {
    width = 0;
    type = TDI_FIELD_DATA_TYPE_CONTAINER;
  } else {
    width = 0;
  }

  // If default value is listed, populate it, else its zero
  if (node["type"]["default_value"].exists()) {
    default_value = static_cast<uint64_t>(node["type"]["default_value"]);
  } else {
    default_value = 0;
  }
}

// This function is only applicable when the key field is a field slice
// This function strips out '[]' if present in the name of the key_field.
// This will be true when the key is a field slice. The compiler will
// publish the name of the key with '[]' when the user does not use '@name'
// p4 annotation for the field slice. If the p4 annotation is uses, then
// the new name will be published in tdi json and this field that will be
// treated as an independent field and not as a field slice.
// Refer P4C-1293 for more details on how the compiler will publish
// different fields
std::string _tdi_parse_getParentName(const tdi::Cjson &key_field) {
  const std::string key_name = key_field["name"];
  tdi::Cjson key_annotations = key_field["annotations"];
  for (const auto &annotation : key_annotations.getCjsonChildVec()) {
    std::string annotation_name = (*annotation)["name"];
    std::string annotation_value = (*annotation)["value"];
    if ((annotation_name == "isFieldSlice") && (annotation_value == "true")) {
      size_t offset = key_name.find_last_of("[");
      if (offset == std::string::npos) {
        LOG_ERROR(
            "%s:%d ERROR %s Field is a field slice but the name does not "
            "contain a '['",
            __func__,
            __LINE__,
            key_name.c_str());
        TDI_DBGCHK(0);
        return key_name;
      }
      return std::string(key_name.begin(), key_name.begin() + offset);
    }
  }
  return key_name;
}

// This function determines the offset and the size(in bytes) of the field
tdi_status_t _tdi_parse_keyByteSizeAndOffsetGet(
    const std::string &table_name,
    const std::string &key_name,
    const std::map<std::string, size_t> &match_key_field_name_to_position_map,
    const std::map<std::string, size_t>
        &match_key_field_name_to_parent_field_byte_size_map,
    const std::map<size_t, size_t> &match_key_field_position_to_offset_map,
    size_t *field_offset,
    size_t *parent_field_byte_size) {
  const auto iter_1 = match_key_field_name_to_position_map.find(key_name);
  // Get the field offset and the size of the field in bytes
  if (iter_1 == match_key_field_name_to_position_map.end()) {
    LOG_ERROR(
        "%s:%d %s ERROR key field name %s not found in "
        "match_key_field_name_to_position_map ",
        __func__,
        __LINE__,
        table_name.c_str(),
        key_name.c_str());
    // TDI_DBGCHK(0);
    return TDI_OBJECT_NOT_FOUND;
  }
  size_t position = iter_1->second;
  const auto iter_2 = match_key_field_position_to_offset_map.find(position);
  if (iter_2 == match_key_field_position_to_offset_map.end()) {
    LOG_ERROR(
        "%s:%d %s ERROR Unable to find offset of key field %s with position "
        "%zu",
        __func__,
        __LINE__,
        table_name.c_str(),
        key_name.c_str(),
        position);
    TDI_DBGCHK(0);
    return TDI_OBJECT_NOT_FOUND;
  }
  *field_offset = iter_2->second;
  const auto iter_3 =
      match_key_field_name_to_parent_field_byte_size_map.find(key_name);
  if (iter_3 == match_key_field_name_to_parent_field_byte_size_map.end()) {
    LOG_ERROR(
        "%s:%d %s ERROR key field name %s not found in "
        "match_key_field_name_to_parent_field_byte_size_map ",
        __func__,
        __LINE__,
        table_name.c_str(),
        key_name.c_str());
    TDI_DBGCHK(0);
    return TDI_OBJECT_NOT_FOUND;
  }
  *parent_field_byte_size = iter_3->second;
  return TDI_SUCCESS;
}
// This function returns the start bit of the field
// If the field is a field slice then the start bit is derived from the name
// The format of a field slice name is as follows :
// <field_name>[<end_bit>:<start_bit>]
// If the field is a name annotated field slice or not a field slice at all,
// then the start bit is read from the context json node if present or set
// to zero if the context json node is absent
size_t _tdi_parse_getStartBit(const tdi::Cjson *tdi_json_key_field) {
  const std::string key_name = (*tdi_json_key_field)["name"];
  tdi::Cjson key_annotations = (*tdi_json_key_field)["annotations"];
  for (const auto &annotation : key_annotations.getCjsonChildVec()) {
    std::string annotation_name = (*annotation)["name"];
    std::string annotation_value = (*annotation)["value"];
    if ((annotation_name == "isFieldSlice") && (annotation_value == "true")) {
      size_t offset = key_name.find(":");
      if (offset == std::string::npos) {
        LOG_ERROR(
            "%s:%d ERROR %s Field is a field slice but the name does not "
            "contain a ':'",
            __func__,
            __LINE__,
            key_name.c_str());
        TDI_DBGCHK(0);
        return 0;
      }
      return std::stoi(key_name.substr(offset + 1), nullptr, 10);
    }
  }

  // This indicates that the key field is not a field slice
  return 0;
}

std::unique_ptr<KeyFieldInfo> TdiInfoParser::parseKeyField(
    const tdi::Cjson &table_key_cjson) {
  // parses the table_key json node and extracts all the relevant
    std::string key_name = table_key_cjson["name"];
    tdi_field_data_type_e field_data_type;
    size_t width;
    uint64_t default_value;
    float default_fl_value;
    std::string default_str_value;
    std::vector<std::string> choices;
    std::string data_type;
    parseFieldWidth(table_key_cjson,
                    field_data_type,
                    width,
                    default_value,
                    default_fl_value,
                    default_str_value,
                    choices);

    // create key_field structure and fill it
    auto tmp =
        new KeyFieldInfo(static_cast<tdi_id_t>(table_key_cjson["id"]),
                         static_cast<std::string>(table_key_cjson["name"]),
                         width,
                         field_data_type,
                         static_cast<bool>(table_key_cjson["mandatory"]),
                         static_cast<bool>(table_key_cjson["read_only"]),
                         choices,
                         parseAnnotations(table_key_cjson["annotations"]),
                         default_value,
                         default_fl_value,
                         default_str_value,
                         checkIsFieldSlice(table_key_cjson),
                         false,
                         false);
    if (tmp == nullptr) {
      LOG_ERROR("%s:%d Error forming key_field %d",
                __func__,
                __LINE__,
                static_cast<uint32_t>(table_key_cjson["id"]));
  }

  std::unique_ptr<KeyFieldInfo> key_field(tmp);
  if (key_field == nullptr) {
    LOG_ERROR("%s:%d Error forming key_field %d",
              __func__,
              __LINE__,
              static_cast<uint32_t>(table_key_cjson["id"]));
  }
  return key_field;
}

std::set<tdi::Annotation> TdiInfoParser::parseAnnotations(
    const tdi::Cjson &annotation_cjson) {
  std::set<tdi::Annotation> annotations;
  for (const auto &annotation : annotation_cjson.getCjsonChildVec()) {
    std::string annotation_name = (*annotation)["name"];
    std::string annotation_value = (*annotation)["value"];
    annotations.emplace(annotation_name, annotation_value);
  }
  return annotations;
}

std::unique_ptr<struct DataFieldInfo> TdiInfoParser::parseDataField(
    const tdi::Cjson &data_json_in,
    const uint64_t &oneof_index) {
  tdi::Cjson data_json = data_json_in;
  // if "singleton" field exists, then
  // lets point to it for further parsing
  if (data_json["singleton"].exists()) {
    data_json = data_json["singleton"];
  }
  // first get mandatory and read_only
  // because it exists outside oneof and singleton
  auto mandatory = bool(data_json["mandatory"]);
  auto read_only = bool(data_json["read_only"]);
  std::set<tdi_id_t> oneof_siblings;
  if (data_json["oneof"].exists()) {
    // Create a set of all the oneof members IDs
    for (const auto &oneof_data : data_json["oneof"].getCjsonChildVec()) {
      oneof_siblings.insert(static_cast<tdi_id_t>((*oneof_data)["id"]));
    }
    data_json = data_json["oneof"][oneof_index];
    // remove this field's ID from the siblings. One
    // cannot be their own sibling now, can they?
    oneof_siblings.erase(static_cast<tdi_id_t>(data_json["id"]));
  }
  std::string data_name = static_cast<std::string>(data_json["name"]);
  // get "type"
  tdi_field_data_type_e field_data_type;
  size_t width;
  std::vector<std::string> choices;
  // Default value of the field. We currently only support listing upto 64 bits
  // of default value
  uint64_t default_value;
  float default_fl_value;
  std::string default_str_value;
  parseFieldWidth(data_json,
                  field_data_type,
                  width,
                  default_value,
                  default_fl_value,
                  default_str_value,
                  choices);

  bool repeated = data_json["repeated"];
  // Get Annotations if any
  std::set<Annotation> annotations =
      parseAnnotations(data_json["annotations"]);

  tdi_id_t data_id = static_cast<tdi_id_t>(data_json["id"]);
  // Parse the container if it exists
  bool container_valid = false;
  if (data_json["container"].exists()) {
    container_valid = true;
  }
  std::unique_ptr<struct DataFieldInfo> data_field(
      new struct DataFieldInfo(
                               data_id,
                               data_name,
                               width,
                               action_id,
                               field_data_type
                               std::move(choices),
                               annotations,
                               default_value,
                               default_fl_value,
                               default_str_value,
                               repeated,
                               mandatory,
                               read_only,
                               container_valid,
                               oneof_siblings));

  // Parse the container if it exists

#if 0
  if (container_valid) {
    tdi::Cjson c_table_data = data_json["container"];
    parseContainer(c_table_data, tdiTable, data_field.get());
  }
#endif
  return data_field;
}

// This variant of the function is to parse the action specs for all the tables
// except phase0 table
std::unique_ptr<ActionInfo> TdiInfoParser::parseAction(
    const tdi::Cjson &action_json) {
  std::unique_ptr<ActionInfo> action_info(new ActionInfo());
  action_info->name = static_cast<std::string>(action_tdi["name"]);
  action_info->action_id = static_cast<tdi_id_t>(action_tdi["id"]);
  action_info->annotations =
      parseAnnotations(action_tdi["annotations"]);
  tdi::Cjson action_indirect;  // dummy
  if (action_context.exists()) {
    action_info->act_fn_hdl =
        static_cast<pipe_act_fn_hdl_t>(action_context["handle"]);
    /* applyMaskOnContextJsonHandle(&action_info->act_fn_hdl,
     * tdiTable->table_name_get()); */
    action_indirect = action_context["indirect_resources"];
  } else {
    action_info->act_fn_hdl = 0;
  }
  // get action profile data_json
  tdi::Cjson action_data_cjson = action_tdi["data"];
  size_t offset = 0;
  size_t bitsize = 0;
  for (const auto &action_data : action_data_cjson.getCjsonChildVec()) {
    auto data_field = parseDataField(*action_data);
    if (action_info->data_fields.find(data_field->getId()) !=
        action_info->data_fields.end()) {
      LOG_ERROR("%s:%d Key \"%u\" Exists for data ",
                __func__,
                __LINE__,
                data_field->getId());
      continue;
    }
    action_info->data_fields_names[data_field->getName()] = data_field.get();
    action_info->data_fields[data_field->getId()] = std::move(data_field);
  }
  // Offset value will be the total size of the action spec byte array
  action_info->dataSz = offset;
  // Bitsize is needed to fill out some back-end info
  // which has both action size in bytes and bits
  action_info->dataSzbits = bitsize;
  return action_info;
}
/* The function pasrse the provided json schema file and return an tdi_info
 * object that later can be used to
 * create a finalized tdi table obj, possible a derived class from tdi_table
 */
std::unique_ptr<tdi::TableInfo> TdiInfoParser::parseTable(
    const tdi::Cjson &table_tdi) {
  tdi_id_t table_id = table_tdi["id"];
  std::string table_name = table_tdi["name"];
  std::string table_type = table_tdi["table_type"];
  size_t table_size = static_cast<unsigned int>(table_tdi["size"]);
  key_size_t table_total_key_size;
  std::map<tdi_id_t, std::unique_ptr<KeyFieldInfo>> table_key_map;
  std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> table_data_map;
  std::map<tdi_id_t, std::unique_ptr<ActionInfo>> table_action_map;
  std::map<tdi_id_t, std::unique_ptr<TableRefInfo>> table_ref_map;

  LOG_DBG("Table : %s :: Type :: %s ID :: %d SIZE :: %lu",
            table_name.c_str(),
            tdi_table_type.c_str(),
            table_id,
            table_size);

  ////////////////////
  // getting key   //
  ///////////////////
  tdi::Cjson table_key_cjson = table_tdi["key"];
  for (const auto &key : table_key_cjson.getCjsonChildVec()) {
    std::unique_ptr<KeyFieldInfo> key_field =
        parseKeyField(*key);
    if (key_field == nullptr) {
      continue;
    }
    auto elem = table_key_map.find(key_field->getId());
    if (elem == table_key_map.end()) {
      table_key_map[key_field->field_id] = (std::move(key_field));
      table_total_key_size.bytes += key_field->field_offset;
      table_total_key_size.bits += key_field->size_bits;
    } else {
      // Field id is repeating, log an error message
      LOG_ERROR("%s:%d Field ID %d is repeating",
                __func__,
                __LINE__,
                key_field->getId());
      return nullptr;
    }
  }

  ////////////////////
  // getting data   //
  ////////////////////
  tdi::Cjson table_data_cjson = table_tdi["data"];
  for (const auto &data_json : table_data_cjson.getCjsonChildVec()) {
    std::string data_name;
    size_t offset = 0;
    // Bitsize is needed to fill out some back-end info
    // which has both action size in bytes and bits
    // This is not applicable for the common data, but only applicable
    // for per action data
    size_t bitsize = 0;
    int oneof_size = 1;
    if ((*data_json)["oneof"].exists()) {
      oneof_size = (*data_json)["oneof"].array_size();
    }
    tdi::Cjson temp;
    for (int oneof_loop = 0; oneof_loop < oneof_size; oneof_loop++) {
      bool is_register_data_field = false;
      auto data_field = parseDataField(*data_json, oneof_loop);
      tdi_id_t data_field_id = data_field->field_id;
      if (table_data_map.find(data_field_id) != table_data_map.end()) {
        LOG_ERROR("%s:%d Id \"%u\" Exists for common data of table %s",
                  __func__,
                  __LINE__,
                  data_field_id,
                  table_name.c_str());
        continue;
      }
      // insert data_field in table
      common_data_fields_names[data_field->getName()] = data_field.get();
      table_data_map[data_field_id] = std::move(data_field);
    }
  }

  ////////////////////////
  // getting attributes //
  ////////////////////////
  std::vector<std::string> attributes_v =
      table_tdi["attributes"].getCjsonChildStringVec();
  for (auto const &item : attributes_v) {
    table_attribute_set.insert(item);
  }

  ////////////////////
  // getting action //
  ////////////////////
  tdi::Cjson table_action_spec_cjson = table_tdi["action_specs"];
  for (const auto &action : table_action_spec_cjson.getCjsonChildVec()) {
    std::unique_ptr<ActionInfo> action_info;
    tdi::Cjson dummy;
    action_info =
        actionSpecsParse(*action, dummy, table_type, tdiTable.get());
    auto elem = table_action_map.find(action_info->action_id);
    if (elem == table_action_map.end()) {
      auto act_id = action_info->action_id;
      auto act_name = action_info->name;
      table_action_map[action_info->action_id] = (std::move(action_info));
      table_action_map_from_str[act_name] = act_id;
    } else {
      // Action id is repeating. Log an error message
      LOG_ERROR("%s:%d Action ID %d is repeating",
                __func__,
                __LINE__,
                action_info->action_id);
      return nullptr;
    }
  }

  //////////////////////////
  // getting dependencies //
  //////////////////////////
  tdi::Cjson depends_on_cjson = table_tdi["depends_on"];
  std::string ref_name = "other";
  for (const auto &tbl_id : depends_on_cjson.getCjsonChildVec()) {
    struct TableRefInfo ref_info;
    ref_info.id = static_cast<tdi_id_t>(*tbl_id);
    ref_info.name = "";
    for (auto &tbl_tdi_cjson : table_cjson_map) {
      // Check if table present in tdi.json if so then verify if id match
      if (tbl_tdi_cjson.second.first != nullptr &&
          static_cast<tdi_id_t>((*tbl_tdi_cjson.second.first)["id"]) ==
              ref_info.id) {
        ref_info.name = tbl_tdi_cjson.first;
      }
    }
    if (ref_info.name == "") {
      LOG_TRACE("%s:%d Cannot find proper reference table info for id %u",
                __func__,
                __LINE__,
                ref_info.id);
      break;
    }
    auto context_cjson = table_cjson_map.at(ref_info.name).second;
    if (!context_cjson || !context_cjson->exists()) continue;

    ref_info.tbl_hdl = static_cast<tdi_id_t>((*context_cjson)["handle"]);
    applyMaskOnContextJsonHandle(&ref_info.tbl_hdl, ref_info.name);
    ref_info.indirect_ref = true;

    LOG_DBG("%s:%d Adding \'%s\' as \'%s\' of \'%s\'",
            __func__,
            __LINE__,
            ref_info.name.c_str(),
            ref_name.c_str(),
            tdiTable->table_name_get().c_str());
    tdiTable->table_ref_map[ref_name].push_back(std::move(ref_info));
  }

  //////////////////////////
  // create tableInfo     //
  //////////////////////////
  auto tmp_tbl = new TableInfo();
  if (tmp_tbl == nullptr) {
    LOG_ERROR("%s:%d Error forming tableInfo for table id %d",
              __func__,
              __LINE__,
              table_id);
  }
  tmp_tbl->table_name = table_name;
  tmp_tbl->table_key_map = table_key_map;
  tmp_tbl->table_data_map = table_data_map;
  tmp_tbl->table_attribute_set = table_attribute_set;

  std::unique_ptr<TableInfo> ret_uqi_ptr(tmp_tbl);
  if (ret_uqi_ptr == nullptr) {
    LOG_ERROR("%s:%d Error forming tableInfo for table id %d",
              __func__,
              __LINE__,
              table_id);
  }
  return ret_uqi_ptr;
}

tdi_status_t TdiInfoParser::tdiInfoCreate(
    const std::vector<std::string> &tdi_info_file_paths) {
  // A. read file form a list of schema files
  if (tdi_info_file_paths.empty()) {
    LOG_CRIT("Unable to find any TDI Json Schema File");
    return TDI_ERR;
  }
  for (auto const &tdiJsonFile : tdi_info_file_paths) {
    std::ifstream file(tdiJsonFile);
    if (file.fail()) {
      LOG_CRIT("Unable to find TDI Json File %s", tdiJsonFile.c_str());
      return TDI_ERR;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    tdi::Cjson root_cjson = tdi::Cjson::createCjsonFromFile(content);
    tdi::Cjson tables_cjson = root_cjson[tdi_json::TABLES];
    for (const auto &table : tables_cjson.getCjsonChildVec()) {
      // B. parse file to form tdi_table_info object
      std::string table_name = static_cast<std::string>((*table)[tdi_json::TABLE_NAME]);
      table_info_map_[table_name] = this->parseTable(*table);
    }

    tdi::Cjson tables_cjson = root_cjson[tdi_json::LEARNS];
    for (const auto &table : tables_cjson.getCjsonChildVec()) {
      // C. parse file to form tdi_learn_info object
      std::string learn_name = static_cast<std::string>((*learn)["name"]);
      learn_info_map_[learn_name] = this->parseLearn(*learn);
    }
  }
  return TDI_OK;
}

} // namespace tdi

