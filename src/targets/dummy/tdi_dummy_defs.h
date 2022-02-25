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
/** @file tdi_dummy_defs.h
 *
 *  @brief Contains Common enum and defs used in TDI specific to a target
 */
#ifndef _TDI_DUMMY_DEFS_H
#define _TDI_DUMMY_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Table types. Users are discouraged from using this especially when
 * creating table-agnostic generic applications like a CLI or RPC server
 */
enum tdi_dummy_table_type_e {
  /** Match action table*/
  TDI_DUMMY_TABLE_TYPE_MATCH_DIRECT = TDI_TABLE_TYPE_DEVICE,
  /** Match action table with actions of the table implemented using an
  "ActionProfile" */
  TDI_DUMMY_TABLE_TYPE_MATCH_INDIRECT,
  /** Match action table with actions of the table implemented using an
  "ActionSelector"*/
  TDI_DUMMY_TABLE_TYPE_MATCH_INDIRECT_SELECTOR,
  /** Action Profile table*/
  TDI_DUMMY_TABLE_TYPE_ACTION_PROFILE,
  /** Action Selector table*/
  TDI_DUMMY_TABLE_TYPE_SELECTOR,
  /** Counter table*/
  TDI_DUMMY_TABLE_TYPE_COUNTER,
  /** Meter table*/
  TDI_DUMMY_TABLE_TYPE_METER,
  /** Register table*/
  TDI_DUMMY_TABLE_TYPE_REGISTER,
  TDI_DUMMY_TABLE_TYPE_PORT_CFG,
  /** Port Stats */
  TDI_DUMMY_TABLE_TYPE_PORT_STAT,
  TDI_DUMMY_TABLE_TYPE_INVALID_TYPE
};

#ifdef __cplusplus
}
#endif

#endif
