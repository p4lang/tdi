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
#include <tdi/common/c_frontend/tdi_table.h>
#include <stdio.h>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_init.hpp>
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_session.hpp>
#include <tdi/common/tdi_target.hpp>
#include <tdi/common/tdi_table.hpp>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>
#include <tdi/common/tdi_attributes.hpp>
#include <tdi/common/c_frontend/tdi_table_attributes.h>
#include <tdi/common/c_frontend/tdi_table_operations.h>
#include <tdi/common/tdi_table_data.hpp>
#include <tdi/common/tdi_table_key.hpp>
#include <tdi/common/tdi_operations.hpp>
#ifdef _TDI_FROM_BFRT
#include <tdi_common/tdi_pipe_mgr_intf.hpp>
#include <tdi_common/tdi_table_data_impl.hpp>
#include <tdi_common/tdi_table_impl.hpp>
#include <tdi_common/tdi_table_key_impl.hpp>
#endif
#include <tdi/common/tdi_utils.hpp>
namespace {
template <typename T>
tdi_status_t key_field_allowed_choices_get_helper(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t &field_id,
    //std::vector<std::reference_wrapper<const std::string>> *cpp_choices,
    const std::vector<std::string> **cpp_choices,
    T out_param) {
  if (out_param == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  const tdi::TableInfo *tableInfo = table->tableInfoGet();
  const tdi::KeyFieldInfo *keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *cpp_choices = &keyFieldInfo->choicesGet();
  return TDI_SUCCESS;
}

template <typename T>
tdi_status_t data_field_allowed_choices_get_helper(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t &field_id,
    //std::vector<std::reference_wrapper<const std::string>> *cpp_choices,
    const std::vector<std::string> **cpp_choices,
    T out_param,
    const tdi_id_t &action_id = 0) {
  if (out_param == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  const tdi::TableInfo *tableInfo = table->tableInfoGet();
  const tdi::DataFieldInfo *dataFieldInfo = tableInfo->dataFieldGet(field_id);
  if (!action_id) {
    *cpp_choices = &dataFieldInfo->allowedChoicesGet();
    //sts = tableDataInfo->dataFieldAllowedChoicesGet(field_id, cpp_choices);
  } else {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
    //sts = table->dataFieldAllowedChoicesGet(field_id, action_id, cpp_choices);
  }
  return TDI_SUCCESS;
}

tdi_annotation_t convert_annotation(const tdi::Annotation &annotation) {
  tdi_annotation_t annotations_c;
  annotations_c.name = annotation.name_.c_str();
  annotations_c.value = annotation.value_.c_str();
  return annotations_c;
}
}  // anonymous namespace

tdi_status_t tdi_table_id_from_handle_get(const tdi_table_hdl *table_hdl,
                                           tdi_id_t *id) {
  if (id == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  const tdi::Table *table =
      reinterpret_cast<const tdi::Table *>(table_hdl);
  const tdi::TableInfo *tableInfo = table->tableInfoGet();
  *id = tableInfo->idGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_name_get(const tdi_table_hdl *table_hdl,
                                 const char **table_name_ret) {
  if (table_name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  // We need the internal function which returns a const string ref
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  const tdi::TableInfo *tableInfo;
  tableInfo = table->tableInfoGet();
  *table_name_ret = tableInfo->nameGet().c_str();
  return TDI_SUCCESS;
}

bool tdi_generic_flag_support(void) {
  return true;
}

tdi_status_t tdi_table_entry_add(const tdi_table_hdl *table_hdl,
                                  const tdi_session_hdl *session,
                                  //const tdi_target_hdl *dev_tgt,
                                  const tdi_target_hdl *target,
                                  const tdi_flags_hdl *flags,
                                  const tdi_table_key_hdl *key,
                                  const tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  //auto &devMgr=tdi::DevMgr::getInstance();
  //tdi_status_t status=tdi:devMgr->deviceGet(dev_tgt->dev_id, device);
  return table->entryAdd(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableKey *>(key),
      *reinterpret_cast<const tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_mod(const tdi_table_hdl *table_hdl,
                                  const tdi_session_hdl *session,
                                  //const tdi_target_t *dev_tgt,
                                  //const uint64_t flags,
                                  const tdi_target_hdl *target,
                                  const tdi_flags_hdl *flags,
                                  const tdi_table_key_hdl *key,
                                  const tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryMod(
      *reinterpret_cast<const tdi::Session *>(session),
      //*dev_tgt,
      //flags,
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableKey *>(key),
      *reinterpret_cast<const tdi::TableData *>(data));
}

tdi_status_t tdi_table_default_entry_mod(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *target,
                                         const tdi_flags_hdl *flags,
                                         const tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->defaultEntryMod(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_del(const tdi_table_hdl *table_hdl,
                                  const tdi_session_hdl *session,
                                  const tdi_target_hdl *target,
                                  const tdi_flags_hdl *flags,
                                  const tdi_table_key_hdl *key) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryDel(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableKey *>(key));
}

tdi_status_t tdi_table_clear(const tdi_table_hdl *table_hdl,
                              const tdi_session_hdl *session,
                              const tdi_target_hdl *target,
                              const tdi_flags_hdl *flags) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->clear(
      *reinterpret_cast<const tdi::Session *>(session), /**dev_tgt, flags*/
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags)
      );
}

tdi_status_t tdi_table_entry_get(const tdi_table_hdl *table_hdl,
                                  const tdi_session_hdl *session,
                                  const tdi_target_hdl *target,
                                  const tdi_flags_hdl *flags,
                                  const tdi_table_key_hdl *key,
                                  tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableKey *>(key),
      reinterpret_cast<tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_get_by_handle(const tdi_table_hdl *table_hdl,
                                            const tdi_session_hdl *session,
                                            const tdi_target_hdl *target,
                                            const tdi_flags_hdl *flags,
                                            const uint32_t entry_handle,
                                            tdi_table_key_hdl *key,
                                            tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      static_cast<tdi_handle_t>(entry_handle),
      reinterpret_cast<tdi::TableKey *>(key),
      reinterpret_cast<tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_key_get(const tdi_table_hdl *table_hdl,
                                      const tdi_session_hdl *session,
                                      const tdi_target_hdl *target_in,
                                      const tdi_flags_hdl *flags,
                                      const uint32_t entry_handle,
                                      tdi_target_hdl *target_out,
                                      tdi_table_key_hdl *key) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryKeyGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target_in),
      *reinterpret_cast<const tdi::Flags *>(flags),
      static_cast<tdi_handle_t>(entry_handle),
      reinterpret_cast<tdi::Target*>(target_out),
      reinterpret_cast<tdi::TableKey *>(key));
}

tdi_status_t tdi_table_entry_handle_get(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *target,
                                         const tdi_flags_hdl *flags,
                                         const tdi_table_key_hdl *key,
                                         uint32_t *entry_handle) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryHandleGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableKey *>(key),
      entry_handle);
}

tdi_status_t tdi_table_entry_get_first(const tdi_table_hdl *table_hdl,
                                        const tdi_session_hdl *session,
                                        const tdi_target_hdl *target,
                                        const tdi_flags_hdl *flags,
                                        tdi_table_key_hdl *key,
                                        tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->entryGetFirst(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      reinterpret_cast<tdi::TableKey *>(key),
      reinterpret_cast<tdi::TableData *>(data));
}

tdi_status_t tdi_table_entry_get_next_n(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *target,
                                         const tdi_flags_hdl *flags,
                                         const tdi_table_key_hdl *key,
                                         tdi_table_key_hdl **output_keys,
                                         tdi_table_data_hdl **output_data,
                                         uint32_t n,
                                         uint32_t *num_returned) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  tdi::Table::keyDataPairs key_data_pairs;
  unsigned i = 0;
  for (i = 0; i < n; i++) {
    key_data_pairs.push_back(std::make_pair(
        reinterpret_cast<tdi::TableKey *>(output_keys[i]),
        reinterpret_cast<tdi::TableData *>(output_data[i])));
  }

  return table->entryGetNextN(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableKey *>(key),
      n,
      &key_data_pairs,
      num_returned);
}

tdi_status_t tdi_table_usage_get(const tdi_table_hdl *table_hdl,
                                  const tdi_session_hdl *session,
                                  const tdi_target_hdl *target,
                                  const tdi_flags_hdl *flags,
                                  uint32_t *count) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->usageGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      count);
}

