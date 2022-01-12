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
/** @file tdi_table.hpp
 *
 *  @brief Contains TDI Table APIs
 */
#ifndef _TDI_TABLE_HPP
#define _TDI_TABLE_HPP

#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>

#include <tdi/common/tdi_defs.h>
#include <../src/tdi_utils.hpp>
//#include <tdi/common/tdi_target.hpp>
//#include <tdi/common/tdi_session.hpp>
//#include <tdi/common/tdi_table_data.hpp>
//#include <tdi/common/tdi_table_key.hpp>
//#include <tdi/common/tdi_attributes.hpp>
//#include <tdi/common/tdi_operations.hpp>

namespace tdi {

// Forward declarations
class TableInfo;
class ActionInfo;
class KeyFieldInfo;
class DataFieldInfo;
class TableRefInfo;

/**
 * @brief Class for Annotations. Contains 2 strings to uniquely identify an
 * annotation. These annotations can be on a Table, Action, Key or Data Field
 *
 * Possible list of annotations are below. This list is not exhaustive since
 * pragmas can be annotations too like alpm_partition ones which will vary
 * according to the params given ->
 *  1. ("$tdi_field_class","register_data") If a Data field is a register data.
 *      Register data fields are to be set using one value but while
 *      tableEntryGet, this field returns a vector(one value for each field)
 *  2. ("$tdi_port_type_class","dev_port") If the data field is a dev_port.
 *  3. ("$tdi_port_type_class","port_hdl_conn_id") If the data field is a port
 *      handle conn ID.
 *  4. ("$tdi_port_type_class","port_hdl_chnl_id") If the data field is a port
 *      handle channel ID.
 *  5. ("$tdi_port_type_class","port_name") If the data field is a port name.
 *  6. ("$tdi_port_type_class","fp_port") If the data field is a front panel
 *      port.
 *  7. ("isFieldSlice","true") If a Data field is a slice of bigger field.
 *  8. ("@defaultonly","") If an action is to be default only
 *  9. ("$tdi_field_imp_level", "level") Importance level of a field. All
 *fields
 *     start off with an importance level of 1
 */
class Annotation {
 public:
  Annotation(std::string name, std::string value) : name_(name), value_(value) {
    full_name_ = name_ + "." + value_;
  };
  bool operator<(const Annotation &other) const;
  bool operator==(const Annotation &other) const;
  bool operator==(const std::string &other_str) const;
  tdi_status_t fullNameGet(std::string *fullName) const;
  const std::string name_{""};
  const std::string value_{""};
  struct Less {
    bool operator()(const Annotation &lhs, const Annotation &rhs) const {
      return lhs < rhs;
    }
  };

 private:
  std::string full_name_{""};
};

/**
 * @brief We wrap the annotation in a reference wrapper because we want to send
 * the actual object to the API user and not a copy.
 */
using AnnotationSet =
    std::set<std::reference_wrapper<const Annotation>, Annotation::Less>;

/**
 * @brief In memory representation of tdi.json
 */
class TableInfo {
 public:
  /**
   * @brief Get name of the table
   *
   * @param[out] name Name of the table
   *
   * @return Status of the API call
   */
  tdi_status_t tableNameGet(std::string *name) const;

  /**
   * @brief Get ID of the table
   *
   * @param[out] id ID of the table
   *
   * @return Status of the API call
   */
  tdi_status_t tableIdGet(tdi_id_t *id) const;

  /**
   * @brief The type of the table
   *
   * @param[out] table_type Type of the table
   *
   * @return Status of the API call
   */
  tdi_status_t tableTypeGet(tdi_table_type_e *table_type) const;

  /**
   * @brief Size of a table acc to tdi.json
   *
   * @param[out] size Number of total available entries
   *
   * @return Status of the API call
   */
  tdi_status_t tableSizeGet(size_t *size) const;
  /**
   * @brief Get whether this table has a const default action
   *
   * @param[out] has_const_default_action If default action is const
   *
   * @return Status of the API call
   */
  tdi_status_t tableHasConstDefaultAction(bool *has_const_default_action) const;

  /**
   * @brief Get whether this table is a const table
   *
   * @param[out] is_const If table is const
   *
   * @return Status of the API call
   */
  tdi_status_t tableIsConst(bool *is_const) const;

