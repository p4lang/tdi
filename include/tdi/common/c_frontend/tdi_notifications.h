/*
 * Copyright(c) 2023 Intel Corporation.
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
/** @file tdi_notifications.h
 *
 * @brief Contains TDI Table notifications APIs
 */
#ifndef _TDI_NOTIFICATIONS_H
#define _TDI_NOTIFICATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

tdi_status_t tdi_notifications_set_value(
    tdi_notification_param_hdl *notifications_hdl,
    const tdi_id_t field_id,
    const uint64_t value);

tdi_status_t tdi_notifications_set_value_array(
    tdi_notification_param_hdl *notifications_hdl,
    const tdi_id_t field_id,
    const uint32_t *value,
    const uint32_t array_sz);

tdi_status_t tdi_notifications_get_value(
    const tdi_notification_param_hdl *notifications_hdl,
    const tdi_id_t field_id,
    uint64_t *value);

tdi_status_t tdi_notification_param_get_value_array(
    const tdi_notification_param_hdl *notification_hdl,
    const tdi_id_t field_id,
    uint32_t *val);

#ifdef __cplusplus
}
#endif

#endif  // _TDI_NOTIFICATIONS_H
