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
/** @file tdi_table.h
 *
 *  @brief Contains TDI Table APIs
 */
#ifndef _TDI_TABLE_H
#define _TDI_TABLE_H

#include <tdi/common/tdi_defs.h>
#include <tdi/common/c_frontend/tdi_table_data.h>
#include <tdi/common/c_frontend/tdi_table_key.h>
#include <tdi/common/c_frontend/tdi_table_operations.h>
//#include <tdi/common/c_frontend/tdi_table_attributes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Annotation struct. Contains name-value pair.
 * This is extracted from tdi.json. @c value might contain
 * null for some annotations but name will always contain
 * a string.
 */
typedef struct tdi_annotation_ {
  /** Name field of annotation */
  const char *name;
  /** Value field of annotation */
  const char *value;
} tdi_annotation_t;

/**
 * @brief Table types. Users are discouraged from using this especially when
 * creating table-agnostic generic applications like a CLI or RPC server
 */
typedef enum tdi_table_type_ {
  /** Match action table*/
  MATCH_DIRECT = 0,
  /** Match action table with actions of the table implemented using an
     "ActionProfile" */
  MATCH_INDIRECT = 1,
  /** Match action table with actions of the table implemented using an
     "ActionSelector"*/
  MATCH_INDIRECT_SELECTOR = 2,
  /** Action Profile table*/
  ACTION_PROFILE = 3,
  /** Action Selector table*/
  SELECTOR = 4,
  /** Counter table*/
  COUNTER = 5,
  /** Meter table*/
  METER = 6,
  /** Register table*/
  REGISTER = 7,
  /** LPF table*/
  LPF = 8,
  /** WRED table*/
  WRED = 9,
  /** Parser Value Set table. This table contains only Keys and no Data */
  PVS = 10,
  /** Port Metadata table.*/
  PORT_METADATA = 11,
  /** Dynamic Hashing configuration table*/
  DYN_HASH_CFG = 12,
  /** Snapshot config table. */
  SNAPSHOT_CFG = 13,
  /** Snapshot field Liveness */
  SNAPSHOT_LIVENESS = 14,
  /** Port Configuration */
  PORT_CFG = 15,
  /** Port Stats */
  PORT_STAT = 16,
  /** Port Hdl to Dev_port Conversion table */
  PORT_HDL_INFO = 17,
  /** Front panel Idx to Dev_port Conversion table */
  PORT_FRONT_PANEL_IDX_INFO = 18,
  /** Port Str to Dev_port Conversion table */
  PORT_STR_INFO = 19,
  /** Pktgen Port Configuration table */
  PKTGEN_PORT_CFG = 20,
  /** Pktgen Application Configuration table */
  PKTGEN_APP_CFG = 21,
  /** Pktgen Packet Buffer Configuration table */
  PKTGEN_PKT_BUFF_CFG = 22,
  /** Pktgen Port Mask Configuration table */
  PKTGEN_PORT_MASK_CFG = 23,
  /** Pktgen Port Down Replay Configuration table*/
  PKTGEN_PORT_DOWN_REPLAY_CFG = 24,
  /** PRE MGID table */
  PRE_MGID = 25,
  /** PRE Node table */
  PRE_NODE = 26,
  /** PRE ECMP table */
  PRE_ECMP = 27,
  /** PRE LAG table */
  PRE_LAG = 28,
#if 0
  /** PRE Prune (L2 XID) table */
  PRE_PRUNE = 29,
  /** Mirror configuration table */
  MIRROR_CFG = 30,
  /** Traffic Mgr PPG Table - retired */
  TM_PPG_OBSOLETE = 31,
  /** PRE Port table */
  PRE_PORT = 32,
  /** Dynamic Hashing algorithm table*/
  DYN_HASH_ALGO = 33,
  /** TM Pool Config Table **/
  TM_POOL_CFG = 34,
  /** TM Skid Pool Table **/
  TM_POOL_SKID = 35,
  /** Device Config Table */
  DEV_CFG = 36,
  /** TM App Pool Table **/
  TM_POOL_APP = 37,
  /** TM Egress Queue general configuration table */
  TM_QUEUE_CFG = 38,
  /** TM Egress Queue mappings read-only table */
  TM_QUEUE_MAP = 39,
  /** TM Egress Queue color limit settings table */
  TM_QUEUE_COLOR = 40,
  /** TM Egress Queue buffer and pool settings table */
  TM_QUEUE_BUFFER = 41,
  /** TM Port Group general config parameters table */
  TM_PORT_GROUP_CFG = 42,
  /** TM Port Group table */
  TM_PORT_GROUP = 43,
  /** TM Color table */
  TM_POOL_COLOR = 44,
  /** Snapshot PHV table */
  SNAPSHOT_PHV = 45,
  /** Snapshot trigger table */
  SNAPSHOT_TRIG = 46,
  /** Snapshot data table */
  SNAPSHOT_DATA = 47,
  /** TM Pool App PFC table */
  TM_POOL_APP_PFC = 48,
  /** TM Ingress Port Counters table */
  TM_COUNTER_IG_PORT = 49,
  /** TM Egress Port Counters table */
  TM_COUNTER_EG_PORT = 50,
  /** TM Queue Counters table */
  TM_COUNTER_QUEUE = 51,
  /** TM Pool Counters table */
  TM_COUNTER_POOL = 52,
  /** TM Port general config parameters table */
  TM_PORT_CFG = 53,
  /** TM Port Buffer table */
  TM_PORT_BUFFER = 54,
  /** TM Port Flow Control table */
  TM_PORT_FLOWCONTROL = 55,
  /** TM Pipe Counters table */
  TM_COUNTER_PIPE = 56,
  /** Debug Counters table */
  DBG_CNT = 57,
  /** Logical table debug counters table */
  LOG_DBG_CNT = 58,
  /** TM Cfg table */
  TM_CFG = 59,
  /** TM Pipe Multicast fifo table */
  TM_PIPE_MULTICAST_FIFO = 60,
  /** TM Mirror port DPG table */
  TM_MIRROR_DPG = 61,
  /** TM Port DPG table */
  TM_PORT_DPG = 62,
  /** TM PPG configuration table */
  TM_PPG_CFG = 63,
  /** Register param table */
  REG_PARAM = 64,
  /** TM Port DPG Counters table */
  TM_COUNTER_PORT_DPG = 65,
  /** TM Mirror Port DPG Counters table */
  TM_COUNTER_MIRROR_PORT_DPG = 66,
  /** TM PPG Counters table */
  TM_COUNTER_PPG = 67,
  /** Dynamic hash compute table */
  DYN_HASH_COMPUTE = 68,
  /** Action Selector Get Member table */
  SELECTOR_GET_MEMBER = 69,
  /** TM Egress Port Queue Scheduler parameters table */
  TM_QUEUE_SCHED_CFG = 70,
  /** TM Egress Port Queue Scheduler shaping table */
  TM_QUEUE_SCHED_SHAPING = 71,
  /** TM Egress Port Scheduler parameters table */
  TM_PORT_SCHED_CFG = 72,
  /** TM Egress Port Scheduler shaping table */
  TM_PORT_SCHED_SHAPING = 73,
  /** TM Pipe general config parameters table */
  TM_PIPE_CFG = 74,
  /** TM Pipe Scheduler parameters table */
  TM_PIPE_SCHED_CFG = 75,
  /** TM L1 Node Scheduler parameters table */
  TM_L1_NODE_SCHED_CFG = 76,
  /** TM L1 Node Scheduler shaping table */
  TM_L1_NODE_SCHED_SHAPING = 77,
#endif
  INVLD = 78
} tdi_table_type_t;