  /**
   * @brief Get a set of annotations on a Table
   *
   * @param[out] annotations Set of annotations on a Table
   *
   * @return Status of the API call
   */
  tdi_status_t tableAnnotationsGet(AnnotationSet *annotations) const;

  using TableApiSet =
      std::set<std::reference_wrapper<const tdi_table_api_type_e>>;
  /**
   * @brief Get a set of APIs which are supported by this table.
   *
   * @param[out] tableApiset The set of APIs which are supported by this table
   *
   * @return Status of the API call
   */
  tdi_status_t tableApiSupportedGet(TableApiSet *tableApis) const;

  /**
   * @brief Get the set of supported attributes
   *
   * @param[out] type_set Set of the supported Attributes
   *
   * @return Status of the API call
   */
  tdi_status_t tableAttributesSupported(
      std::set<tdi_attributes_type_e> *type_set) const;
  /**
   * @brief Get set of supported Operations
   *
   * @param[out] type_set Set of supported Operations
   *
   * @return Status of the API call
   */
  tdi_status_t tableOperationsSupported(
      std::set<tdi_operations_type_e> *type_set) const;
  /**
   * @brief Get a vector of Key field IDs
   *
   * @param[out] id Vector of Key field IDs
   *
   * @return Status of the API call
   */
  tdi_status_t keyFieldIdListGet(std::vector<tdi_id_t> *id) const;

  /**
   * @brief Get field ID of Key Field from name
   *
   * @param[in] name Key Field name
   * @param[out] field_id Field ID
   *
   * @return Status of the API call
   */
  tdi_status_t keyFieldIdGet(const std::string &name, tdi_id_t *field_id) const;

  /**
   * @brief Get Key Field
   *
   * @param[in] id Key Field ID
   * @param[out] key_field_info KeyFieldInfo object
   *
   * @return Status of the API call
   */
  tdi_status_t keyFieldGet(const tdi_id_t &field_id, const KeyFieldInfo *key_field_info) const;

  /**
   * @brief Get vector of DataField IDs. Only applicable for tables
   * without Action IDs
   *
   * @param[out] id Vector of IDs
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldIdListGet(std::vector<tdi_id_t> *id) const;

  /**
   * @brief Get vector of DataField IDs for a particular action. If action
   * doesn't exist, then common fields list is returned.
   *
   * @param[in] action_id Action ID
   * @param[out] id Vector of IDs
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldIdListGet(const tdi_id_t &action_id,
                                  std::vector<tdi_id_t> *id) const;
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
   * @brief Get the field ID of a Data Field from a name
   *
   * @param[in] name Name of a Data field
   * @param[in] action_id Action ID
   * @param[out] field_id Field ID
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldIdGet(const std::string &name,
                              const tdi_id_t &action_id,
                              tdi_id_t *field_id) const;

  /**
   * @brief Get the data Field info object from tdi_id.
   *
   * @param[in] id id of a Data field
   * @param[out] data_field_info DataFieldInfo object
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldGet(const tdi_id_t field_id, const DataFieldInfo **data_field_info) const;

  /**
   * @brief Get the data Field info object from tdi_id.
   *
   * @param[in] id id of a Data field
   * @param[in] action_id Action ID
   * @param[out] data_field_info DataFieldInfo object
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldGet(const tdi_id_t field_id,
                            const tdi_id_t &action_id,
                            const DataFieldInfo **data_field_info) const;

  /**
   * @brief Get Action ID from Name
   *
   * @param[in] name Action Name
   * @param[out] action_id Action ID
   *
   * @return Status of the API call
   */
  tdi_status_t actionIdGet(const std::string &name, tdi_id_t *action_id) const;
  /**
   * @brief Get vector of Action IDs
   *
   * @param[out] action_id Vector of Action IDs
   *
   * @return Status of the API call
   */
  tdi_status_t actionIdListGet(std::vector<tdi_id_t> *action_id) const;

  /**
   * @brief Get ActionInfo object from tdi_id of action (action_id)
   *
   * @param[in] action_id tdi_id of Action
   * @param[out] action_info ActionInfo object
   *
   * @return Status of the API call
   */
  tdi_status_t actionGet(const tdi_id_t &action_id, const ActionInfo **action_info) const;

