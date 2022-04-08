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
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <tdi/common/c_frontend/tdi_learn.h>

#ifdef __cplusplus
}
#endif

#include <tdi/common/tdi_session.hpp>
#include "tdi_state_c.hpp"

namespace tdi {
namespace tdi_c {

TdiCFrontEndSessionState &TdiCFrontEndSessionState::getInstance() {
  static TdiCFrontEndSessionState instance;
  return instance;
}

std::shared_ptr<Session> TdiCFrontEndSessionState::getSharedPtr(
    const Session *session_raw) {
  if (session_raw == nullptr) {
    return nullptr;
  }
  std::lock_guard<std::mutex> lock(state_lock);
  if (sessionStateMap.find(session_raw) != sessionStateMap.end()) {
    return sessionStateMap.at(session_raw);
  }
  return nullptr;
}

void TdiCFrontEndSessionState::insertShared(
    std::shared_ptr<Session> session) {
  std::lock_guard<std::mutex> lock(state_lock);
  if (sessionStateMap.find(session.get()) != sessionStateMap.end()) {
    return;
  }
  sessionStateMap[session.get()] = session;
}

void TdiCFrontEndSessionState::removeShared(const Session *session) {
  std::lock_guard<std::mutex> lock(state_lock);
  if (sessionStateMap.find(session) == sessionStateMap.end()) {
    return;
  }
  sessionStateMap.erase(sessionStateMap.find(session));
}

}  // tdi_c
}  // tdi