/* Table APIs */

/**
 * @brief Add an entry to the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key
 * @param[in] data Entry Data
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_add(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key,
                                 const tdi_table_data_hdl *data);

/**
 * @brief Modify an existing entry of the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key
 * @param[in] data Entry Data
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_mod(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *target,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key,
                                 const tdi_table_data_hdl *data);

/**
 * @brief Modify only an existing default entry of the table.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] data Entry Data
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_default_entry_mod(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *target,
                                         const tdi_flags_hdl *flags,
                                         const tdi_table_data_hdl *data);

/**
 * @brief Delete an entry of the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_del(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *dev_tgt,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key);

/**
 * @brief Clear a table. Delete all entries. This API also resets default
 * entry if present and is not const default. If table has always present
 * entries like Counter table, then this table resets all the entries
 * instead.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_clear(const tdi_table_hdl *table_hdl,
                             const tdi_session_hdl *session,
                             const tdi_target_hdl *dev_tgt,
                             const tdi_flags_hdl *flags);

/**
 * @brief Get an entry from the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key
 * @param[inout] data Entry Data, if not empty will be used to filter
 *                    returned fields
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_get(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *dev_tgt,
                                 const tdi_flags_hdl *flags,
                                 const tdi_table_key_hdl *key,
                                 tdi_table_data_hdl *data);

/**
 * @brief Get an entry from the table by entry handle
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] entry_handle Handle to the entry
 * @param[out] key Entry Key
 * @param[inout] data Entry Data, if not empty will be used to filter
 *                    returned fields
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_get_by_handle(const tdi_table_hdl *table_hdl,
                                           const tdi_session_hdl *session,
                                           const tdi_target_hdl *dev_tgt,
                                           const tdi_flags_hdl *flags,
                                           const uint32_t entry_handle,
                                           tdi_table_key_hdl *key,
                                           tdi_table_data_hdl *data);

/**
 * @brief Get the first entry of the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[out] key Entry Key
 * @param[inout] data Entry Data, if not empty will be used to filter
 *                    returned fields
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_get_first(const tdi_table_hdl *table_hdl,
                                       const tdi_session_hdl *session,
                                       const tdi_target_hdl *dev_tgt,
                                       const tdi_flags_hdl *flags,
                                       tdi_table_key_hdl *key,
                                       tdi_table_data_hdl *data);

/**
 * @brief Get an entry key from the table by handle
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] entry_handle Handle to the entry
 * @param[out] entry_tgt Target device for specified entry handle,
 *                       may not match dev_tgt values
 * @param[out] key Entry Key
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_key_get(const tdi_table_hdl *table_hdl,
                                     const tdi_session_hdl *session,
                                     const tdi_target_hdl *dev_tgt_in,
                                     const tdi_flags_hdl *flags,
                                     const uint32_t entry_handle,
                                     tdi_target_hdl *dev_tgt_out,
                                     tdi_table_key_hdl *key);

/**
 * @brief Get an entry handle from the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] entry_handle Handle to the entry
 * @param[out] key Entry Key
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_handle_get(const tdi_table_hdl *table_hdl,
                                        const tdi_session_hdl *session,
                                        const tdi_target_hdl *dev_tgt,
                                        const tdi_flags_hdl *flags,
                                        const tdi_table_key_hdl *key,
                                        uint32_t *entry_handle);

/**
 * @brief Get next N entries of the table following the entry that is
 * specificed by key.
 * If the N queried for is greater than the actual entries, then
 * all the entries present are returned.
 * N must be greater than zero.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] key Entry Key from which N entries are queried
 * @param[out] output_keys Array of allocated Key objects.
 * Size should be equal to n
 * @param[out] output_data Array of allocated Data objects.
 * Size should be equal to n
 * @param[in] n Number of entries queried 'N'
 * @param[out] num_returned Actual number of entries returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_get_next_n(const tdi_table_hdl *table_hdl,
                                        const tdi_session_hdl *session,
                                        const tdi_target_hdl *dev_tgt,
                                        const tdi_flags_hdl *flags,
                                        const tdi_table_key_hdl *key,
                                        tdi_table_key_hdl **output_keys,
                                        tdi_table_data_hdl **output_data,
                                        uint32_t n,
                                        uint32_t *num_returned);

/**
 * @brief Current Usage of the table
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[out] count Table usage
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_usage_get(const tdi_table_hdl *table_hdl,
                                 const tdi_session_hdl *session,
                                 const tdi_target_hdl *dev_tgt,
                                 const tdi_flags_hdl *flags,
                                 uint32_t *count);
/**
 * @brief Set the default Entry of the table
 *
 * @details There can be a P4 defined default entry with parameters. This API
 * modifies any existing default entry to the one passed in here. Note that
 * this API is idempotent and should be called either when modifying an
 * existing default entry or to program one newly. There could be a particular
 * action which is designated as a default-only action. In that case, an error
 * is returned if the action id of the data object passed in here is different
 * from the designated default action.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[in] data Entry Data
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_default_entry_set(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *dev_tgt,
                                         const tdi_flags_hdl *flags,
                                         const tdi_table_data_hdl *data);

/**
 * @brief Get the default Entry of the table
 *
 * @details The default entry returned will be the one programmed or the P4
 * defined one, if it exists. Note that, when the entry is obtained from
 * software, the P4 defined default entry will not be available if the default
 * entry was not programmed ever. However, when the entry is obtained from
 * hardware, the P4 defined default entry will be returned even if the default
 * entry was not programmed ever.
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[inout] data Entry Data, if not empty will be used to filter
 *                    returned fields
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_default_entry_get(const tdi_table_hdl *table_hdl,
                                         const tdi_session_hdl *session,
                                         const tdi_target_hdl *dev_tgt,
                                         const tdi_flags_hdl *flags,
                                         tdi_table_data_hdl *data);

/**
 * @brief Reset the default Entry of the table
 *
 * @details The default entry of the table is reset to the P4 specified
 * default action with parameters, if it exists, else its reset to a "no-op"
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_default_entry_reset(const tdi_table_hdl *table_hdl,
                                           const tdi_session_hdl *session,
                                           const tdi_target_hdl *dev_tgt,
                                           const tdi_flags_hdl *flags);

/**
 * @brief The maximum size of the table. Note that this size might
 * be different than present in tdi.json especially for Match Action
 * Tables. This is because sometimes MATs might reserve some space for
 * atomic modfies and hence might be 1 or 2 < json size
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device target
 * @param[in] flags Call flags
 * @param[out] size Number of total available entries
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_size_get(const tdi_table_hdl *table_hdl,
                                const tdi_session_hdl *session,
                                const tdi_target_hdl *dev_tgt,
                                const tdi_flags_hdl *flags,
                                size_t *size);


/**
 * @brief Get name of the table
 *
 * @param[in] table_hdl Table object
 * @param[out] name Name of the table
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_name_get(const tdi_table_hdl *table_hdl,
                                const char **table_name_ret);

/**
 * @brief Get ID of the table
 *
 * @param[in] table_hdl Table object
 * @param[out] id ID of the table
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_id_from_handle_get(const tdi_table_hdl *table_hdl,
                                          tdi_id_t *id);
/**
 * @brief The type of the table
 *
 * @param[in] table_hdl Table object
 * @param[out] table_type Type of the table
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_type_get(const tdi_table_hdl *table_hdl,
                                tdi_table_type_t *table_type);

/**
 * @brief Get whether this table has a const default action
 *
 * @param[in] table_hdl Table object
 * @param[out] has_const_default_action If default action is const
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_has_const_default_action(
    const tdi_table_hdl *table_hdl, bool *has_const_default_action);

/**
 * @brief Get size of list of annotations of table
 *
 * @param[in] table_hdl Table object
 * @param[out] num_annotations Size of list
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_num_annotations_get(const tdi_table_hdl *table_hdl,
                                           uint32_t *num_annotations);
/**
 * @brief Get an array of annotations on a Table
 *
 * @param[in] table_hdl Table object
 * @param[out] annotations_c Array of annotations
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_annotations_get(const tdi_table_hdl *table_hdl,
                                       tdi_annotation_t *annotations_c);

/**
 * @brief Get the size of array of supported APIs on table
 * @param[in] table_hdl Table object
 * @param[out] num Size of array
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_num_api_supported(const tdi_table_hdl *table_hdl,
                                         uint32_t *num);

/**
 * @brief Get array of supported APIs on table
 *
 * @param[in] table_hdl Table object
 * @param[out] apis Array of supported APIs
 * User needs to allocate memory
 * @param[out] num_returned Number actually returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_api_supported(const tdi_table_hdl *table_hdl,
                                     tdi_table_api_type_e *apis,
                                     uint32_t *num_returned);

/******************** Key APIs *******************/

