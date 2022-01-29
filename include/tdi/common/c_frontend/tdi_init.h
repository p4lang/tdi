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
/** @file tdi_init.h
 *
 *  @brief C frontend for init
 */
#ifndef _TDI_INIT_H_
#define _TDI_INIT_H_

// tdi includes
#include <tdi/common/tdi_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the TdiInfo object corresponding to the (device_id,
 * program name)
 *
 * @param[in] dev_id Device ID
 * @param[in] prog_name Name of the P4 program
 * @param[out] tdi_info TdiInfo Obj associated with the Device
 *    and the program name.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_info_get(const tdi_dev_id_t dev_id,
                           const char *prog_name,
                           const tdi_info_hdl **info_hdl_ret);

/**
 * @brief Get size of list of all device IDs currently added
 *
 * @param[out] num Size of list of device IDs
 * IDs that are present.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_num_device_id_list_get(uint32_t *num);
/**
 * @brief Get a list of all device IDs currently added
 *
 * @param[out] device_id_list Set Contains list of device
 * IDs that are present.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_device_id_list_get(tdi_dev_id_t *device_id_list);

/**
 * @brief Get size of list of loaded p4 program names on a particular device
 *
 * @param[in] dev_id Device ID
 * @param[out] num_names Size of the list
 *
 * @return Status of the API call
 */
tdi_status_t tdi_num_p4_names_get(const tdi_dev_id_t dev_id, int *num_names);

/**
 * @brief Get a list of loaded p4 program names on a particular device
 *
 * @param[in] dev_id Device ID
 * @param[out] p4_names Array of char*. User needs to allocate the array of ptrs
 * to pass as input to this function.
 *
 * @return Status of the API call
 */
tdi_status_t tdi_p4_names_get(const tdi_dev_id_t dev_id, const char **p4_names);

/**
 * @brief TDI Module Init API. This function needs to be called to
 * initialize TDI. Some specific managers can be specified to be skipped
 * TDI initialization. This allows TDI session layer to not know about these
 * managers.
 * Recommendation is not to skip any unless user knows exactly
 * what they are doing.
 *
 * @param[in] pkt_mgr_skip Skip Packet mgr
 * @param[in] mc_mgr_skip Skip Multicast mgr
 * @param[in] port_mgr_skip Skip Port mgr
 * @param[in] traffic_mgr_skip Skip Traffic mgr
 * @return Status of the API call
 */
tdi_status_t tdi_module_init(bool pkt_mgr_skip,
                              bool mc_mgr_skip,
                              bool port_mgr_skip,
                              bool traffic_mgr_skip);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_INIT_H
