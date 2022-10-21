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
/** @file tdi_dummy_init.h
 *
 *  @brief C frontend for init
 */
#ifndef _TDI_DUMMY_INIT_H_
#define _TDI_DUMMY_INIT_H_

// tdi includes
#include <tdi/common/tdi_defs.h>

#include "../tdi_dummy_init.hpp"

#ifdef __cplusplus
extern "C" {
#endif

tdi_status_t tdi_module_init(void *target_options);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_DUMMY_INIT_H_
