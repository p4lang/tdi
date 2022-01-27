/*******************************************************************************
 * BAREFOOT NETWORKS CONFIDENTIAL & PROPRIETARY
 *
 * Copyright (c) 2017-2021 Barefoot Networks, Inc.

 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property of
 * Barefoot Networks, Inc. and its suppliers, if any. The intellectual and
 * technical concepts contained herein are proprietary to Barefoot Networks,
 * Inc.
 * and its suppliers and may be covered by U.S. and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from
 * Barefoot Networks, Inc.
 *
 * No warranty, explicit or implicit is provided, unless granted under a
 * written agreement with Barefoot Networks, Inc.
 *
 * $Id: $
 *
 ******************************************************************************/
/** @file tdi_table_operations.h
 *
 * @brief Contains TDI Table operations APIs
 */
#ifndef _TDI_TABLE_OPERATIONS_H
#define _TDI_TABLE_OPERATIONS_H

#include <tdi/common/c_frontend/tdi_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Table Operations Type
 */
typedef enum tdi_table_operations_mode_ {
  /** Update sw value of all counters with the value in hw.
     Applicable on Counters or MATs with direct counters */
  TDI_COUNTER_SYNC = 0,
  /** Update sw value of all registers with the value in hw.
     Applicable on Registers or MATs with direct registers */
  TDI_REGISTER_SYNC = 1,
  /** Update sw value of all hit state o entries with the actual
     hw status. Applicable MATs with idletimeout POLL mode*/
  TDI_HIT_STATUS_UPDATE = 2,
  TDI_INVALID
} tdi_table_operations_mode_t;

/**
 * @brief Register Sync Callback
 * @param[in] dev_tgt Device target
 * @param[in] cookie User registered optional cookie
 */
typedef void (*tdi_register_sync_cb)(tdi_target_t *, void *);

/**
 * @brief Counter Sync Callback
 * @param[in] dev_tgt Device target
 * @param[in] cookie User registered optional cookie
 */
typedef void (*tdi_counter_sync_cb)(tdi_target_t *, void *);

/**
 * @brief Hit State Update Callback
 * @param[in] dev_tgt Device target
 * @param[in] cookie User registered optional cookie
 */
typedef void (*tdi_hit_state_update_cb)(tdi_target_t *, void *);

/**
 * @brief Set Register sync callback.
 *
 * @param[in] tbl_ops Table operations handle
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] callback Register sync callback
 * @param[in] cookie User cookie
 *
 * @return Status of the API call
 */
tdi_status_t tdi_operations_register_sync_set(
    tdi_table_operations_hdl *tbl_ops,
    const tdi_session_hdl *session,
    const tdi_target_t *dev_tgt,
    const tdi_register_sync_cb callback,
    const void *cookie);

/**
 * @brief Set Counter sync callback.
 *
 * @param[in] tbl_ops Table operations handle
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] callback Counter sync callback
 * @param[in] cookie User cookie
 *
 * @return Status of the API call
 */
tdi_status_t tdi_operations_counter_sync_set(
    tdi_table_operations_hdl *tbl_ops,
    const tdi_session_hdl *session,
    const tdi_target_t *dev_tgt,
    const tdi_counter_sync_cb callback,
    const void *cookie);

/**
 * @brief Set Hit State Update callback.
 *
 * @param[in] tbl_ops Table operations handle
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] callback Hit State update sync callback
 * @param[in] cookie User cookie
 *
 * @return Status of the API call
 */
tdi_status_t tdi_operations_hit_state_update_set(
    tdi_table_operations_hdl *tbl_ops,
    const tdi_session_hdl *session,
    const tdi_target_t *dev_tgt,
    const tdi_hit_state_update_cb callback,
    const void *cookie);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_TABLE_OPERATIONS_H
