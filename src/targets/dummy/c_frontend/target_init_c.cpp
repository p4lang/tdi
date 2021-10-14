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
#include <tdi/common/tdi_init.hpp>
#include "target_init.h"

tdi_status_t tdi_module_init(const tdi_mgr_type_e* arr, const size_t arr_size) {
  std::vector<tdi_mgr_type_e> mgr_type_vec(arr, arr + arr_size);
  // Need to call tdi::tna::dummy::Init::tdiModuleInit() instead
  return tdi::Init::tdiModuleInit(mgr_type_vec);
}