/**
 * @brief Allocate key for the table
 *
 * @param[in] table_hdl Table object
 * @param[out] key_hdl_ret Key object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_key_allocate(const tdi_table_hdl *table_hdl,
                                    tdi_table_key_hdl **key_hdl_ret);

/**
 * @brief Reset the key object associated with the table
 *
 * @param[in] table_hdl Table object
 * @param[inout] key_hdl_ret Pointer to a key object, previously allocated using
 * tdi_table_key_allocate() on the table.
 *
 * @return Status of the API call. Error is returned if the key object is not
 * associated with the table.
 */
tdi_status_t tdi_table_key_reset(const tdi_table_hdl *table_hdl,
                                 tdi_table_key_hdl **key_hdl_ret);

/**
 * @brief Deallocate key for the table
 *
 * @param[in] table_hdl Table object
 * @param[in] key_hdl Key object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_key_deallocate(tdi_table_key_hdl *key_hdl);

/**
 * @brief Get size of list of key field IDs
 *
 * @param[in] table_hdl Table object
 * @param[out] num Size of array
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_id_list_size_get(const tdi_table_hdl *table_hdl,
                                            uint32_t *num);

/**
 * @brief Get an array of Key field IDs
 *
 * @param[in] table_hdl Table object
 * @param[out] id_arr Array of key field IDs
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_id_list_get(const tdi_table_hdl *table_hdl,
                                       tdi_id_t *id_arr);

/**
 * @brief Get field type of Key Field
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] field_type Field Type (Exact/Ternary/LPM/Range)
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_match_type_get(const tdi_table_hdl *table_hdl,
                                          const tdi_id_t field_id,
                                          tdi_match_type_e *field_type);

/**
 * @brief Get data type of Key Field
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] data_type Field Type (uint64, float, string)
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_data_type_get(const tdi_table_hdl *table_hdl,
                                         const tdi_id_t field_id,
                                         tdi_field_data_type_e *data_type);

/**
 * @brief Get field ID of Key Field from name
 *
 * @param[in] table_hdl Table object
 * @param[in] name Key Field name
 * @param[out] field_id Field ID
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_id_get(const tdi_table_hdl *table_hdl,
                                  const char *name,
                                  tdi_id_t *field_id);

/**
 * @brief Get field size
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] size Field Size in bits
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_size_get(const tdi_table_hdl *table_hdl,
                                    const tdi_id_t field_id,
                                    size_t *size);

/**
 * @brief Get whether Key Field is of type ptr or not. If the field is
 * of ptr type, then only ptr sets/gets are applicable on the field. Else
 * both the ptr versions and the uint64_t versions work
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] is_ptr Boolean type indicating whether Field is of type ptr
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_is_ptr_get(const tdi_table_hdl *table_hdl,
                                      const tdi_id_t field_id,
                                      bool *is_ptr);

/**
 * @brief Get name of field
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] name Field name
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_name_get(const tdi_table_hdl *table_hdl,
                                    const tdi_id_t field_id,
                                    const char **name);

/**
 * @brief Get size of array of allowed choices for string
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] num Array size
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_num_allowed_choices_get(
    const tdi_table_hdl *table_hdl, const tdi_id_t field_id, uint32_t *num);

/**
 * @brief Get array of allowed choices for string
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] choices Array of char ptrs. The array of ptrs needs to
 * be allocated by user based upon the size from num API
 *
 * @return Status of the API call
 */