  /**
   * @brief Are Action IDs applicable for this table
   *
   * @retval True : If Action ID applicable
   * @retval False : If not
   *
   */
  bool actionIdApplicable() const;
  const std::string &table_name_get() const { return table_name_; }

 private:
  std::string table_name_;
  tdi_table_type_e table_type_;
  tdi_id_t table_id_;
  size_t table_size_;
  bool has_const_default_action_{false};
  bool is_const_table_{false};
  std::map<tdi_id_t, std::unique_ptr<KeyFieldInfo>>  table_key_map_;
  std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> table_data_map_;
  std::map<tdi_id_t, std::unique_ptr<ActionInfo>>    table_action_map_;
  std::map<tdi_id_t, std::unique_ptr<TableRefInfo>>  table_ref_map_;
  std::set<tdi_table_api_type_e> table_apis_{};
  std::set<tdi_operations_type_e> operations_type_set_;
  std::set<tdi_attributes_type_e> attributes_type_set_;
  std::set<Annotation> annotations_{};
};

/* class to keep info regarding a reference to another tableInfo */
class TableRefInfo {
  tdi_id_t ref_id_;
  std::string ref_name_;
};

class KeyFieldInfo {
 public:
  /**
   * @name Key APIs
   * @{
   */

  /**
   * @brief Get match type of Key Field
   *
   * @param[out] match_type Match Type (Exact/Ternary/LPM ... )
   *
   * @return Status of the API call
   */
  tdi_status_t keyFieldMatchTypeGet(tdi_match_type_e *field_type) const;

  /**
   * @brief Get data type of Key Field
   *
   * @param[out] data_type Field Type (uint64, float, string)
   *
   * @return Status of the API call
   */
  tdi_status_t keyFieldDataTypeGet(tdi_field_data_type_e *data_type) const;

  /**
   * @brief Get field size
   *
   * @param[out] size Field Size in bits
   *
   * @return Status of the API call
   */
  tdi_status_t keyFieldSizeGet(size_t *size) const;

  /**
   * @brief Get whether Key Field is of type ptr or not. If the field is
   * of ptr type, then only ptr sets/gets are applicable on the field. Else
   * both the ptr versions and the uint64_t versions work
   *
   * @param[out] is_ptr Boolean type indicating whether Field is of type ptr
   *
   * @return Status of the API call
   */
  tdi_status_t keyFieldIsPtrGet(bool *is_ptr) const;

  /**
   * @brief Get name of field
   *
   * @param[out] name Field name
   *
   * @return Status of the API call
   */
  tdi_status_t keyFieldNameGet(std::string *name) const;

  /**
   * @brief Get a list of all the allowed values that a particular field can
   * have. This API is only for fields with string type. If the returned
   * vector is empty, it indicates that the allowed choices have not been
   * published in tdi json
   *
   * @param[out] choices Vector of const references of the values that are
   * allowed for this field
   *
   * @return Status of the API call
   */
  tdi_status_t keyFieldAllowedChoicesGet(
      std::vector<std::reference_wrapper<const std::string>> *choices) const;

  /** @} */  // End of group Key
 private:
  // data for key field info
  tdi_id_t field_id_;
  size_t size_bits_;
  tdi_match_type_e match_type_;
  tdi_field_data_type_e data_type_;
  std::string name_;
  bool mandatory_;
  bool read_only_;
  std::vector<std::string> enum_choices_;
  std::set<std::string> annotations_;
  // Default value for this data field
  uint64_t default_value_;
  float default_fl_value_;
  std::string default_str_value_;
  // Flag to indicate if this is a field slice or not
  bool is_field_slice_{false};
  bool is_ptr_;
  // A flag to indicate this is a match priority field
  bool match_priority_;
};  // class KeyFieldInfo

class DataFieldInfo {
 public:
  /**
   * @brief Get vector of DataField IDs for a container's field id.
   *
   * @param[out] id Vector of IDs
   *
   * @return Status of the API call
   */
  tdi_status_t containerDataFieldIdListGet(std::vector<tdi_id_t> *id) const;

