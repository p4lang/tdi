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
/** @file tdi_common.h
 *
 *  @brief Contains Common data types used in TDI
 */
#ifndef _TDI_COMMON_H
#define _TDI_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include <pipe_mgr/pipe_mgr_intf.h>

#ifdef __cplusplus
extern "C" {
#endif
//#include <tdi_types/tdi_types.h>

/**
 * @brief 32-bit ID for all TDI entities
 */
typedef uint32_t tdi_id_t;

/**
 * @brief 32-bit ID for table entry handles
 */
typedef uint32_t tdi_handle_t;

/**
 * @brief For sake of strict typechecking, instead of typedef-ing
 * \n void to our handle, we make a small struct with one element.
 */
#define DECLARE_HANDLE(name) \
  struct name##__ {          \
    int unused;              \
  };                         \
  typedef struct name##__ name

/**
 * @brief Macro used to set flag for table API calls.
 */
#define TDI_FLAG_SET(vf, offset) vf |= 1ull << offset;
/**
 * @brief Macro used to init flags variable.
 */
#define TDI_FLAG_INIT(vf) vf = 0;
/**
 * @brief Macro used to clear a flag.
 */
#define TDI_FLAG_CLEAR(vf, offset) vf &= ~(1ull << offset);
/**
 * @brief Conditional macro to check if specific flag is set.
 */
#define TDI_FLAG_IS_SET(vf, offset) (vf & (1ull << offset))

/**
 * @brief Used to set/clear bits in flag vector. Flags that are
 *        not used/handled by a specific function will be ignored.
 */
typedef enum tdi_flag_offset_t {
  /** Flag to operate on HW instead of SW. Supported with various
   * tableEntryGet() calls. Set for HW, unset for SW.
   */
  TDI_FROM_HW = 0,
  /** Flag used to perform delete action in tableEntryModInc
   * function calls. Bit set means delete otherwise add.
   */
  TDI_INC_DEL = 1,
  /** Flag used to skip TTL reset when modifying entry. */
  TDI_SKIP_TTL_RESET = 2,
} tdi_flag_offset_t;

#if 0
// C frontend typedefs
DECLARE_HANDLE(tdi_info_hdl);
DECLARE_HANDLE(tdi_session_hdl);
DECLARE_HANDLE(tdi_table_hdl);
DECLARE_HANDLE(tdi_table_key_hdl);
DECLARE_HANDLE(tdi_table_data_hdl);
DECLARE_HANDLE(tdi_table_attributes_hdl);
DECLARE_HANDLE(tdi_entry_scope_arguments_hdl);
DECLARE_HANDLE(tdi_learn_hdl);
DECLARE_HANDLE(tdi_table_operations_hdl);
DECLARE_HANDLE(tdi_learn_msg_hdl);
#endif

/**
 * @brief learn_data_hdl and table_data_hdl are the same,
 *  \n hence simple typedef like BfRtLearnData in
 *  \n include/tdi_learn.hpp
 */
//typedef tdi_table_data_hdl tdi_learn_data_hdl;

/* C and C++ void typedefs */
//DECLARE_HANDLE(tdi_learn_msg_hdl);


typedef uint32_t tdi_dev_id_t;

/*! Identifies a specific die within an ASIC in the system. */
typedef int tdi_subdev_id_t;

/** Identifies a port on an ASIC.  This is a 9-bit field where the upper two
 *  bits identify the pipeline and the lower 7 bits identify the port number
 *  local to that pipeline.  The valid range for the lower 7 bits is 0-71. */
typedef int tdi_dev_port_t;

typedef enum {
  TDI_DEV_DIR_INGRESS = 0,
  TDI_DEV_DIR_EGRESS = 1,
  TDI_DEV_DIR_ALL = 0xff
} tdi_dev_direction_t;

/** Identifies a pipe on an ASIC.  This is a 2-bit field where the bits identify
 * pipeline.
 */
typedef uint32_t tdi_dev_pipe_t;

/**
 * @brief Structure definition for configuration target
 */
typedef struct tdi_target_ {
  /** Device ID */
  tdi_dev_id_t dev_id;
  /** Pipe ID */
  tdi_dev_pipe_t pipe_id;
  /** Gress direction In/Eg */
  tdi_dev_direction_t direction;
  /** Parser ID */
  uint8_t prsr_id;
} tdi_target_t;

/**
 * @brief Get error details string from an error status
 *
 * @param[in] sts Status of type @c tdi_status_t
 * @param[out] err_str Pointer to error string. Doesn't require user to allocate
 *space
 */
//void tdi_err_str(tdi_status_t sts, const char **err_str);

#ifdef __cplusplus
}
#endif

#endif