tdi_status_t tdi_key_field_allowed_choices_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const char *choices[]);

/******************** Data APIs *******************/

/**
 * @brief Allocate Data Object for the table
 *
 * @param[in] table_hdl Table object
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_data_allocate(const tdi_table_hdl *table_hdl,
                                     tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Allocate Data Object for the table
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id Action ID
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_action_data_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Data Allocate with a list of field-ids. This allocates the data
 * object for
 * the list of field-ids. The field-ids passed must be valid for this table.
 * The Data Object then entertains APIs to read/write only those set of fields
 *
 * @param[in] table_hdl Table object
 * @param[in] fields Array of field IDs
 * @param[in] num_array Size of input array
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_data_allocate_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Data Allocate with a list of field-ids. This allocates the data
 * object for
 * the list of field-ids. The field-ids passed must be valid for this table.
 * The Data Object then entertains APIs to read/write only those set of fields
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id Action ID
 * @param[in] fields Array of field IDs
 * @param[in] num_array Size of input array
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_action_data_allocate_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Allocate Data Object for a container field
 *
 * @note: Users do not need to deallocate this object. After sending this object
 * as part of setValue, the parent Data object takes ownership. This will get
 * freed when parent is deallocated. For getValue, a previous allocate isn't
 * required.
 * However, if setValue fails for some reason and user needs to deallocate, then
 * a \ref tdi_table_data_deallocate() can be called
 *
 * @param[in] table_hdl Table object
 * @param[in] c_field_id Container field ID
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_data_allocate_container(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Allocate Data Object for a container field
 *
 * @note: Users do not need to deallocate this object. After sending this object
 * as part of setValue, the parent Data object takes ownership. This will get
 * freed when parent is deallocated. For getValue, a previous allocate isn't
 * required.
 * However, if setValue fails for some reason and user needs to deallocate, then
 * a \ref tdi_table_data_deallocate() can be called
 *
 * @param[in] table_hdl Table object
 * @param[in] c_field_id Container field ID
 * @param[in] action_id Action ID
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_action_data_allocate_container(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t action_id,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Data Allocate for container with a list of field-ids.
 * This allocates the data object for
 * the list of field-ids. The field-ids passed must be valid for this table
 * and container.
 * The Data Object then entertains APIs to read/write only those set of fields
 *
 * @note: Users do not need to deallocate this object. After sending this object
 * as part of setValue, the parent Data object takes ownership. This will get
 * freed when parent is deallocated. For getValue, a previous allocate isn't
 * required.
 * However, if setValue fails for some reason and user needs to deallocate, then
 * a \ref tdi_table_data_deallocate() can be called
 *
 * @param[in] table_hdl Table object
 * @param[in] c_field_id Container field ID
 * @param[in] fields Array of field IDs
 * @param[in] num_array Size of input array
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_data_allocate_container_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Data Allocate for container with a list of field-ids.
 * This allocates the data object for
 * the list of field-ids. The field-ids passed must be valid for this table
 * and container.
 * The Data Object then entertains APIs to read/write only those set of fields
 *
 * @note: Users do not need to deallocate this object. After sending this object
 * as part of setValue, the parent Data object takes ownership. This will get
 * freed when parent is deallocated. For getValue, a previous allocate isn't
 * required.
 * However, if setValue fails for some reason and user needs to deallocate, then
 * a \ref tdi_table_data_deallocate() can be called
 *
 * @param[in] table_hdl Table object
 * @param[in] c_field_id Container field ID
 * @param[in] action_id Action ID
 * @param[in] fields Array of field IDs
 * @param[in] num_array Size of input array
 * @param[out] data_hdl_ret Data Object returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_action_data_allocate_container_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t c_field_id,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Reset the data object previously allocated using dataAllocate on the
 * table
 *
 * @details Calling this API resets the action-id in the object to 0.
 * Typically this needs to be done when doing an entry get,
 * since the caller does not know the action-id associated with the entry.
 *
 * @param[in] table_hdl Table object
 * @param[inout] data_hdl_ret Pointer to the data object allocated using
 * dataAllocate on the table.
 *
 * @return Status of the API call. An error is returned if the data object is
 * not associated with the table
 */