tdi_status_t tdi_table_default_entry_set(const tdi_table_hdl *table_hdl,
                                          const tdi_session_hdl *session,
                                          const tdi_target_hdl *target,
                                          const tdi_flags_hdl *flags,
                                          const tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->defaultEntrySet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      *reinterpret_cast<const tdi::TableData *>(data));
}

tdi_status_t tdi_table_default_entry_get(const tdi_table_hdl *table_hdl,
                                          const tdi_session_hdl *session,
                                          const tdi_target_hdl *target,
                                          const tdi_flags_hdl *flags,
                                          tdi_table_data_hdl *data) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->defaultEntryGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      reinterpret_cast<tdi::TableData *>(data));
}

tdi_status_t tdi_table_default_entry_reset(const tdi_table_hdl *table_hdl,
                                            const tdi_session_hdl *session,
                                            const tdi_target_hdl *target,
                                            const tdi_flags_hdl *flags) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->defaultEntryReset(
      *reinterpret_cast<const tdi::Session *>(session), 
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags)
      );
}

tdi_status_t tdi_table_size_get(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 size_t *count) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->sizeGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      *reinterpret_cast<const tdi::Flags *>(flags),
      count);
}

tdi_status_t tdi_table_type_get(const tdi_table_hdl *table_hdl,
                                 tdi_table_type_t *table_type) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  *table_type = static_cast<tdi_table_type_t>(tableInfo->tableTypeGet());
      //reinterpret_cast<tdi::TableInfo::TableType *>(table_type));
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_has_const_default_action(
    const tdi_table_hdl *table_hdl, bool *has_const_default_action) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  *has_const_default_action = tableInfo->hasConstDefaultAction();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_num_annotations_get(const tdi_table_hdl *table_hdl,
                                            uint32_t *num_annotations) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  //tdi::AnnotationSet cpp_annotations;
  auto tableInfo = table->tableInfoGet();
  auto cpp_annotations = tableInfo->annotationsGet();
  *num_annotations = cpp_annotations.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_annotations_get(const tdi_table_hdl *table_hdl,
                                        tdi_annotation_t *annotations_c) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  const tdi::TableInfo *tableInfo=table->tableInfoGet();
  std::set<tdi::Annotation> cpp_annotations;
  //tdi_status_t sts = table->annotationsGet(&cpp_annotations);
  cpp_annotations = tableInfo->annotationsGet();
  int i = 0;
  for (const auto &annotation : cpp_annotations) {
    annotations_c[i++] = convert_annotation(annotation);
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_action_id_from_data_get(const tdi_table_data_hdl *data,
                                          tdi_id_t *id_ret) {
  auto data_obj = reinterpret_cast<const tdi::TableData *>(data);
  // disable it now, will enable it once the method is available in the code class
  return data_obj->actionIdGet(id_ret);
  return TDI_SUCCESS;
}

// Allocate APIs
tdi_status_t tdi_table_key_allocate(const tdi_table_hdl *table_hdl,
                                     tdi_table_key_hdl **key_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableKey> key_hdl;
  auto status = table->keyAllocate(&key_hdl);
  *key_hdl_ret = reinterpret_cast<tdi_table_key_hdl *>(key_hdl.release());
  return status;
}

tdi_status_t tdi_table_data_allocate(const tdi_table_hdl *table_hdl,
                                      tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  auto status = table->dataAllocate(&data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}

tdi_status_t tdi_table_action_data_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  auto status = table->dataAllocate(action_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());
  return status;
}

tdi_status_t tdi_table_data_allocate_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  auto status = table->dataAllocate(vec, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}

tdi_status_t tdi_table_action_data_allocate_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  auto status = table->dataAllocate(vec, action_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}

#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_table_data_allocate_container(
    tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  auto status = table->dataAllocateContainer(c_field_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());
  return status;
}

