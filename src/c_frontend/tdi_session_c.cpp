/*******************************************************************************
 * BAREFOOT NETWORKS CONFIDENTIAL & PROPRIETARY
 *
 * Copyright (c) 2017-2018 Barefoot Networks, Inc.

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
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
//#include <tdi/common/c_frontend/tdi_table.h>
#include <tdi/common/c_frontend/tdi_session.h>

#ifdef __cplusplus
}
#endif
#include <tdi/common/tdi_init.hpp>
#include <tdi/common/tdi_session.hpp>
//#include <tdi_common/tdi_session_impl.hpp>
#include <tdi/common/tdi_utils.hpp>

#include "tdi_state_c.hpp"

tdi_status_t tdi_session_create(const tdi_device_hdl *device_hdl, tdi_session_hdl **session) {
  auto sess = reinterpret_cast <std::shared_ptr<tdi::Session> *>(session);
  auto device = reinterpret_cast <const tdi::Device *> (device_hdl);
  if (device == nullptr) {
    LOG_ERROR("%s:%d Unable to create session", __func__, __LINE__);
    return TDI_UNEXPECTED;
  }
  device->createSession(sess);
  if (*sess == nullptr) {
    LOG_ERROR("%s:%d Unable to create session", __func__, __LINE__);
    return TDI_UNEXPECTED;
  }

  // insert the shared_ptr in the state map
  auto &c_state = tdi::tdi_c::TdiCFrontEndSessionState::getInstance();
  c_state.insertShared(*sess);
  return TDI_SUCCESS;
}

tdi_status_t tdi_session_destroy(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  auto status = sess->destroy();
  if (status != TDI_SUCCESS) {
    LOG_ERROR("%s:%d Failed to destroy session", __func__, __LINE__);
    return status;
  }
  // remove the shared_ptr from the state map. It will get destroyed
  // automatically if not already
  // wdai: temperate disable for compile
  auto &c_state = tdi::tdi_c::TdiCFrontEndSessionState::getInstance();
  c_state.removeShared(sess);
  return TDI_SUCCESS;
}

tdi_id_t tdi_sess_handle_get(const tdi_session_hdl *const session, tdi_mgr_type_e& mgr_type) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<const tdi::Session *>(session);
  return sess->handleGet(mgr_type);
}
#ifdef _TDI_FROM_BFRT
tdi_id_t tdi_pre_sess_handle_get(const tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<const tdi::Session *>(session);
  return sess->preSessHandleGet();
}
#endif 
bool tdi_session_is_valid(const tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return false;
  }
  auto sess = reinterpret_cast<const tdi::Session *>(session);
  return sess->isValid();
}

tdi_status_t tdi_session_complete_operations(
    const tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return false;
  }
  auto sess = reinterpret_cast<const tdi::Session *>(session);
  return sess->completeOperations();
}

tdi_status_t tdi_begin_batch(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->beginBatch();
}

tdi_status_t tdi_flush_batch(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->flushBatch();
}

tdi_status_t tdi_end_batch(tdi_session_hdl *const session,
                            bool hwSynchronous) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->endBatch(hwSynchronous);
}

tdi_status_t tdi_begin_transaction(tdi_session_hdl *const session,
                                    bool isAtomic) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->beginTransaction(isAtomic);
}

tdi_status_t tdi_verify_transaction(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->verifyTransaction();
}

tdi_status_t tdi_commit_transaction(tdi_session_hdl *const session,
                                     bool hwSynchronous) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->commitTransaction(hwSynchronous);
}

tdi_status_t tdi_abort_transaction(tdi_session_hdl *const session) {
  if (session == nullptr) {
    LOG_ERROR("%s:%d Session Handle passed is null", __func__, __LINE__);
    return false;
  }
  auto sess = reinterpret_cast<tdi::Session *>(session);
  return sess->abortTransaction();
}
