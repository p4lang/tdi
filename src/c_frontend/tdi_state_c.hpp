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