  /**
   * @brief Get the Size of a field.
   * For container fields this function will return number
   * of elements inside the container.
   *
   * @param[out] size Size of the field in bits
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldSizeGet(size_t *size) const;

  /**
   * @brief Get whether a field is a ptr type.
   * Only the ptr versions of setValue/getValue will work on fields
   * for which this API returns true
   *
   * @param[out] is_ptr Boolean value indicating if it is ptr type
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldIsPtrGet(bool *is_ptr) const;

  /**
 * @brief Get whether a field is mandatory.
 *
 * @param[out] is_mandatory Boolean value indicating if it is mandatory
 *
 * @return Status of the API call
 */
  tdi_status_t dataFieldMandatoryGet(bool *is_mandatory) const;

  /**
  * @brief Get whether a field is ReadOnly.
  *
  * @param[out] is_read_only Boolean value indicating if it is ReadOnly
  *
  * @return Status of the API call
  */
  tdi_status_t dataFieldReadOnlyGet(bool *is_read_only) const;

  /**
  * @brief Get the IDs of oneof siblings of a field. If a field is part of a
  * oneof , for example, consider $ACTION_MEMBER_ID and $SELECTOR_GROUP_ID. then
  * this API will return [field_ID($ACTION_MEMBER_ID)] for $SELECTOR_GROUP_ID.
  *
  *
  * @param[out] oneof_siblings Set containing field IDs of oneof siblings
  *
  * @return Status of the API call
  */
  tdi_status_t dataFieldOneofSiblingsGet(
      std::set<tdi_id_t> *oneof_siblings) const;

  /**
   * @brief Get the Name of a field.
   *
   * @param[out] name Name of the field
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldNameGet(std::string *name) const;

  /**
   * @brief Get the Data type of a field (INT/BOOL/ENUM/INT_ARR/BOOL_ARR)
   *
   * @param[out] type Data type of a data field
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldDataTypeGet(tdi_field_data_type_e *type) const;

  /**
   * @brief Get a list of all the allowed values that a particular field can
   * have. This API is only for fields with string type. If the returned
   * vector is empty, it indicates that the allowed choices have not been
   * published in tdi json
   *
   * @param[out] choices Vector of const references of the values that are
   * allowed for this field
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldAllowedChoicesGet(
      std::vector<std::reference_wrapper<const std::string>> *choices) const;

  /**
   * @brief Get a set of annotations on a data field
   *
   * @param[out] annotations Set of annotations on a data field
   *
   * @return Status of the API call
   */
  tdi_status_t dataFieldAnnotationsGet(AnnotationSet *annotations) const;

  /** @} */  // End of group Data
private:
  std::string name_;
  tdi_id_t field_id_;
  tdi_id_t action_id_;
  size_t size_;
  std::string data_type_;
  // Vector of allowed choices for this field
  std::vector<std::string> enum_choices_;
  // Default value for this data field
  uint64_t default_value_;
  float default_fl_value_;
  std::string default_str_value_;
  bool mandatory_;
  bool read_only_;
  bool container_valid_{false};
  /* Map of Objects within container */
  std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> container_;
  std::map<std::string, tdi_id_t> container_names_;
  std::set<std::string> annotations_;
  std::set<tdi_id_t> oneof_siblings_;
};

// Action ID APIs
class ActionInfo {
 public:
  /**
   * @name Action ID APIs
   * @{
   */
  /**
   * @brief Get Action ID
   *
   * @param[out] action_id Action ID
   *
   * @return Status of the API call
   */
  tdi_status_t actionIdGet(tdi_id_t *action_id) const;

  /**
   * @brief Get Action Name
   *
   * @param[out] name Action Name
   *
   * @return Status of the API call
   */
  tdi_status_t actionNameGet(std::string *name) const;

  /**
   * @brief Get a set of annotations for this action.
   *
   * @param[out] annotations Set of annotations
   *
   * @return Status of the API call
   */
  tdi_status_t actionAnnotationsGet(AnnotationSet *annotations) const;
  /** @} */  // End of group Action IDs
 private:
  tdi_id_t action_id_;
  std::string name_;
  // Map of table_data_fields
  std::map<tdi_id_t, std::unique_ptr<DataFieldInfo>> data_fields_;
  // Map of table_data_fields with names
  std::map<std::string, tdi_id_t> data_fields_names_;
  std::set<std::string> annotations_;
};

}  // tdi

#endif  // _TDI_TABLE_HPP