tdi_status_t tdi_table_action_data_allocate_container(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t action_id,
    tdi_table_data_hdl **data_hdl_ret) {
  auto tableData = reinterpret_cast<const tdi::TableData *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  auto status = table->dataAllocateContainer(c_field_id, action_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());
  return status;
}

tdi_status_t tdi_table_data_allocate_container_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto tableData = reinterpret_cast<const tdi::TableData *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  auto status = table->dataAllocateContainer(c_field_id, vec, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}

tdi_status_t tdi_table_action_data_allocate_container_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto tableData = reinterpret_cast<const tdi::TableData *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  auto status =
      tableData->dataAllocateContainer(c_field_id, vec, action_id, &data_hdl);
  *data_hdl_ret = reinterpret_cast<tdi_table_data_hdl *>(data_hdl.release());

  return status;
}
#endif

#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_table_entry_scope_attributes_allocate(
    const tdi_table_hdl *table_hdl, tdi_table_attributes_hdl **tbl_attr) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status =
      table->attributeAllocate(tdi::TableAttributesType::ENTRY_SCOPE, &attr);
  *tbl_attr = reinterpret_cast<tdi_table_attributes_hdl *>(attr.release());
  return status;
}

tdi_status_t tdi_table_idle_table_attributes_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_attributes_idle_table_mode_t idle_mode,
    tdi_table_attributes_hdl **tbl_attr) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status = table->attributeAllocate(
      tdi::TableAttributesType::IDLE_TABLE_RUNTIME,
      static_cast<tdi::TableAttributesIdleTableMode>(idle_mode),
      &attr);
  *tbl_attr = reinterpret_cast<tdi_table_attributes_hdl *>(attr.release());
  return status;
}

tdi_status_t tdi_table_port_status_notif_attributes_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_table_attributes_hdl **tbl_attr_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status = table->attributeAllocate(
      tdi::TableAttributesType::PORT_STATUS_NOTIF, &attr);
  *tbl_attr_hdl_ret =
      reinterpret_cast<tdi_table_attributes_hdl *>(attr.release());
  return status;
}

tdi_status_t tdi_table_port_stats_poll_intv_attributes_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_table_attributes_hdl **tbl_attr_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status = table->attributeAllocate(
      tdi::TableAttributesType::PORT_STAT_POLL_INTVL_MS, &attr);
  *tbl_attr_hdl_ret =
      reinterpret_cast<tdi_table_attributes_hdl *>(attr.release());
  return status;
}