tdi_status_t tdi_table_data_reset(const tdi_table_hdl *table_hdl,
                                  tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Reset the data object previously allocated using dataAllocate on the
 *table
 *
 * @details Calling this API sets the action-id in the object to the
 * passed in value.
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id  new action id of the object the table.
 * @param[in/out] data_hdl_ret Pointer to the data object allocated using
 * dataAllocate on
 * the table.
 *
 * @return Status of the API call. An error is returned if the data object is
 * not associated with the table or if action_id doesn't exist
 */
tdi_status_t tdi_table_action_data_reset(const tdi_table_hdl *table_hdl,
                                         const tdi_id_t action_id,
                                         tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Reset the data object previously allocated using dataAllocate on the
 * table
 *
 * @details Calling this API resets the action-id in the object to an
 * invalid value. Typically this needs to be done when doing an entry get,
 * since the caller does not know the action-id associated with the entry.
 * Using the data object for an entry add on a table where action-id is
 * expected will result in an error. The data object will contain the passed
 * in vector of field-ids active. This is typically done when reading an
 * entry's fields. Note that, the fields passed in must be common data fields
 * across all action-ids (common data fields, such as direct counter/direct
 * meter etc)
 *
 * @param[in] table_hdl Table object
 * @param[in] fields Array of field-ids that are to be activated in the data
 *object the table.
 * @param[in] num_array Array size of fields
 * @param[in/out] data_hdl_ret Pointer to the data object allocated using
 * dataAllocate on the table.
 *
 * @return Status of the API call. An error is returned if the data object is
 *not associated with the table
 */
tdi_status_t tdi_table_data_reset_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Reset the data object previously allocated using dataAllocate on the
 *table
 *
 * @details Calling this API sets the action-id in the object to the
 * passed in value and the list of fields passed in will be active in the data
 * object. The list of fields passed in must belong to the passed in action-id
 * or common across all action-ids associated with the table.
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id  new action id of the object the table.
 * @param[in] fields Array of field-ids that are to be activated in the data
 * object of the table.
 * @param[in] num_array Input field array size
 * @param[in/out] data_hdl_ret Pointer to the data object allocated using
 *dataAllocate on the table.
 *
 * @return Status of the API call. An error is returned if the data object is
 * not associated with the table
 */
tdi_status_t tdi_table_action_data_reset_with_fields(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    const tdi_id_t *fields,
    const uint32_t num_array,
    tdi_table_data_hdl **data_hdl_ret);

/**
 * @brief Deallocate data object. Only one version of data deallocate API exists
 *
 * @param[in] data_hdl Data object
 *
 * @return Status of API call
 */
tdi_status_t tdi_table_data_deallocate(tdi_table_data_hdl *data_hdl);

/**
 * @brief Get Size of array of Data field IDs
 *
 * @param[in] table_hdl Table object
 * @param[out] num Size of array
 *
 * @return Status of API call
 */
tdi_status_t tdi_data_field_id_list_size_get(const tdi_table_hdl *table_hdl,
                                             uint32_t *num);
/**
 * @brief Get Size of array of Data field IDs
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id Action ID
 * @param[out] num Size of Array of IDs
 *
 * @return Status of API call
 */
tdi_status_t tdi_data_field_id_list_size_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    uint32_t *num);

/**
 * @brief Get array of data field IDs
 *
 * @param[in] table_hdl Table object
 * @param[in] id_arr Array of IDs
 *
 * @return Status of API call
 */
tdi_status_t tdi_data_field_list_get(const tdi_table_hdl *table_hdl,
                                     tdi_id_t *id_arr);

/**
 * @brief Get array of field IDs inside a container
 * field. This API works off the first found match of container
 * field ID
 *
 * @param[in] table_hdl Table object
 * @param[in] container_field_id Field ID of container field
 * @param[out] id_arr Array of IDs
 *
 * @return Status of API call
 */
tdi_status_t tdi_container_data_field_list_get(
    const tdi_table_hdl *table_hdl,
    tdi_id_t container_field_id,
    tdi_id_t *id_arr);

/**
 * @brief Get Size of array of field IDs inside a container
 * field. This API works off the first found match of container
 * field ID
 *
 * @param[in] table_hdl Table object
 * @param[in] container_field_id Field ID of container field
 * @param[out] num Size of array of IDs
 *
 * @return Status of API call
 */
tdi_status_t tdi_container_data_field_list_size_get(
    const tdi_table_hdl *table_hdl,
    tdi_id_t container_field_id,
    size_t *num);

/**
 * @brief Get array of data field IDs for a particular action
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id Action ID
 * @param[in] id_arr Array of IDs
 *
 * @return Status of API call
 */
tdi_status_t tdi_data_field_list_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t action_id,
    tdi_id_t *id_vec_ret);

