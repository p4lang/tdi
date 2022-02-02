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
#include <iterator>
#include <vector>
#include <algorithm>
// tdi includes
#include <tdi/common/tdi_init.hpp>
// c_frontend includes
#include <tdi/common/c_frontend/tdi_init.h>

tdi_status_t tdi_info_get(const tdi_dev_id_t dev_id,
                           const char *prog_name,
                           const tdi_info_hdl **info_hdl_ret) {
  tdi_status_t sts = TDI_SUCCESS;
  const tdi::TdiInfo *tdiInfo = nullptr;
  std::string program_name(prog_name);
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  // From the dev_id, to get devObj
  const tdi::Device *devObj = nullptr;
  sts = devMgrObj.deviceGet(dev_id, &devObj);
  if (sts != TDI_SUCCESS) {
    return sts;
  }

  sts = devObj->tdiInfoGet(program_name, &tdiInfo);
  if (sts != TDI_SUCCESS) {
    return sts;
  }

  *info_hdl_ret = reinterpret_cast<const tdi_info_hdl *>(tdiInfo);
  return TDI_SUCCESS;
}

tdi_status_t tdi_num_device_id_list_get(uint32_t *num) {
  std::set<tdi_dev_id_t> device_id_list;
  tdi_status_t sts = TDI_SUCCESS;
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  sts = devMgrObj.deviceIdListGet(&device_id_list);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num = device_id_list.size();
  return TDI_SUCCESS;
}

tdi_status_t tdi_device_id_list_get(tdi_dev_id_t *device_id_list_out) {
  std::set<tdi_dev_id_t> device_id_list;
  tdi_status_t sts = TDI_SUCCESS;
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  sts = devMgrObj.deviceIdListGet(&device_id_list);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  int i = 0;
  for (const auto &id : device_id_list) {
    device_id_list_out[i++] = id;
  }
  return TDI_SUCCESS;
}

tdi_status_t tdi_num_p4_names_get(const tdi_dev_id_t dev_id, int *num_names) {
  tdi_status_t sts = TDI_SUCCESS;
  std::vector<std::reference_wrapper<const std::string>> p4_names;
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  // From the dev_id, to get devObj
  const tdi::Device *devObj = nullptr;
  sts = devMgrObj.deviceGet(dev_id, &devObj);
  if (sts != TDI_SUCCESS) {
    return sts;
  }

  sts = devObj->p4NamesGet(p4_names);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  *num_names = static_cast<int>(p4_names.size());
  return sts;
}

tdi_status_t tdi_p4_names_get(const tdi_dev_id_t dev_id,
                               const char **prog_names) {
  tdi_status_t sts = TDI_SUCCESS;
  std::vector<std::reference_wrapper<const std::string>> p4_names;
  tdi::DevMgr &devMgrObj = tdi::DevMgr::getInstance();
  // From the dev_id, to get devObj
  const tdi::Device *devObj = nullptr;
  sts = devMgrObj.deviceGet(dev_id, &devObj);
  if (sts != TDI_SUCCESS) {
    return sts;
  }
  sts = devObj->p4NamesGet(p4_names);

  /*
   * Iterate through the p4 names and fill the name string array.
   * We use iterators rather than auto so we can use iterator
   * arithmetic to increment our array index rather than a variable.
   * We use the reference_wrapper because we iterate through a list
   * of string references, not string values.
   */
  std::vector<std::reference_wrapper<const std::string>>::iterator it;
  for (it = p4_names.begin(); it != p4_names.end(); ++it) {
    prog_names[it - p4_names.begin()] = it->get().c_str();
  }
  return sts;
}