tdi_status_t tdi_table_pre_device_config_attributes_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_table_attributes_hdl **tbl_attr_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status = table->attributeAllocate(
      tdi::TableAttributesType::PRE_DEVICE_CONFIG, &attr);
  *tbl_attr_hdl_ret =
      reinterpret_cast<tdi_table_attributes_hdl *>(attr.release());
  return status;
}
tdi_status_t tdi_table_dyn_hashing_attributes_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_table_attributes_hdl **tbl_attr_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status = table->attributeAllocate(
      tdi::TableAttributesType::DYNAMIC_HASH_ALG_SEED, &attr);
  *tbl_attr_hdl_ret =
      reinterpret_cast<tdi_table_attributes_hdl *>(attr.release());
  return status;
}
tdi_status_t tdi_table_dyn_key_mask_attributes_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_table_attributes_hdl **tbl_attr_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status = table->attributeAllocate(
      tdi::TableAttributesType::DYNAMIC_KEY_MASK, &attr);
  *tbl_attr_hdl_ret =
      reinterpret_cast<tdi_table_attributes_hdl *>(attr.release());
  return status;
}
tdi_status_t tdi_table_meter_byte_count_adjust_attributes_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_table_attributes_hdl **tbl_attr_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status = table->attributeAllocate(
      tdi::TableAttributesType::METER_BYTE_COUNT_ADJ, &attr);
  *tbl_attr_hdl_ret =
      reinterpret_cast<tdi_table_attributes_hdl *>(attr.release());
  return status;
}
tdi_status_t tdi_table_selector_table_update_cb_attributes_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_table_attributes_hdl **tbl_attr_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableAttributes> attr;
  auto status = table->attributeAllocate(
      tdi::TableAttributesType::SELECTOR_UPDATE_CALLBACK, &attr);
  *tbl_attr_hdl_ret =
      reinterpret_cast<tdi_table_attributes_hdl *>(attr.release());
  return status;
}
tdi_status_t tdi_table_operations_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_table_operations_mode_t op_type,
    tdi_table_operations_hdl **tbl_ops) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableOperations> ops;
  auto status = table->operationsAllocate(
      static_cast<tdi::TableOperationsType>(op_type), &ops);
  *tbl_ops = reinterpret_cast<tdi_table_operations_hdl *>(ops.release());
  return status;
}
#endif
// Reset APIs
tdi_status_t tdi_table_key_reset(const tdi_table_hdl *table_hdl,
                                  tdi_table_key_hdl **key_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->keyReset(reinterpret_cast<tdi::TableKey *>(*key_hdl_ret));
}

tdi_status_t tdi_table_data_reset(const tdi_table_hdl *table_hdl,
                                   tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->dataReset(
      reinterpret_cast<tdi::TableData *>(*data_hdl_ret));
}

tdi_status_t tdi_table_action_data_reset(const tdi_table_hdl *table_hdl,
                                          const tdi_id_t action_id,
                                          tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->dataReset(
      action_id, reinterpret_cast<tdi::TableData *>(*data_hdl_ret));
}

tdi_status_t tdi_table_data_reset_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  return table->dataReset(
      vec, reinterpret_cast<tdi::TableData *>(*data_hdl_ret));
}

tdi_status_t tdi_table_action_data_reset_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::unique_ptr<tdi::TableData> data_hdl;
  const auto vec = std::vector<tdi_id_t>(fields, fields + num_array);

  return table->dataReset(
      vec, action_id, reinterpret_cast<tdi::TableData *>(*data_hdl_ret));
}