/**
 * @brief Get the field ID of a Data Field from a name
 *
 * @param[in] table_hdl Table object
 * @param[in] name Name of a Data field
 * @param[out] field_id Field ID
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_id_get(const tdi_table_hdl *table_hdl,
                                   const char *name,
                                   tdi_id_t *field_id);

/**
 * @brief Get the field ID of a Data Field from a name and
 * action ID
 *
 * @param[in] table_hdl Table object
 * @param[in] name Name of a Data field
 * @param[in] action_id Action ID
 * @param[out] field_id Field ID
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_id_with_action_get(
    const tdi_table_hdl *table_hdl,
    const char *name,
    const tdi_id_t action_id,
    tdi_id_t *field_id);

/**
 * @brief Get the Size of a field. For container fields this
 * function will return number of elements inside the container.
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] field_size Size of the field in bits
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_size_get(const tdi_table_hdl *table_hdl,
                                     const tdi_id_t field_id,
                                     size_t *field_size);

/**
 * @brief Get the Size of a field with field ID and action ID.
 * For container fields this
 * function will return number of elements inside the container.
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] field_size Size of the field in bits
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_size_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    size_t *field_size);

/**
 * @brief Get whether a field is a ptr type.
 * Only the ptr versions of setValue/getValue will work on fields
 * for which this API returns true
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] is_ptr Boolean value indicating if it is ptr type
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_is_ptr_get(const tdi_table_hdl *table_hdl,
                                       const tdi_id_t field_id,
                                       bool *is_ptr);

/**
 * @brief Get whether a field is a ptr type with field and action ID
 * Only the ptr versions of setValue/getValue will work on fields
 * for which this API returns true
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] is_ptr Boolean value indicating if it is ptr type
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_is_ptr_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    bool *is_ptr);

/**
 * @brief Get whether a field is mandatory.
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] is_mandatory Boolean value indicating if it is mandatory
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_is_mandatory_get(const tdi_table_hdl *table_hdl,
                                             const tdi_id_t field_id,
                                             bool *is_mandatory);

/**
 * @brief Get whether a field is mandatory
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] is_mandatory Boolean value indicating if it is mandatory
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_is_mandatory_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    bool *is_mandatory);

/**
* @brief Get whether a field is ReadOnly.
*
* @param[in] table_hdl Table object
* @param[in] field_id Field ID
* @param[out] is_read_only Boolean value indicating if it is ReadOnly
*
* @return Status of the API call
*/
tdi_status_t tdi_data_field_is_read_only_get(const tdi_table_hdl *table_hdl,
                                             const tdi_id_t field_id,
                                             bool *is_read_only_ret);

/**
* @brief Get whether a field is ReadOnly.
*
* @param[in] table_hdl Table object
* @param[in] field_id Field ID
* @param[in] action_id Action ID
* @param[out] is_read_only Boolean value indicating if it is ReadOnly
*
* @return Status of the API call
*/
tdi_status_t tdi_data_field_is_read_only_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    bool *is_read_only_ret);

