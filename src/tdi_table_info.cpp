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

#include <tdi/common/tdi_table_info.hpp>

namespace tdi {

class KeyFieldInfo {
  tdiId field_id;
  size_t size_bits;
  std::string match_type;
  std::string data_type;
  std::string name;
  std::vector<std::string> enum_choices;
  std::set<std::string> annotations;
  // Default value for this data field
  uint64_t default_value;
  float default_fl_value;
  std::string default_str_value;
  size_t field_offset;
  size_t start_bit{0};
  // Flag to indicate if this is a field slice or not
  bool is_field_slice{false};
  // This might vary from the 'size_bits' in case of
  // field slices when the field slice width does not
  // equal the size of the entire key field
  size_t parent_field_full_byte_size{0};
  bool is_ptr;
  // flag to indicate whether it is a priority index or not
  bool is_partition;
  // A flag to indicate this is a match priority field
  bool match_priority;
};

class DataFieldInfo {
  std::string name;
  tdiId field_id;
  tdiId action_id;
  bool  ptr_valid;
  size_t size;
  size_t field_offset;
  std::string field_type;
  std::string data_type;
  // Vector of allowed choices for this field
  std::vector<std::string> enum_choices;
  // Default value for this data field
  uint64_t default_value;
  float default_fl_value;
  std::string default_str_value;
  bool mandatory;
  bool read_only;
  bool container_valid{false};
  /* Map of Objects within container */
  std::map<tdiId, std::unique_ptr<DataFieldInfo>> container;
  std::map<std::string, tdiId> container_names;
  std::set<std::string> annotations;
  std::set<tdiId> oneof_siblings_;
};

/* Structure to keep action info */
class ActionInfo {
  tdiId action_id;
  std::string name;
  // Map of table_data_fields
  std::map<tdiId, std::unique_ptr<DataFieldInfo>> data_fields;
  // Map of table_data_fields with names
  std::map<std::string, tdiId> data_fields_names;
  // Size of the action data in bytes
  size_t dataSz;
  // Size of the action data in bits (not including byte padding)
  size_t dataSzbits;
  std::set<std::string> annotations;
};

/* The meta structure to keep info on the table, generated from _tdi_parse logic */
class TableInfo {
  std::string table_name;
  std::string table_type;
  std::map<tdiId, std::unique_ptr<KeyFieldInfo>>  table_key_map;
  std::map<tdiId, std::unique_ptr<DataFieldInfo>> table_data_map;
  std::map<tdiId, std::unique_ptr<ActionInfo>>    table_action_map;
  std::map<tdiId, std::unique_ptr<TableRefInfo>>  table_ref_map;
  std::set<std::string> table_attribute_set;
  std::set<std::string> table_operation_set;
  std::set<std::string> table_api_set; // lookup tm fixed json
};

/* class to keep info regarding a reference to another tableInfo */
class TableRefInfo {
  tdiId id;
  std::string name;
  pipe_tbl_hdl_t tbl_hdl;
  // A flag to indicate if the reference is indirect. TRUE when it is, FALSE
  // when the refernece is direct
  bool indirect_ref;
};

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
  *name = object_name;
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
  *id = object_id;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableTypeGet(tdi_table_type_e *table_type) const {
  if (nullptr == table_type) {
    LOG_ERROR("%s:%d Outparam passed is nullptr", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  *table_type = object_type;
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

tdi_status_t TableInfo::keyFieldIdListGet(std::vector<tdi_id_t> *id_vec) const {
  if (id_vec == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  for (const auto &kv : key_field_info_map_) {
    id_vec->push_back(kv.first);
  }
  std::sort(id_vec->begin(), id_vec->end());
  return TDI_SUCCESS;
}
tdi_status_t TableInfo::tableAttributesSupported(
    std::set<tdi_attributes_type_e> *type_set) const {
  *type_set = attributes_type_set;
  return TDI_SUCCESS;
}

tdi_status_t TableInfo::tableOperationsSupported(
    std::set<tdi_operations_type_e> *op_type_set) const {
  *op_type_set = operations_type_set;
  return TDI_SUCCESS;
}

#if 0
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

tdi_status_t Table::keyFieldDataTypeGet(const tdi_id_t &field_id,
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

tdi_status_t Table::keyFieldIdGet(const std::string &name,
                                        tdi_id_t *field_id) const {
  if (field_id == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  auto found = std::find_if(
      key_fields.begin(),
      key_fields.end(),
      [&name](const std::pair<const tdi_id_t,
                              std::unique_ptr<TableKeyField>> &map_item) {
        return (name == map_item.second->getName());
      });
  if (found != key_fields.end()) {
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

uint32_t Table::dataFieldListSize() const {
  return this->dataFieldListSize(0);
}

uint32_t Table::dataFieldListSize(const tdi_id_t &action_id) const {
  if (action_info_list.find(action_id) != action_info_list.end()) {
    return action_info_list.at(action_id)->data_fields.size() +
           common_data_fields.size();
  }
  return common_data_fields.size();
}

tdi_status_t Table::dataFieldIdListGet(
    const tdi_id_t &action_id, std::vector<tdi_id_t> *id_vec) const {
  if (id_vec == nullptr) {
    LOG_ERROR("%s:%d %s Please pass mem allocated out param",
              __func__,
              __LINE__,
              table_name_get().c_str());
    return TDI_INVALID_ARG;
  }
  if (action_id) {
    if (action_info_list.find(action_id) == action_info_list.end()) {
      LOG_ERROR("%s:%d %s Action Id %d Not Found",
                __func__,
                __LINE__,
                table_name_get().c_str(),
                action_id);
      return TDI_OBJECT_NOT_FOUND;
    }
    for (const auto &kv : action_info_list.at(action_id)->data_fields) {
      id_vec->push_back(kv.first);
    }
  }
  // Also include common data fields
  for (const auto &kv : common_data_fields) {
    id_vec->push_back(kv.first);
  }
  std::sort(id_vec->begin(), id_vec->end());
  return TDI_SUCCESS;
}

tdi_status_t Table::dataFieldIdListGet(
    std::vector<tdi_id_t> *id_vec) const {
  return this->dataFieldIdListGet(0, id_vec);
}

tdi_status_t Table::containerDataFieldIdListGet(
    const tdi_id_t &field_id, std::vector<tdi_id_t> *id_vec) const {
  if (id_vec == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
  auto status = getDataField(field_id, 0, &field);
  if (status != TDI_SUCCESS) {
    LOG_TRACE("%s:%d Field ID %d not found", __func__, __LINE__, field_id);
    return status;
  }
  return field->containerFieldIdListGet(id_vec);
}

tdi_status_t Table::dataFieldIdGet(const std::string &name,
                                         tdi_id_t *field_id) const {
  return this->dataFieldIdGet(name, 0, field_id);
}

tdi_status_t Table::dataFieldIdGet(const std::string &name,
                                         const tdi_id_t &action_id,
                                         tdi_id_t *field_id) const {
  if (field_id == nullptr) {
    LOG_ERROR("%s:%d Please pass mem allocated out param", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  const TableDataField *field;
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

tdi_status_t Table::dataFieldAnnotationsGet(
    const tdi_id_t &field_id, AnnotationSet *annotations) const {
  return this->dataFieldAnnotationsGet(field_id, 0, annotations);
}

tdi_status_t Table::dataFieldAnnotationsGet(
    const tdi_id_t &field_id,
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

tdi_status_t Table::defaultDataValueGet(
    const tdi_id_t &field_id,
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
                              std::unique_ptr<tdi_info_action_info_t>> &
                  action_map_pair) {
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

tdi_status_t Table::actionIdListGet(
    std::vector<tdi_id_t> *id_vec) const {
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

tdi_status_t Table::actionAnnotationsGet(
    const tdi_id_t &action_id, AnnotationSet *annotations) const {
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
  auto table_ops_impl =
      static_cast<const TableOperationsImpl *>(&table_ops);
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

tdi_status_t Table::dataFieldTypeGet(
    const tdi_id_t &field_id, std::set<DataFieldType> *ret_type) const {
  return this->dataFieldTypeGet(field_id, 0, ret_type);
}

tdi_status_t Table::dataFieldTypeGet(
    const tdi_id_t &field_id,
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

tdi_status_t Table::getDataField(
    const tdi_id_t &field_id,
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

tdi_status_t Table::getDataField(
    const std::string &field_name,
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
  auto bf_status = tableIdGet(&id);
  if (bf_status == TDI_SUCCESS) {
    return id;
  }
  return 0;
}
const std::string &Table::key_field_name_get(
    const tdi_id_t &field_id) const {
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

const std::string &Table::data_field_name_get(
    const tdi_id_t &field_id) const {
  return this->data_field_name_get(field_id, 0);
}

const std::string &Table::data_field_name_get(
    const tdi_id_t &field_id, const tdi_id_t &action_id) const {
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

const std::string &Table::action_name_get(
    const tdi_id_t &action_id) const {
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
  tdi_status_t bf_status = this->tableIdGet(&table_id);
  if (bf_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id, err %d",
              __func__,
              __LINE__,
              this->table_name_get().c_str(),
              bf_status);
    TDI_DBGCHK(0);
    return false;
  }

  bf_status = key.tableGet(&table_from_key);
  if (bf_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table object from key object, err %d",
              __func__,
              __LINE__,
              this->table_name_get().c_str(),
              bf_status);
    TDI_DBGCHK(0);
    return false;
  }

  bf_status = table_from_key->tableIdGet(&table_id_of_key_obj);
  if (bf_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id from key object, err %d",
              __func__,
              __LINE__,
              this->table_name_get().c_str(),
              bf_status);
    TDI_DBGCHK(0);
    return false;
  }

  if (table_id == table_id_of_key_obj) {
    return true;
  }
  return false;
}

bool Table::validateTable_from_dataObj(
    const TableDataObj &data) const {
  tdi_id_t table_id;
  tdi_id_t table_id_of_data_obj;

  tdi_status_t bf_status = this->tableIdGet(&table_id);
  if (bf_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id, err %d",
              __func__,
              __LINE__,
              this->table_name_get().c_str(),
              bf_status);
    TDI_DBGCHK(0);
    return false;
  }

  const Table *table_from_data = nullptr;
  bf_status = data.getParent(&table_from_data);
  if (bf_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table object from data object, err %d",
              __func__,
              __LINE__,
              this->table_name_get().c_str(),
              bf_status);
    TDI_DBGCHK(0);
    return false;
  }

  bf_status = table_from_data->tableIdGet(&table_id_of_data_obj);
  if (bf_status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d %s ERROR in getting table id from data object, err %d",
              __func__,
              __LINE__,
              this->table_name_get().c_str(),
              bf_status);
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

} /* END of tdi namespace */

std::map< std::string, std::unique_ptr<struct table_info> > tdi_info_map;
static const std::string match_key_priority_field_name = "$MATCH_PRIORITY";

/* the ID that is generated by the compiler, the users, or provided by the deivce specific code.
   It is the id preseted in json schema file.
   */
typedef size_t tdiId;

typedef struct key_size_ {
  size_t bytes;
  size_t bits;
} key_size_t;



void _tdi_parse_getTableFieldWidth(
    const bfrt::Cjson &node,
    std::string &type,
    size_t &width,
    uint64_t &default_value,
    float &default_fl_value,
    std::string &default_str_value,
    std::vector<std::string> &choices)
{
  //  bfrt::Cjson node_type = node["type"];
  type = std::string(node["type"]["type"]);
  if (type == "bytes") {
    width = static_cast<unsigned int>(node["type"]["width"]);
  } else if (type == "uint64") {
    width = 64;
  } else if (type == "uint32") {
    width = 32;
  } else if (type == "uint16") {
    width = 16;
  } else if (type == "uint8") {
    width = 8;
  } else if (type == "bool") {
    width = 1;
  } else if (type == "float") {
    width = 0;
    // If default value is listed, populate it, else its zero
    if (node["type"]["default_value"].exists()) {
      default_fl_value = static_cast<float>(node["type"]["default_value"]);
    } else {
      default_fl_value = 0.0;
    }
  } else if (type == "string") {
    width = 0;
    for (const auto &choice : node["type"]["choices"].getCjsonChildVec()) {
      choices.push_back(static_cast<std::string>(*choice));
    }
    // If string default value is listed populate it, else its empty
    if (node["type"]["default_value"].exists()) {
      default_str_value = static_cast<std::string>(node["type"]["default_value"]);
    } else {
      default_str_value = std::string("INVALID");
    }
    if (node["repeated"].exists()) {
      bool repeated = node["repeated"];
      if (repeated) type = "string_arr";
    }
  } else if (node["container"].exists()) {
    width = 0;
    type = "container";
  } else {
    width = 0;
  }

  // If default value is listed, populate it, else its zero
  if (node["type"]["default_value"].exists()) {
    default_value = static_cast<uint64_t>(node["type"]["default_value"]);
  } else {
    default_value = 0;
  }

  if (node["repeated"].exists()) {
    bool repeated = node["repeated"];
    if (repeated) {
      if (width == 32 || width == 16 || width == 8) {
        type = "int_arr";
      } else if (width == 1) {
        type = "bool_arr";
      }
    }
  }
}

// This function is only applicable when the key field is a field slice
// This function strips out '[]' if present in the name of the key_field.
// This will be true when the key is a field slice. The compiler will
// publish the name of the key with '[]' when the user does not use '@name'
// p4 annotation for the field slice. If the p4 annotation is uses, then
// the new name will be published in bfrt json and this field that will be
// treated as an independent field and not as a field slice.
// Refer P4C-1293 for more details on how the compiler will publish
// different fields
std::string _tdi_parse_getParentName(const bfrt::Cjson &key_field) {
  const std::string key_name = key_field["name"];
  bfrt::Cjson key_annotations = key_field["annotations"];
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
        BF_RT_DBGCHK(0);
        return key_name;
      }
      return std::string(key_name.begin(), key_name.begin() + offset);
    }
  }
  return key_name;
}

// This function determines the offset and the size(in bytes) of the field
bf_status_t _tdi_parse_keyByteSizeAndOffsetGet(
    const std::string &table_name,
    const std::string &key_name,
    const std::map<std::string, size_t> &match_key_field_name_to_position_map,
    const std::map<std::string, size_t> &
        match_key_field_name_to_parent_field_byte_size_map,
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
    // BF_RT_DBGCHK(0);
    return BF_OBJECT_NOT_FOUND;
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
    BF_RT_DBGCHK(0);
    return BF_OBJECT_NOT_FOUND;
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
    BF_RT_DBGCHK(0);
    return BF_OBJECT_NOT_FOUND;
  }
  *parent_field_byte_size = iter_3->second;
  return BF_SUCCESS;
}
// This function returns the start bit of the field
// If the field is a field slice then the start bit is derived from the name
// The format of a field slice name is as follows :
// <field_name>[<end_bit>:<start_bit>]
// If the field is a name annotated field slice or not a field slice at all,
// then the start bit is read from the context json node if present or set
// to zero if the context json node is absent
size_t _tdi_parse_getStartBit(const bfrt::Cjson *context_json_key_field,
                              const bfrt::Cjson *bfrt_json_key_field,
                              bfrt::BfRtTable::TableType table_type) {
  const std::string key_name = (*bfrt_json_key_field)["name"];
  bfrt::Cjson key_annotations = (*bfrt_json_key_field)["annotations"];
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
        BF_RT_DBGCHK(0);
        return 0;
      }
      return std::stoi(key_name.substr(offset + 1), nullptr, 10);
    }
  }

  bool is_context_json_node = false;
  switch(table_type) {
    case bfrt::BfRtTableObj::TableType::MATCH_DIRECT:
    case bfrt::BfRtTableObj::TableType::MATCH_INDIRECT:
    case bfrt::BfRtTableObj::TableType::MATCH_INDIRECT_SELECTOR:
    case bfrt::BfRtTableObj::TableType::PORT_METADATA:
      is_context_json_node = true;
      break;
    default:
      is_context_json_node = false;
      break;
  }
  // Since we couldn't get the start bit from the name of the key field, get
  // it from the context json if the context json node is present
  if (is_context_json_node) {
    if (key_name == match_key_priority_field_name) {
      // $MATCH_PRIORITY key field is not published in context json. Just return
      // zero in this case
      return 0;
    }

    for (const auto &match_field : context_json_key_field->getCjsonChildVec()) {
      if (static_cast<const std::string>((*match_field)["name"]) != key_name) {
        continue;
      } else {
        return static_cast<size_t>((*match_field)["start_bit"]);
      }
    }
    // This indicates that we weren't able to find the context json node of the
    // key field
    LOG_ERROR(
        "%s:%d ERROR %s Context json node not found for the key field. Hence "
        "unable to determine the start bit",
        __func__,
        __LINE__,
        key_name.c_str());
    // BF_RT_DBGCHK(0);
    return 0;
  }

  // This indicates that the key field is not a field slice and it does not
  // even have a context json node associated with it (this is true for fixed
  // tables, resourse tables, etc.) Hence just return 0
  return 0;
}

// This function returns if a key field is a field slice or not
bool _tdi_parse_isFieldSlice(
    const bfrt::Cjson &key_field,
    const size_t &start_bit,
    const size_t &key_width_byte_size,
    const size_t &key_width_parent_byte_size) {
  bfrt::Cjson key_annotations = key_field["annotations"];
  for (const auto &annotation : key_annotations.getCjsonChildVec()) {
    std::string annotation_name = (*annotation)["name"];
    std::string annotation_value = (*annotation)["value"];
    if ((annotation_name == "isFieldSlice") && (annotation_value == "true")) {
      return true;
    }
  }
  /// The 2 invariants to determine if a key field is a slice or not are the
  // following
  // 1. the start bit published in the context json is not zero (soft
  //    invariant)
  // 2. the bit_witdth and bit_width_full published in the context json
  //    are not equal (hard invariant)
  // The #1 invariant is soft because there can be cases where-in the start
  // bit is zero but the field is still a field slice. This can happen when
  // the p4 is written such that we have a template for a table wherein the
  // the match key field is templatized and then we instantiate it using a
  // field slice. As a result, the front end compiler (bf-rt generator)
  // doesn't know that the key field is actually a field slice and doesn't
  // publish the "is_field_slice" annotation in bf-rt json. However, the
  // the back-end compiler (context json generator) knows it's a field slice
  // and hence makes the bit width and bit-width-full unequal
  /// Test invariant #1
  // Now check if the start bit of the key field. If the key field is not a
  // field slice then the start bit has to be zero. If the start bit is not
  // zero. then that indicates that this key field is a name aliased field
  // slice. Return true in that case. We need to correctly distinguish if
  // a key field is a field slice (name aliased or not) or not, so that we
  // correctly and efficiently pack the field in the match spec byte buffer
  if (start_bit != 0) {
    return true;
  }
  /// Test invariant #2
  if (key_width_byte_size != key_width_parent_byte_size) {
    return true;
  }
  return false;
}

std::unique_ptr<KeyFieldInfo> _tdi_parse_keyParse(
  bfrt::Cjson &key_json,
  const tdiId &table_id,
  const size_t &field_offset,
  const size_t &start_bit,
  const size_t &parent_field_byte_size,
  const bool &is_atcam_partition_index) {

  // parses the table_key json node and extracts all the relevant
  // information into the helper maps. It extracts the following things
  // - Gets the position (the order in which they appear in p4) of every match field
  // - Gets the full size of every match field in bytes
  // - Gets the offset (in the match spec byte buf) of every match field
  bfrt::Cjson table_key_cjson = table_bfrt["key"];
  std::map<std::string, size_t> match_key_field_name_to_position_map;
  std::map<std::string, size_t> match_key_field_name_to_parent_field_byte_size_map;
  std::map<size_t, size_t> match_key_field_position_to_offset_map;
  std::map<std::string, size_t> match_key_field_name_to_bit_size_map;
  bool is_context_json_node = false;
  size_t cumulative_offset = 0;
  size_t cumulative_key_width_bits = 0;
  // This uses the bfrt json key node
  size_t pos = 0;
  // Since we don't support field slices for fixed objects, we use the bfrt
  // key json node to extract all the information. The position of the
  // fields will just be according to the order in which they are published
  // Hence simply increment the position as we iterate
  for (const auto &key_json : table_key_cjson->getCjsonChildVec()) {
    std::string key_name = (*key_json)["name"];
    std::string key_type_str;
    size_t width;
    uint64_t default_value;
    float default_fl_value;
    std::string default_str_value;
    std::vector<std::string> choices;
    _tdi_parse_getTableFieldWidth(
        *key_json,
        key_type_str,
        width,
        default_value,
        default_fl_value,
        default_str_value,
        choices);
    (*match_key_field_name_to_position_map)[key_name] = pos;
    (*match_key_field_position_to_offset_map)[pos] = (width + 7) / 8;
    (*match_key_field_name_to_parent_field_byte_size_map)[key_name] =
        (width + 7) / 8;
    match_key_field_name_to_bit_size_map[key_name] = width;
    pos++;
  }
  // Now that we have the position of each field with it's offset, get the
  // cumulative offset of each field.
  // Set the total offset and compute the cumulative bit size
  cumulative_offset = 0;
  cumulative_key_width_bits = 0;
  size_t prev_cumulative_offset = 0;
  for (auto &iter : (*match_key_field_position_to_offset_map)) {
    cumulative_offset += iter.second;
    iter.second = prev_cumulative_offset;
    prev_cumulative_offset = cumulative_offset;
  }
  for (const auto &iter : match_key_field_name_to_bit_size_map) {
    cumulative_key_width_bits += iter.second;
  }

  const std::string key_name = _tdi_parse_getParentName(key_json);
  const size_t start_bit = _tdi_parse_getStartBit(nullptr /* context json node */, key.get(), table_type);
  size_t field_offset = 0, parent_field_byte_size = 0;

  // Get the field offset and the size of the field in bytes
  if (_tdi_parse_keyByteSizeAndOffsetGet(
          table_name,
          key_name,
          match_key_field_name_to_position_map,
          match_key_field_name_to_parent_field_byte_size_map,
          match_key_field_position_to_offset_map,
          &field_offset,
          &parent_field_byte_size) != BF_SUCCESS) {
    LOG_ERROR(
        "%s:%d %s ERROR in processing key field %s while trying to get field "
        "size and offset",
        __func__,
        __LINE__,
        table_name.c_str(),
        key_name.c_str());
    BF_RT_DBGCHK(0);
    return nullptr;
  }
  std::string data_type;
  size_t width;
  uint64_t default_value;
  float default_fl_value;
  std::string default_str_value;
  std::vector<std::string> choices;

  _tdi_parse_getTableFieldWidth(
      key_json,
      data_type,
      width,
      default_value,
      default_fl_value,
      default_str_value,
      choices);

  // create key_field structure and fill it
  auto tmp = new KeyFieldInfo();
  if (tmp == nullptr) {
    LOG_ERROR("%s:%d Error forming key_field %d for table id %d",
              __func__,
              __LINE__,
              static_cast<uint32_t>(key["id"]),
              table_id);
  }
  tmp->field_id = key_json["id"];
  tmp->name = key_name;
  tmp->data_type = data_type;
  tmp->size_bits = width;
  tmp->match_type = key_json["match_type"];
  tmp->enum_choices  = choices;
  tmp->field_offset = field_offset;
  tmp->start_bit = start_bit;
  tmp->is_field_slice = _tdi_parse_isFieldSlice(key_json, start_bit, (width + 7) / 8, parent_field_byte_size);
  tmp->parent_field_full_byte_size = parent_field_full_byte_size;
  tmp->is_partition = is_atcam_partition_index;
  tmp->default_value = default_value;
  tmp->default_fl_value = default_fl_value;
  tmp->default_str_value = default_str_value;

  std::unique_ptr<KeyFieldInfo> key_field(tmp);
  if (key_field == nullptr) {
    LOG_ERROR("%s:%d Error forming key_field %d for table id %d",
              __func__,
              __LINE__,
              static_cast<uint32_t>(key["id"]),
              table_id);
  }
  return key_field;
}

std::set<bfrt::Annotation> _tdi_parse_parseAnnotations(bfrt::Cjson annotation_cjson) {
  std::set<bfrt::Annotation> annotations;
  for (const auto &annotation : annotation_cjson.getCjsonChildVec()) {
    std::string annotation_name = (*annotation)["name"];
    std::string annotation_value = (*annotation)["value"];
    annotations.emplace(annotation_name, annotation_value);
  }
  return annotations;
}

bool _tdi_parse_isActionParam_matchTbl(
    const std::string table_name,
    const std::map<std::string, std::vector<struct TableRefInfo>> table_ref_map;
    tdiId action_handle,
    std::string parameter_name)
{
  std::string ref = "action_data_table_refs";
  if (table_ref_map.find(ref) != table_ref_map.end()) {
    // If action table exists then check the action table for the param name
    for (const auto &action_table_res_pair : table_ref_map.at(ref) ) {
      if (table_cjson_map.find(action_table_res_pair.name) == table_cjson_map.end()) {
        LOG_ERROR("%s:%d Ref Table %s not found",
                  __func__,
                  __LINE__,
                  action_table_res_pair.name.c_str());
        continue;
      }
      bfrt::Cjson action_table_cjson = *(table_cjson_map.at(action_table_res_pair.name).second);
      auto action_table_name = action_table_res_pair.name;
      if (_tdi_parse_isActionParam(action_table_cjson, action_handle, action_table_name, parameter_name)) {
        return true;
      }
    }
  }

  // FIXME : we should probably move to parsing the p4 parameters. In that case
  // 	     we dont need to worry if the field is an immediate action or not
  //	     and we can get rid of the below logic

  // We are here because the parameter is not encoded in the action RAM. Next
  // check if its encoded as part of an immediate field

  bfrt::Cjson match_table_cjson = *(table_cjson_map.at(bfrtTable->table_name_get()).second);
  // Find if it's an ATCAM table
  std::string atcam = match_table_cjson["match_attributes"]["match_type"];
  bool is_atcam = (atcam == "algorithmic_tcam") ? true : false;
  // Find if it's an ALPM table
  bool is_alpm =
      match_table_cjson["match_attributes"]["pre_classifier"].exists();
  // For ALPM, table data fields reside in the algorithmic tcam part of the
  // table
  // ATCAM table will be present in the "atcam_table" node of the match table
  // json
  // There will be as many "units" as the number of ATCAM logical tables for the
  // ALPM table. We just process the first one in order to find out about action
  // tables
  bfrt::Cjson stage_table = match_table_cjson["match_attributes"]["stage_tables"];
  if (is_alpm) {
    stage_table =
        match_table_cjson["match_attributes"]["atcam_table"]["match_attributes"]
                         ["units"][0]["match_attributes"]["stage_tables"][0];
  } else if (is_atcam) {
    stage_table = match_table_cjson["match_attributes"]["units"][0]
                                   ["match_attributes"]["stage_tables"][0];
  } else {
    stage_table = match_table_cjson["match_attributes"]["stage_tables"][0];
  }

  bfrt::Cjson action_formats = stage_table["action_format"];

  if (stage_table["ternary_indirection_stage_table"].exists()) {
    action_formats =
        stage_table["ternary_indirection_stage_table"]["action_format"];
  }
  for (const auto &action_format : action_formats.getCjsonChildVec()) {
    auto json_action_handle =
        static_cast<unsigned int>((*action_format)["action_handle"]);
    // We need to modify the raw handle parsed before comparing
    applyMaskOnContextJsonHandle(&json_action_handle,
                                 bfrtTable->table_name_get());
    if (json_action_handle != action_handle) {
      continue;
    }
    for (const auto &imm_field :
         (*action_format)["immediate_fields"].getCjsonChildVec()) {
      if (static_cast<std::string>((*imm_field)["param_name"]) ==
          parameter_name) {
        return true;
      }
    }
  }
  return false;
}

bool _tdi_parse_isParamActionParam(
    bfrt::BfRtTable::TableType table_type,
    std::string table_name,
    tdiId action_handle,
    std::string parameter_name,
    bool use_p4_params_node)
{
  // If the table is a Match-Action table call the matchTbl function to analyze
  // if this parameter belongs to the action spec or not
  if (table_type == BfRtTable::TableType::MATCH_DIRECT) {
    return _tdi_parse_isActionParam_matchTbl(bfrtTable, action_handle, parameter_name);
  } else if (table_type == BfRtTable::TableType::ACTION_PROFILE) {
    return _tdi_parse_isActionParam_actProf(
        bfrtTable, action_handle, parameter_name, use_p4_params_node);
  } else if (table_type == BfRtTable::TableType::PORT_METADATA) {
    // Phase0 can have only action params as data
    return true;
  }
  return false;
}

// This funcion, given the parameter_name of a certain action figures out if the
// parameter is part of the action spec based on the action table entry
// formatting information
bool _tdi_parse_isActionParam_actProf(const BfRtTableObj *bfrtTable,
                                         tdiId action_handle,
                                         std::string parameter_name,
                                         bool use_p4_params_node) {
  bfrt::Cjson action_table_cjson =
      *(table_cjson_map.at(bfrtTable->object_name).second);

  auto action_table_name = bfrtTable->table_name_get();

  return _tdi_parse_isActionParam(action_table_cjson,
                       action_handle,
                       action_table_name,
                       parameter_name,
                       use_p4_params_node);
}

bool _tdi_parse_isActionParam(bfrt::Cjson action_table_cjson,
                              tdiId action_handle,
                              std::string action_table_name,
                              std::string parameter_name,
                              bool use_p4_params_node) {
  /* FIXME : Cleanup this hairy logic. For the time being we are cheking the
             pack format to figure this out, whereas we should be using the
             p4_parameters node (commented out logic down below). If we
             indeed move to parsing the p4_parameters, we could get rid of
             the new type that we added "ACTION_PARAM_OPTIMIZED_OUT"
  */
  if (!use_p4_params_node) {
    for (const auto &pack_format :
         action_table_cjson["stage_tables"][0]["pack_format"]
             .getCjsonChildVec()) {
      auto json_action_handle =
          static_cast<unsigned int>((*pack_format)["action_handle"]);
      // We need to modify the raw handle parsed before comparing
      applyMaskOnContextJsonHandle(&json_action_handle, action_table_name);
      if (json_action_handle != action_handle) {
        continue;
      }
      for (const auto &field :
           (*pack_format)["entries"][0]["fields"].getCjsonChildVec()) {
        if (static_cast<std::string>((*field)["field_name"]) ==
            parameter_name) {
          return true;
        }
      }
    }
    return false;
  } else {
    // Using the p4_parameters node to figure out if its an action param instead
    // of using the pack format. This is because there can be instances where
    // the param might be optimized out by the compiler and hence not appear in
    // pack format. However, the param always appears in bfrt json (optimized
    // out
    // or not). Hence if we use pack format to figure out if its an action param
    // there is an inconsistency (as the param exists in the bfrt json but not
    // in pack format). So its better to use p4_parameters as the param is
    // guaranteed to be present there (optimized or not)
    for (const auto &action :
         action_table_cjson["actions"].getCjsonChildVec()) {
      auto json_action_handle = static_cast<unsigned int>((*action)["handle"]);
      // We need to modify the raw handle parsed before comparing
      applyMaskOnContextJsonHandle(&json_action_handle, action_table_name);
      if (json_action_handle != action_handle) {
        continue;
      }
      for (const auto &field : (*action)["p4_parameters"].getCjsonChildVec()) {
        if (static_cast<std::string>((*field)["name"]) == parameter_name) {
          return true;
        }
      }
    }
    return false;
  }
}

std::unique_ptr<struct DataFieldInfo> _tdi_parse_dataParse(
    bfrt::Cjson data_json,
    bfrt::Cjson action_indirect_res,
    std::string this_table_type,
    std::string this_table_name,
    const bfrt::BfRtTableObj *bfrtTable,
    const TableInfo *parent_tbl_info,
    pipe_act_fn_hdl_t action_handle,
    tdiId table_id,
    tdiId table_id,
    tdiId action_id,
    uint32_t oneof_index,
    bool *is_register_data_field) {
  // first get mandatory and read_only
  // because it exists outside oneof and singleton
  auto mandatory = bool(data_json["mandatory"]);
  auto read_only = bool(data_json["read_only"]);
  // if "singleton" field exists, then
  // lets point to it for further parsing
  if (data_json["singleton"].exists()) {
    data_json = data_json["singleton"];
  }
  std::set<tdiId> oneof_siblings;
  if (data_json["oneof"].exists()) {
    // Create a set of all the oneof members IDs
    for (const auto &oneof_data : data_json["oneof"].getCjsonChildVec()) {
      oneof_siblings.insert(static_cast<tdiId>((*oneof_data)["id"]));
    }
    data_json = data_json["oneof"][oneof_index];
    // remove this field's ID from the siblings. One
    // cannot be their own sibling now, can they?
    oneof_siblings.erase(static_cast<tdiId>(data_json["id"]));
  }
  // get "name"
  std::string data_name = static_cast<std::string>(data_json["name"]);
  // get "type"
  std::string key_type_str;
  size_t width;
  std::vector<std::string> choices;
  // Default value of the field. We currently only support listing upto 64 bits
  // of default value
  uint64_t default_value;
  float default_fl_value;
  std::string default_str_value;
  _tdi_parse_getTableFieldWidth(
      data_json,
      key_type_str,
      width,
      default_value,
      default_fl_value,
      default_str_value,
      choices);

  bool repeated = data_json["repeated"];
  // Get Annotations if any
  // We need to use "annotations" field to detect if a particular data_json field
  // is a register data_json field as unlike other resource parameters register
  // fields have user defined names in bfrt json
  bool data_field_is_reg = false;
  std::set<Annotation> annotations = _tdi_parse_parseAnnotations(data_json["annotations"]);
  if (annotations.find(Annotation("$bfrt_field_class", "register_data")) !=
      annotations.end()) {
    data_field_is_reg = true;
    *is_register_data_field = data_field_is_reg;
  }

  tdiId table_id = bfrtTable->table_id_get();
  tdiId data_id = static_cast<tdiId>(data_json["id"]);
  // Parse the container if it exists
  bool container_valid = false;
  if (data_json["container"].exists()) {
    container_valid = true;
  }
  std::unique_ptr<struct DataFieldInfo> data_field(
      new struct DataFieldInfo(
        table_id,
        data_id,
        data_name,
        action_id,
        width,
        type,
        std::move(choices),
        default_value,
        default_fl_value,
        default_str_value,
        repeated,
        mandatory,
        read_only,
        container_valid,
        annotations,
        oneof_siblings
      )
  );

  // Parse the container if it exists
  if (container_valid) {
    bfrt::Cjson c_table_data = data_json["container"];
    parseContainer(c_table_data, bfrtTable, data_field.get());
  }
  return data_field;
}

// This variant of the function is to parse the action specs for all the tables
// except phase0 table
std::unique_ptr<ActionInfo> _tdi_parse_actionSpecsParse(
    bfrt::Cjson &action_bfrt,
    bfrt::Cjson &action_context,
    bfrt::BfRtTable::TableType table_type,
    const bfrt::BfRtTableObj *bfrtTable)
{
  std::unique_ptr<ActionInfo> action_info(new ActionInfo());
  action_info->name = static_cast<std::string>(action_bfrt["name"]);
  action_info->action_id = static_cast<tdiId>(action_bfrt["id"]);
  action_info->annotations = _tdi_parse_parseAnnotations(action_bfrt["annotations"]);
  bfrt::Cjson action_indirect;  // dummy
  if (action_context.exists()) {
    action_info->act_fn_hdl =
        static_cast<pipe_act_fn_hdl_t>(action_context["handle"]);
    /* applyMaskOnContextJsonHandle(&action_info->act_fn_hdl, bfrtTable->table_name_get()); */
    action_indirect = action_context["indirect_resources"];
  } else {
    action_info->act_fn_hdl = 0;
  }
  // get action profile data_json
  bfrt::Cjson action_data_cjson = action_bfrt["data"];
  size_t offset = 0;
  size_t bitsize = 0;
  for (const auto &action_data : action_data_cjson.getCjsonChildVec()) {
    auto data_field = _tdi_parse_dataParse(*action_data,
                                action_indirect,
                                bfrtTable->table_name_get(),
                                table_type,
                                bfrtTable,
                                offset,
                                bitsize,
                                action_info->act_fn_hdl,
                                action_info->action_id,
                                0);
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
/* The function pasrse the provided json schema file and return an tdi_info object that later can be used to
 * create a finalized tdi table obj, possible a derived class from tdi_table
 */
std::unique_ptr<bfrt::BfRtTableObj> _tdi_parse_table(const std::shared_ptr<bfrt::Cjson> json_schema) {
  /* std::string prog_name = program_config.prog_name_; */
  // Name of the program to which this table obj belongs
  std::string prog_name;

  // hash_bit_width of hash object. Only required for the hash tables
  uint64_t hash_bit_width = 0;

  // action_info map
  std::map<tdiId, std::unique_ptr<ActionInfo>> table_action_map;
  std::map<std::string, tdiId> table_action_map_from_str;

  mutable std::mutex state_lock;
  bf_status_t action_id_get_helper(const std::string &name,
                                   tdiId *action_id) const;
  // Whether this table has a const default action or not
  bool has_const_default_action_{false};
  // Whether this is a const table
  bool is_const_table_{false};
  // Table annotations
  std::set<Annotation> annotations_{};
  const std::string object_name;
  const TableType object_type;
  const std::set<TableApi> table_apis_{};
  // Map of reference_type -> vector of ref_info structs
  std::map<std::string, std::vector<struct TableRefInfo>> table_ref_map;
  // key-fields map
  std::map<tdiId, std::unique_ptr<KeyFieldInfo>> table_key_map;
  // Map of common data-fields like TTL/Counter etc
  std::map<tdiId, std::unique_ptr<DataFieldInfo>> table_data_map;
  // Map of common data-fields like TTL/Counter etc with names
  std::map<std::string, DataFieldInfo *> common_data_fields_names;
  std::set<TableOperationsType> operations_type_set;
  std::set<TableAttributesType> attributes_type_set;

  bfrt::Cjson table_bfrt = *json_schema;
  tdiId table_id = table_bfrt["id"];
  std::string table_name = table_bfrt["name"];
  std::string table_type = table_bfrt["table_type"];
  size_t table_size = static_cast<unsigned int>(table_bfrt["size"]);
  key_size_t table_total_key_size;

  LOG_ERROR("Table : %s :: Type :: %s ID :: %d SIZE :: %lu", table_name.c_str(), tdi_table_type.c_str(), table_id, table_size);

  ////////////////////
  // getting key   //
  ///////////////////
  for (const auto &key : table_key_cjson.getCjsonChildVec()) {
    std::unique_ptr<KeyFieldInfo> key_field = _tdi_parse_keyParse(*key, table_id, table_name, table_type, table_size);
    if (key_field == nullptr) {
      continue;
    }
    auto elem = table_key_map.find(key_field->getId());
    if (elem == table_key_map.end()) {
      table_key_map[key_field->field_id] = (std::move(key_field));
      table_total_key_size.bytes += key_field->field_offset;
      table_total_key_size.bits  += key_field->size_bits;
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
  bfrt::Cjson table_data_cjson = table_bfrt["data"];
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
    bfrt::Cjson temp;
    for (int oneof_loop = 0; oneof_loop < oneof_size; oneof_loop++) {
      bool is_register_data_field = false;
      auto data_field = _tdi_parse_dataParse(*data_json,
                                  temp,
                                  offset,
                                  bitsize,
                                  0,
                                  0,
                                  oneof_loop,
                                  &is_register_data_field);
      tdiId data_field_id = data_field->field_id;
      if (is_register_data_field) {
        // something is not right
        LOG_ERROR(
            "%s:%d fixed table %s should not have register data field id %d",
            __func__,
            __LINE__,
            table_name.c_str(),
            data_field_id);
        continue;
      }
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
    table_bfrt["attributes"].getCjsonChildStringVec();
  for (auto const &item : attributes_v) {
    table_attribute_set.insert(item);
  }

  ////////////////////
  // getting action //
  ////////////////////
  bfrt::Cjson table_action_spec_cjson = table_bfrt["action_specs"];
  for (const auto &action : table_action_spec_cjson.getCjsonChildVec()) {
    std::unique_ptr<ActionInfo> action_info;
    bfrt::Cjson dummy;
    action_info = _tdi_parse_actionSpecsParse(*action, dummy, table_type, bfrtTable.get());
    auto elem = table_action_map.find(action_info->action_id);
    if (elem == table_action_map.end()) {
      auto act_id = action_info->action_id;
      auto act_name = action_info->name;
      table_action_map[action_info->action_id] =
          (std::move(action_info));
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
  bfrt::Cjson depends_on_cjson = table_bfrt["depends_on"];
  std::string ref_name = "other";
  for (const auto &tbl_id : depends_on_cjson.getCjsonChildVec()) {
    struct TableRefInfo ref_info;
    ref_info.id = static_cast<tdiId>(*tbl_id);
    ref_info.name = "";
    for (auto &tbl_bfrt_cjson : table_cjson_map) {
      // Check if table present in bf-rt.json if so then verify if id match
      if (tbl_bfrt_cjson.second.first != nullptr &&
          static_cast<tdiId>((*tbl_bfrt_cjson.second.first)["id"]) ==
              ref_info.id) {
        ref_info.name = tbl_bfrt_cjson.first;
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

    ref_info.tbl_hdl = static_cast<tdiId>((*context_cjson)["handle"]);
    applyMaskOnContextJsonHandle(&ref_info.tbl_hdl, ref_info.name);
    ref_info.indirect_ref = true;

    LOG_DBG("%s:%d Adding \'%s\' as \'%s\' of \'%s\'",
            __func__,
            __LINE__,
            ref_info.name.c_str(),
            ref_name.c_str(),
            bfrtTable->table_name_get().c_str());
    bfrtTable->table_ref_map[ref_name].push_back(std::move(ref_info));
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

// tdi_table creation will take three steps
//  1. create tdi_info_object creation will take three steps
//     parse device-architecture-p4_program independent json schmea file
//     a. tdi_info_obj -> tableName => table_info_obj (json in memory presentation)
//  2. create context_info_object that parse device dependent json schema file (context.json)
//  3. in implementation, it need to consider the both tdi_info and context_info to make correct choices
tdi_status tdi_info_obj_create(
    const bf_dev_id_t dev_id,
    const bfrt::ProgramConfig program_config)
{
/// A. read file form a list of schema files
 if (program_config.bfrtInfoFilePathVect_.empty()) {
   LOG_CRIT("Unable to find any TDI Json Schema File");
   return TDI_ERR;
 }
 for (auto const &tdiJsonFile : program_config.bfrtInfoFilePathVect_) {
   std::ifstream file(tdiJsonFile);
   if (file.fail()) {
     LOG_CRIT("Unable to find TDI Json File %s", tdiJsonFile.c_str());
     return TDI_ERR;
   }
   std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
   bfrt::Cjson root_cjson = bfrt::Cjson::createCjsonFromFile(content);
   bfrt::Cjson tables_cjson = root_cjson["tables"];
   for (const auto &table : tables_cjson.getCjsonChildVec()) {
/// B. parse file to form tdi_table_info object
     tdi_info_map[tdiJsonFile] = _tdi_parse_table(table);
   }
 }
 (void) dev_id;
 return TDI_OK;
}

