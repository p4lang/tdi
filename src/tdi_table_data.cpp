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
#include <algorithm>

#include <tdi/common/tdi_table_data.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const uint64_t & /*value*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const uint8_t * /*value*/,
                                 const size_t & /*size*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const std::vector<tdi_id_t> & /*arr*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const std::vector<bool> & /*arr*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const std::vector<std::string> & /*arr*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const float & /*value*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const bool & /*value*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(
    const tdi_id_t & /*field_id*/,
    std::vector<std::unique_ptr<tdi::TableData>> /*ret_vec*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::setValue(const tdi_id_t & /*field_id*/,
                                 const std::string & /*str*/) {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 uint64_t * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 const size_t & /*size*/,
                                 uint8_t * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}
tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::vector<tdi_id_t> * /*arr*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::vector<bool> * /*arr*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::vector<std::string> * /*arr*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}
tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 float * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 bool * /*value*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::vector<uint64_t> * /*arr*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(
    const tdi_id_t & /*field_id*/,
    std::vector<tdi::TableData *> * /*ret_vec*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getValue(const tdi_id_t & /*field_id*/,
                                 std::string * /*str*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::actionIdGet(tdi_id_t *act_id) const {
  *act_id = this->action_id_;
  return TDI_SUCCESS;
}

tdi_status_t TableData::dataAllocate(
    const tdi_id_t & /*container_id*/,
    std::unique_ptr<tdi::TableData> * /*data_ret*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::dataAllocateContainer(
    const tdi_id_t & /*container_id*/,
    const std::vector<tdi_id_t> & /*fields*/,
    std::unique_ptr<tdi::TableData> * /*data_ret*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}

tdi_status_t TableData::getParent(const tdi::Table **table) const {
  *table = this->table_;
  return TDI_SUCCESS;
}

// Learn object needs to override this
tdi_status_t TableData::getParent(const tdi::Learn ** /*learn*/) const {
  LOG_ERROR("%s:%d Not supported", __func__, __LINE__);
  return TDI_NOT_SUPPORTED;
}
tdi_status_t TableData::isActive(const tdi_id_t &field_id,
                                 bool *is_active) const {
  *is_active =
      std::find(active_fields_.begin(), active_fields_.end(), field_id) !=
      active_fields_.end();
  return TDI_SUCCESS;
}

}  // namespace tdi