// De-allocate APIs
tdi_status_t tdi_table_key_deallocate(tdi_table_key_hdl *key_hdl) {
  auto key = reinterpret_cast<tdi::TableKey *>(key_hdl);
  if (key == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  delete key;
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_data_deallocate(tdi_table_data_hdl *data_hdl) {
  auto data = reinterpret_cast<tdi::TableData *>(data_hdl);
  if (data == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  delete data;
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_attributes_deallocate(
    tdi_table_attributes_hdl *tbl_attr_hdl) {
  auto tbl_attr = reinterpret_cast<tdi::TableAttributes *>(tbl_attr_hdl);
  if (tbl_attr == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  delete tbl_attr;
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_operations_deallocate(
    tdi_table_operations_hdl *tbl_op_hdl) {
  auto tbl_op = reinterpret_cast<tdi::TableOperations *>(tbl_op_hdl);
  if (tbl_op == nullptr) {
    LOG_ERROR("%s:%d null param passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  delete tbl_op;
  return TDI_SUCCESS;
}

// KeyField APIs
tdi_status_t tdi_key_field_id_list_size_get(const tdi_table_hdl *table_hdl,
                                             uint32_t *num) {
  // Only here are we using TableObj rather than Table
  // since we need the hidden impl function
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  const tdi::TableInfo *tableInfo = table->tableInfoGet();
  *num = tableInfo->keyFieldIdListGet().size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_id_list_get(const tdi_table_hdl *table_hdl,
                                        tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::vector<tdi_id_t> temp_vec;
  const tdi::TableInfo *tableInfo = table->tableInfoGet();
  temp_vec = tableInfo->keyFieldIdListGet();

  for (auto it = temp_vec.begin(); it != temp_vec.end(); ++it) {
    tdi_id_t field_id = *it;
    id_vec_ret[it - temp_vec.begin()] = field_id;
  }

  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_type_get(const tdi_table_hdl *table_hdl,
                                    const tdi_id_t field_id,
                                    tdi_key_field_type_t *field_type_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);

  auto tableInfo = table->tableInfoGet();
  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  auto keyFieldType = static_cast<tdi_key_field_type_t>(keyFieldInfo->dataTypeGet());
  *field_type_ret = static_cast<tdi_key_field_type_t>(keyFieldType);
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_data_type_get(const tdi_table_hdl *table_hdl,
                                         const tdi_id_t field_id,
                                         tdi_data_type_t *field_type_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *field_type_ret = static_cast<tdi_data_type_t>(keyFieldInfo->dataTypeGet());
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_id_get(const tdi_table_hdl *table_hdl,
                                  const char *key_field_name,
                                  tdi_id_t *field_id) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto keyFieldInfo = tableInfo->keyFieldGet(key_field_name);
  *field_id = keyFieldInfo->idGet();
  if (*field_id == 0) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  } else {
    return TDI_SUCCESS;
  }
}

tdi_status_t tdi_key_field_size_get(const tdi_table_hdl *table_hdl,
                                    const tdi_id_t field_id,
                                    size_t *field_size_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *field_size_ret = keyFieldInfo->sizeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_is_ptr_get(const tdi_table_hdl *table_hdl,
                                       const tdi_id_t field_id,
                                       bool *is_ptr_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *is_ptr_ret = keyFieldInfo->isPtrGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_name_get(const tdi_table_hdl *table_hdl,
                                     const tdi_id_t field_id,
                                     const char **name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto keyFieldInfo = tableInfo->keyFieldGet(field_id);
  *name_ret = keyFieldInfo->nameGet().c_str();
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_num_allowed_choices_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    uint32_t *num_choices) {
  //std::vector<std::reference_wrapper<const std::string>> cpp_choices;
  const std::vector<std::string> *cpp_choices;
  tdi_status_t sts = ::key_field_allowed_choices_get_helper<uint32_t *>(
      table_hdl, field_id, &cpp_choices, num_choices);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num_choices = cpp_choices->size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_key_field_allowed_choices_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const char *choices[]) {
  //std::vector<std::reference_wrapper<const std::string>> cpp_choices;
  const std::vector<std::string> *cpp_choices;
  tdi_status_t sts = ::key_field_allowed_choices_get_helper<const char *[]>(
      table_hdl, field_id, &cpp_choices, choices);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  for (auto iter = cpp_choices->begin(); iter != cpp_choices->end(); iter++) {
    //choices[iter - cpp_choices.begin()] = iter->get().c_str();
    choices[iter - cpp_choices->begin()] = iter->c_str();
  }
  return TDI_SUCCESS;
}

// DataField APIs
tdi_status_t tdi_data_field_id_list_size_get(const tdi_table_hdl *table_hdl,
                                              uint32_t *num) {
  if (num == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  // Only here are we using TableObj rather than Table
  // since we need the hidden impl function
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  *num = tableInfo->dataFieldIdListGet().size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_id_list_size_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    uint32_t *num) {
  // Only here are we using TableObj rather than Table
  // since we need the hidden impl function
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  *num = tableInfo->dataFieldIdListGet(action_id).size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_list_get(const tdi_table_hdl *table_hdl,
                                      tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  std::vector<tdi_id_t> field_ids;
  field_ids = tableInfo->dataFieldIdListGet();
  for (auto it = field_ids.begin(); it != field_ids.end(); ++it) {
    tdi_id_t field_id = *it;
    id_vec_ret[it - field_ids.begin()] = field_id;
  }
  return TDI_SUCCESS;
}

#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_container_data_field_list_get(
    const tdi_table_hdl *table_hdl,
    tdi_id_t container_field_id,
    tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  //std::vector<tdi_id_t> field_ids;
  auto tableInfo = table->tableInfoGet();
  auto field_ids = tableInfo->containerDataFieldIdListGet();
  if (sts == TDI_SUCCESS) {
    for (auto it = field_ids.begin(); it != field_ids.end(); ++it) {
      tdi_id_t field_id = *it;
      id_vec_ret[it - field_ids.begin()] = field_id;
    }
  }

  return TDI_SUCCESS;
}

tdi_status_t tdi_container_data_field_list_size_get(
    const tdi_table_hdl *table_hdl,
    tdi_id_t container_field_id,
    size_t *field_list_size_ret) {
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  *field_list_size_ret = 0;
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::vector<tdi_id_t> field_ids;
  tdi_status_t sts =
      table->containerDataFieldIdListGet(container_field_id, &field_ids);
  if (sts == TDI_SUCCESS) {
    *field_list_size_ret = field_ids.size();
  }

  return sts;
}
#endif

tdi_status_t tdi_data_field_list_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table*>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  //std::vector<tdi_id_t> field_ids;
  auto field_ids = tableInfo->dataFieldIdListGet(action_id);
  for (auto it = field_ids.begin(); it != field_ids.end(); ++it) {
    tdi_id_t field_id = *it;
    id_vec_ret[it - field_ids.begin()] = field_id;
  }
  return TDI_SUCCESS;
}

#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_data_field_id_get(const tdi_table_hdl *table_hdl,
                                    const char *data_field_name,
                                    tdi_id_t *field_id_ret) {
  auto table = reinterpret_cast<const tdi::Table*>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  *field_id_ret = tableInfo->dataFieldIdGet(data_field_name);
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_id_with_action_get(
    const tdi_table_hdl *table_hdl,
    const char *data_field_name,
    const tdi_id_t action_id,
    tdi_id_t *field_id_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  *field_id_ret = tableInfo->dataFieldIdGet(data_field_name, action_id);
  return TDI_SUCCESS;
}
#endif

tdi_status_t tdi_data_field_size_get(const tdi_table_hdl *table_hdl,
                                      const tdi_id_t field_id,
                                      size_t *field_size_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *field_size_ret = dataFieldInfo->sizeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_size_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    size_t *field_size_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *field_size_ret = dataFieldInfo->sizeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_ptr_get(const tdi_table_hdl *table_hdl,
                                        const tdi_id_t field_id,
                                        bool *is_ptr_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *is_ptr_ret = dataFieldInfo->isPtrGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_ptr_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    bool *is_ptr_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *is_ptr_ret = dataFieldInfo->isPtrGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_mandatory_get(const tdi_table_hdl *table_hdl,
                                              const tdi_id_t field_id,
                                              bool *is_mandatory_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *is_mandatory_ret = dataFieldInfo->mandatoryGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_mandatory_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    bool *is_mandatory_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *is_mandatory_ret = dataFieldInfo->mandatoryGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_read_only_get(const tdi_table_hdl *table_hdl,
                                              const tdi_id_t field_id,
                                              bool *is_read_only_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *is_read_only_ret = dataFieldInfo->readOnlyGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_is_read_only_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    bool *is_read_only_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *is_read_only_ret = dataFieldInfo->readOnlyGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_name_get(const tdi_table_hdl *table_hdl,
                                      const tdi_id_t field_id,
                                      const char **name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  *name_ret = dataFieldInfo->nameGet().c_str();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_name_copy_get(const tdi_table_hdl *table_hdl,
                                           const tdi_id_t field_id,
                                           const uint32_t buf_sz,
                                           char *name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  std::string fname;
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  fname = dataFieldInfo->nameGet();

  if (fname.size() >= buf_sz) {
    LOG_ERROR("%s:%d Provided buffer too small", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  std::strncpy(name_ret, fname.c_str(), buf_sz);

  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_name_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    const char **name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *name_ret = dataFieldInfo->nameGet().c_str();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_type_get(const tdi_table_hdl *table_hdl,
                                     const tdi_id_t field_id,
                                     //tdi_data_type_t *field_type_ret) {
                                     tdi_field_data_type_e *field_type_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  //*field_type_ret = dataFieldInfo->dataTypeGet(
  //    field_id, reinterpret_cast<tdi::DataType *>(field_type_ret));
  *field_type_ret = dataFieldInfo->dataTypeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_type_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    tdi_field_data_type_e *field_type_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  *field_type_ret = dataFieldInfo->dataTypeGet();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_allowed_choices_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    uint32_t *num_choices) {
  const std::vector<std::string> *cpp_choices;
  tdi_status_t sts = ::data_field_allowed_choices_get_helper<uint32_t *>(
      table_hdl, field_id, &cpp_choices, num_choices);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num_choices = cpp_choices->size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_allowed_choices_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const char *choices[]) {
  const std::vector<std::string> *cpp_choices;
  //std::vector<std::reference_wrapper<const std::string>> cpp_choices;
  tdi_status_t sts = ::data_field_allowed_choices_get_helper<const char *[]>(
      table_hdl, field_id, &cpp_choices, choices);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  for (auto iter = cpp_choices->begin(); iter != cpp_choices->end(); iter++) {
    choices[iter - cpp_choices->begin()] = iter->c_str();
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_allowed_choices_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    uint32_t *num_choices) {
  const std::vector<std::string> *cpp_choices;
  //std::vector<std::reference_wrapper<const std::string>> cpp_choices;
  tdi_status_t sts = ::data_field_allowed_choices_get_helper<uint32_t *>(
      table_hdl, field_id, &cpp_choices, num_choices, action_id);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num_choices = cpp_choices->size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_allowed_choices_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    const char *choices[]) {
  const std::vector<std::string> *cpp_choices;
  //std::vector<std::reference_wrapper<const std::string>> cpp_choices;
  tdi_status_t sts = ::data_field_allowed_choices_get_helper<const char *[]>(
      table_hdl, field_id, &cpp_choices, choices, action_id);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  for (auto iter = cpp_choices->begin(); iter != cpp_choices->end(); iter++) {
    choices[iter - cpp_choices->begin()] = iter->c_str();
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_annotations_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    uint32_t *num_annotations) {
  //tdi::AnnotationSet annotations;
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  auto annotations = dataFieldInfo->annotationsGet();
  *num_annotations = annotations.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_annotations_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    tdi_annotation_t *annotations_c) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  auto annotations = dataFieldInfo->annotationsGet();
  int i = 0;
  for (const auto &annotation : annotations) {
    annotations_c[i++] = convert_annotation(annotation);
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_annotations_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    uint32_t *num_annotations) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  auto annotations = dataFieldInfo->annotationsGet();
  *num_annotations = annotations.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_annotations_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    tdi_annotation_t *annotations_c) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  auto annotations = dataFieldInfo->annotationsGet();
  int i = 0;
  for (const auto &annotation : annotations) {
    annotations_c[i++] = convert_annotation(annotation);
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_oneof_siblings_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    uint32_t *num_oneof_siblings) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  auto oneof_siblings = dataFieldInfo->oneofSiblingsGet();
  *num_oneof_siblings = oneof_siblings.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_oneof_siblings_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    tdi_id_t *oneof_siblings_c) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id);
  auto oneof_siblings = dataFieldInfo->oneofSiblingsGet();
  int i = 0;
  for (const auto &oneof_sibling : oneof_siblings) {
    oneof_siblings_c[i++] = oneof_sibling;
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_num_oneof_siblings_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    uint32_t *num_oneof_siblings) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  auto oneof_siblings = dataFieldInfo->oneofSiblingsGet();
  *num_oneof_siblings = oneof_siblings.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_data_field_oneof_siblings_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    tdi_id_t *oneof_siblings_c) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto dataFieldInfo = tableInfo->dataFieldGet(field_id, action_id);
  auto oneof_siblings = dataFieldInfo->oneofSiblingsGet();
  int i = 0;
  for (const auto &oneof_sibling : oneof_siblings) {
    oneof_siblings_c[i++] = oneof_sibling;
  }
  return TDI_SUCCESS;
}

#ifdef _TDI_FROM_BFRT
// disable it now, will enable it once the method is available in the code class
tdi_status_t tdi_action_id_list_size_get(const tdi_table_hdl *table_hdl,
                                          uint32_t *num) {
  if (num == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto action_ids = tableInfo->actionIdListGet();
  //std::vector<tdi_id_t> action_id;
  *num = action_ids.size();

  return TDI_SUCCESS;
}

tdi_status_t tdi_action_id_list_get(const tdi_table_hdl *table_hdl,
                                     tdi_id_t *id_vec_ret) {
  if (id_vec_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  //std::vector<tdi_id_t> action_ids;
  auto tableInfo = table->tableInfoGet();
  auto action_ids = tableInfo->actionIdListGet();
  for (auto it = action_ids.begin(); it != action_ids.end(); ++it) {
    tdi_id_t action_id = *it;
    id_vec_ret[it - action_ids.begin()] = action_id;
  }
  return TDI_SUCCESS;
}
#endif 

#ifdef _TDI_FROM_BFRT
// disable it now, will enable it once the method is available in the code class
tdi_status_t tdi_action_name_get(const tdi_table_hdl *table_hdl,
                                  const tdi_id_t action_id,
                                  const char **name_ret) {
  if (name_ret == nullptr) {
    LOG_ERROR("%s:%d Invalid arg. Please allocate mem for out param",
              __func__,
              __LINE__);
    return TDI_INVALID_ARG;
  }
  if (!table_hdl) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }

  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto actionInfo = tableInfo->actionGet(action_id);
  *name_ret = actionInfo->nameGet().c_str();

  return TDI_SUCCESS;
}

tdi_status_t tdi_action_name_to_id(const tdi_table_hdl *table_hdl,
                                    const char *action_name,
                                    tdi_id_t *action_id_ret) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  *action_id_ret = tableInfo->actionIdGet(action_name);
  return TDI_SUCCESS;
}

tdi_status_t tdi_action_id_applicable(const tdi_table_hdl *table_hdl,
                                       bool *ret_val) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  *ret_val = tableInfo->actionIdApplicable();
  return TDI_SUCCESS;
}

tdi_status_t tdi_action_num_annotations_get(const tdi_table_hdl *table_hdl,
                                             const tdi_id_t action_id,
                                             uint32_t *num_annotations) {
  //tdi::AnnotationSet annotations;
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto actionInfo = tableInfo->actionGet(action_id);
  auto annotations = actionInfo->annotationsGet();
  *num_annotations = annotations.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_action_annotations_get(const tdi_table_hdl *table_hdl,
                                         const tdi_id_t action_id,
                                         tdi_annotation_t *annotations_c) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto actionInfo = tableInfo->actionGet(action_id);
  auto annotations = actionInfo->annotationsGet();
  int i = 0;
  for (const auto &annotation : annotations) {
    annotations_c[i++] = convert_annotation(annotation);
  }
  return TDI_SUCCESS;
}
#endif

#ifdef __TDI_FROM_BFRT
tdi_status_t tdi_table_attributes_set(
    const tdi_table_hdl *table_hdl,
    const tdi_session_hdl *session,
    const tdi_target_hdl *target,
    const uint64_t flags,
    const tdi_table_attributes_hdl *tbl_attr) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->tableAttributesSet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      flags,
      *reinterpret_cast<const tdi::TableAttributes *>(tbl_attr));
}

tdi_status_t tdi_table_attributes_get(const tdi_table_hdl *table_hdl,
                                       const tdi_session_hdl *session,
                                       const tdi_target_hdl *target,
                                       const uint64_t flags,
                                       tdi_table_attributes_hdl *tbl_attr) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return table->tableAttributesGet(
      *reinterpret_cast<const tdi::Session *>(session),
      *reinterpret_cast<const tdi::Target *>(target),
      flags,
      reinterpret_cast<tdi::TableAttributes *>(tbl_attr));
}
#endif

tdi_status_t tdi_table_num_attributes_supported(
    const tdi_table_hdl *table_hdl, uint32_t *num_attributes) {
  if (table_hdl == nullptr || num_attributes == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  //std::set<tdi::TableAttributesType> type_set;
  auto type_set = tableInfo->attributesSupported();
  *num_attributes = type_set.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_attributes_supported(
    const tdi_table_hdl *table_hdl,
    tdi_table_attributes_type_t *attributes,
    uint32_t *num_returned) {
  if (table_hdl == nullptr || num_returned == nullptr ||
      attributes == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  //std::set<tdi::TableAttributesType> type_set;
  auto type_set = tableInfo->attributesSupported();
  int i = 0;
  for (const auto &iter : type_set) {
    attributes[i] = static_cast<tdi_table_attributes_type_t>(iter);
    i++;
  }
  *num_returned = i;
  return TDI_SUCCESS;
}

#ifdef _TDI_FROM_BFRT
tdi_status_t tdi_table_operations_execute(
    const tdi_table_hdl *table_hdl,
    const tdi_table_operations_hdl *tbl_ops) {
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  return (table->tableOperationsExecute(
      reinterpret_cast<const tdi::TableOperations *>(tbl_ops)));
}
#endif

tdi_status_t tdi_table_num_operations_supported(
    const tdi_table_hdl *table_hdl, uint32_t *num_operations) {
  if (table_hdl == nullptr || num_operations == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  //std::set<tdi::TableOperationsType> type_set;
  auto type_set = tableInfo->operationsSupported();
  *num_operations = type_set.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_operations_supported(
    const tdi_table_hdl *table_hdl,
    tdi_table_operations_mode_t *operations,
    uint32_t *num_returned) {
  if (table_hdl == nullptr || num_returned == nullptr ||
      operations == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  //std::set<tdi::TableOperationsType> type_set;
  auto tableInfo = table->tableInfoGet();
  auto type_set = tableInfo->operationsSupported();
  int i = 0;
  for (const auto &iter : type_set) {
    operations[i] = static_cast<tdi_table_operations_mode_t>(iter);
    i++;
  }
  *num_returned = i;
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_num_api_supported(const tdi_table_hdl *table_hdl,
                                          uint32_t *num_apis) {
  if (table_hdl == nullptr || num_apis == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto type_set = tableInfo->apiSupportedGet();
  *num_apis = type_set.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_table_api_supported(const tdi_table_hdl *table_hdl,
                                      tdi_table_api_type_t *apis,
                                      uint32_t *num_returned) {
  if (table_hdl == nullptr || num_returned == nullptr || apis == nullptr) {
    LOG_ERROR("%s:%d Invalid arg", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto table = reinterpret_cast<const tdi::Table *>(table_hdl);
  auto tableInfo = table->tableInfoGet();
  auto type_set = tableInfo->apiSupportedGet();
  int i = 0;
  for (const auto &iter : type_set) {
    apis[i] = static_cast<tdi_table_api_type_t>(iter);
    i++;
  }
  *num_returned = i;
  return TDI_SUCCESS;
}