/**
 * @brief Get the Name of a field. Note that driver will return
 * a ptr to an internally kept memory. Users need not free
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] name Name of the field
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_name_get(const tdi_table_hdl *table_hdl,
                                     const tdi_id_t field_id,
                                     const char **name);

/**
 * @brief Get the Name of a field. Driver will return a copy of
 * the field name. Please ensure enough memory is allocated.
 * buf_sz tells driver max bytes to write in the buffer.
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] buf_sz Max size to write in the char buffer
 * @param[out] name Name of the field
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_name_copy_get(const tdi_table_hdl *table_hdl,
                                          const tdi_id_t field_id,
                                          const uint32_t buf_sz,
                                          char *name_ret);

/**
 * @brief Get the Name of a field. Note that driver will return
 * a ptr to an internally kept memory. Users need not free
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] name Name of the field
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_name_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    const char **name_ret);

/**
 * @brief Get the Data type of a field (INT/BOOL/ENUM/INT_ARR/BOOL_ARR)
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] type Data type of a data field
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_type_get(const tdi_table_hdl *table_hdl,
                                     const tdi_id_t field_id,
                                     tdi_field_data_type_e *type);

/**
 * @brief Get the Data type of a field (INT/BOOL/ENUM/INT_ARR/BOOL_ARR)
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] type Data type of a data field
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_type_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    tdi_field_data_type_e *field_type_ret);

/**
 * @brief Get size of list of all the allowed values that a particular field can
 * have. This API is only for fields with string type. If the returned
 * vector is empty, it indicates that the allowed choices have not been
 * published in tdi json
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] num_choices Size of array
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_num_allowed_choices_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    uint32_t *num_choices);

/**
 * @brief Get list of all the allowed values that a particular field can
 * have. This API is only for fields with string type. If the returned
 * vector is empty, it indicates that the allowed choices have not been
 * published in tdi json
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] choices Char ptr array. The array itself needs to be allocated
 * by user, but the individual strings don't
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_allowed_choices_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const char *choices[]);

/**
 * @brief Get size of list of all the allowed values that a particular field can
 * have. This API is only for fields with string type. If the returned
 * vector is empty, it indicates that the allowed choices have not been
 * published in tdi json
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] num_choices Size of array
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_num_allowed_choices_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    uint32_t *num_choices);

/**
 * @brief Get list of all the allowed values that a particular field can
 * have. This API is only for fields with string type. If the returned
 * vector is empty, it indicates that the allowed choices have not been
 * published in tdi json
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] choices Char ptr array. The array itself needs to be allocated
 * by user, but the individual strings don't
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_allowed_choices_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    const char *choices[]);

/**
 * @brief Get size of array annotations on a data field
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] num Size of array of annotations
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_num_annotations_get(
    const tdi_table_hdl *table_hdl, const tdi_id_t field_id, uint32_t *num);

/**
 * @brief Get array of annotations on a data field
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] annotations_c Array of annotations
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_annotations_get(const tdi_table_hdl *table_hdl,
                                            const tdi_id_t field_id,
                                            tdi_annotation_t *annotations_c);
/**
 * @brief Get size of array annotations on a data field
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] num Size of array of annotations
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_num_annotations_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    uint32_t *num);

/**
 * @brief Get array of annotations on a data field
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] annotations_c Array of annotations
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_annotations_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    tdi_annotation_t *annotations_c);

/**
 * @brief Get Size of array of IDs of oneof siblings of a field
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] num Size of oneof-siblings array
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_num_oneof_siblings_get(
    const tdi_table_hdl *table_hdl, const tdi_id_t field_id, uint32_t *num);

/**
 * @brief Get the IDs of oneof siblings of a field. If a field is part of a
 * oneof , for example, consider $ACTION_MEMBER_ID and $SELECTOR_GROUP_ID. then
 * this API will return [field_ID($ACTION_MEMBER_ID)] for $SELECTOR_GROUP_ID.
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[out] oneof_siblings_c Array containing field IDs of oneof siblings
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_oneof_siblings_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    tdi_id_t *oneof_siblings_c);

/**
 * @brief Get Size of array of IDs of oneof siblings of a field
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] num Size of oneof-siblings array
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_num_oneof_siblings_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    uint32_t *num);

/**
 * @brief Get the IDs of oneof siblings of a field. If a field is part of a
 * oneof , for example, consider $ACTION_MEMBER_ID and $SELECTOR_GROUP_ID. then
 * this API will return [field_ID($ACTION_MEMBER_ID)] for $SELECTOR_GROUP_ID.
 *
 * @param[in] table_hdl Table object
 * @param[in] field_id Field ID
 * @param[in] action_id Action ID
 * @param[out] oneof_siblings_c Array containing field IDs of oneof siblings
 *
 * @return Status of the API call
 */
tdi_status_t tdi_data_field_oneof_siblings_with_action_get(
    const tdi_table_hdl *table_hdl,
    const tdi_id_t field_id,
    const tdi_id_t action_id,
    tdi_id_t *oneof_siblings_c);

/******************* Action APIs ******************/
/**
 * @brief Get size of array of action IDs
 *
 * @param[in] table_hdl Table object
 * @param[out] num Size  of array
 *
 * @return Status of API call
 */
tdi_status_t tdi_action_id_list_size_get(const tdi_table_hdl *table_hdl,
                                         uint32_t *num);

/**
 * @brief Get array of action IDs
 *
 * @param[in] table_hdl Table object
 * @param[out] id_arr Array of action IDs
 *
 * @return Status of API call
 */
tdi_status_t tdi_action_id_list_get(const tdi_table_hdl *table_hdl,
                                    tdi_id_t *id_arr);

/**
 * @brief Get Action Name
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id Action ID
 * @param[out] name Action Name. User doesn't need to allocate memory
 *
 * @return Status of the API call
 */
tdi_status_t tdi_action_name_get(const tdi_table_hdl *table_hdl,
                                 const tdi_id_t action_id,
                                 const char **name);

/**
 * @brief Get Action ID from Name
 *
 * @param[in] table_hdl Table object
 * @param[in] name Action Name
 * @param[out] action_id Action ID
 *
 * @return Status of the API call
 */
tdi_status_t tdi_action_name_to_id(const tdi_table_hdl *table_hdl,
                                   const char *name,
                                   tdi_id_t *action_id);

