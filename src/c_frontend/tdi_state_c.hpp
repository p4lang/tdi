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
#ifndef _TDI_STATE_C_HPP
#define _TDI_STATE_C_HPP

#include <mutex>
#include <unordered_map>

namespace tdi {
namespace tdi_c {

class TdiCFrontEndSessionState {
 public:
  // To get the singleton instance. Threadsafe
  static TdiCFrontEndSessionState &getInstance();

  // Get the shared_ptr from the raw pointer
  std::shared_ptr<Session> getSharedPtr(const Session *session_raw);

  // Insert shared_ptr in the state
  void insertShared(std::shared_ptr<Session> session);
  // Delete an entry from the raw ptr
  void removeShared(const Session *session_raw);
  TdiCFrontEndSessionState(TdiCFrontEndSessionState const &) = delete;
  void operator=(TdiCFrontEndSessionState const &) = delete;

 private:
  TdiCFrontEndSessionState() {}
  std::mutex state_lock;
  std::map<const Session *, std::shared_ptr<Session> > sessionStateMap;
};

}  // tdi_c
}  // tdi

#endif  // _TDI_STATE_C_HPP