/**
 * @brief Are Action IDs applicable for this table
 *
 * @param[in] table_hdl Table object
 * @param ret_val If actions are applicable
 *
 * @return Status of API call
 */
tdi_status_t tdi_action_id_applicable(const tdi_table_hdl *table_hdl,
                                      bool *ret_val);

/**
 * @brief Get size of array annotations on an action
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id Action ID
 * @param[out] num Size of array of annotations
 *
 * @return Status of the API call
 */
tdi_status_t tdi_action_num_annotations_get(const tdi_table_hdl *table_hdl,
                                            const tdi_id_t action_id,
                                            uint32_t *num);
/**
 * @brief Get array of annotations on an action
 *
 * @param[in] table_hdl Table object
 * @param[in] action_id Action ID
 * @param[out] annotations_c Array of annotations
 *
 * @return Status of the API call
 */
tdi_status_t tdi_action_annotations_get(const tdi_table_hdl *table_hdl,
                                        const tdi_id_t action_id,
                                        tdi_annotation_t *annotations_c);

/********************** Attribute APIs ***********************/
/**
 * @brief Allocate attribute object for entry scope
 *
 * @param[in] table_hdl Table object
 * @param[out] tbl_attr_hdl_ret Attribute object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_entry_scope_attributes_allocate(
    const tdi_table_hdl *table_hdl,
    tdi_table_attributes_hdl **tbl_attr_hdl_ret);

#ifdef _TDI_FROM_BFRT

/**
 * @brief Deallocate attribute object
 *
 * @param[in] tbl_attr_hdl
 *
 * @return Status of API call
 */
tdi_status_t tdi_table_attributes_deallocate(
    tdi_table_attributes_hdl *tbl_attr_hdl);

/**
 * @brief Apply an Attribute from an Attribute Object on the
 * table. Before using this API, the Attribute object needs to
 * be allocated and all the necessary values need to
 * be set on the Attribute object
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device Target
 * @param[in] flags Call flags
 * @param[in] tableAttributes Attribute Object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_attributes_set(
    const tdi_table_hdl *table_hdl,
    const tdi_session_hdl *session,
    const tdi_target_hdl *dev_tgt,
    const tdi_flags_hdl *flags,
    const tdi_table_attributes_hdl *tbl_attr);

/**
 * @brief Get the current value of an Attribute set on the table.
 * The attribute object passed in as input param needs to be allocated
 * first with the required attribute type.
 * After this API call, invidual gets need to be called on the attribute
 * object. Refer to tdi_table_attributes.h for those APIs
 *
 * @param[in] table_hdl Table object
 * @param[in] session Session Object
 * @param[in] dev_tgt Device Target
 * @param[in] flags Call flags
 * @param[out] tbl_attr Attribute Object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_attributes_get(const tdi_table_hdl *table_hdl,
                                      const tdi_session_hdl *session,
                                      const tdi_target_hdl *dev_tgt,
                                      const tdi_flags_hdl *flags,
                                      tdi_table_attributes_hdl *tbl_attr);

/**
 * @brief Get the size of array of supported attributes
 * @param[in] table_hdl Table object
 * @param[out] num Size of array
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_num_attributes_supported(
    const tdi_table_hdl *table_hdl, uint32_t *num);

/**
 * @brief Get array of supported attributes
 *
 * @param[in] table_hdl Table object
 * @param[out] attributes Array of supported attributes.
 * User needs to allocate memory
 * @param[out] num_returned Number actually returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_attributes_supported(
    const tdi_table_hdl *table_hdl,
    tdi_table_attributes_type_t *attributes,
    uint32_t *num_returned);
#endif

/********************* Operations APIs ***********************/

/**
 * @brief Allocate operations object for a particular
 * operation op_type. If operations is not supported on table,
 * API will fail
 *
 * @param[in] table_hdl Table object
 * @param[in] op_type Operation type
 * @param[out] tbl_ops Attribute object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_operations_allocate(
    const tdi_table_hdl *table_hdl,
    const tdi_operations_type_e op_type,
    tdi_table_operations_hdl **tbl_ops);

/**
 * @brief Deallocate operations object
 *
 * @param[in] tbl_attr_hdl
 *
 * @return Status of API call
 */
tdi_status_t tdi_table_operations_deallocate(
    tdi_table_operations_hdl *tbl_op_hdl);

/**
 * @brief Execute Operations on a table. User
 * needs to allocate operation object with correct
 * type and then pass it to this API
 *
 * @param[in] table_hdl Table object
 * @param[in] tbl_ops Operations Object
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_operations_execute(
    const tdi_table_hdl *table_hdl,
    const tdi_table_operations_hdl *tbl_ops);

/**
 * @brief Get the size of array of supported operations
 * @param[in] table_hdl Table object
 * @param[out] num Size of array
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_num_operations_supported(
    const tdi_table_hdl *table_hdl, uint32_t *num);

/**
 * @brief Get array of supported operations
 *
 * @param[in] table_hdl Table object
 * @param[out] attributes Array of supported operations
 * User needs to allocate memory
 * @param[out] num_returned Number actually returned
 *
 * @return Status of the API call
 */
tdi_status_t tdi_table_operations_supported(
    const tdi_table_hdl *table_hdl,
    tdi_operations_type_e *operations,
    uint32_t *num_returned);
#ifdef __cplusplus
}
#endif

#endif  //_TDI_TABLE_H
