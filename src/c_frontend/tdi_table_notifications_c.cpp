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
#ifdef __cplusplus
extern "C" {
#endif
#include <tdi/common/tdi_defs.h>

#include <tdi/common/c_frontend/tdi_notifications.h>

#ifdef __cplusplus
}
#endif
#include <tdi/common/tdi_notifications.hpp>

tdi_status_t tdi_notifications_set_value(tdi_notification_param_hdl *notifications_hdl,
                                           const tdi_id_t field_id,
                                      const uint64_t value) {
  auto notification_param = reinterpret_cast<tdi::NotificationParams *>(notifications_hdl);
  return notification_param->setValue(field_id, value);
}

tdi_status_t tdi_notifications_set_value_array(tdi_notification_param_hdl *notifications_hdl,
                                             const tdi_id_t field_id,
                                             const uint32_t *value,
                                             const uint32_t array_sz) {
  auto notification_param = reinterpret_cast<tdi::NotificationParams *>(notifications_hdl);
  const auto vec = std::vector<uint64_t>(value, value + array_sz);
  return notification_param->setValue(field_id, vec);
}

tdi_status_t tdi_notifications_get_value(const tdi_notification_param_hdl *notifications_hdl,
                                           const tdi_id_t field_id,
                                      uint64_t *value) {
  auto notification_param = reinterpret_cast<const tdi::NotificationParams *>(notifications_hdl);
  return notification_param->getValue(field_id, value);
}

tdi_status_t tdi_notification_param_get_value_array(
    const tdi_notification_param_hdl *notification_hdl,
    const tdi_id_t field_id,
    uint32_t *val) {
  auto notification_param = reinterpret_cast<const tdi::NotificationParams *>(notification_hdl);
  std::vector<uint64_t> vec;
  auto status = notification_param->getValue(field_id, &vec);
  int i = 0;
  for (auto const &item : vec) {
    val[i++] = item;
  }
  return status;
}

tdi_status_t tdi_notification_param_get_value_array_size(
    const tdi_notification_param_hdl *notification_hdl,
    const tdi_id_t field_id,
    uint32_t *array_size) {
  auto notification_param = reinterpret_cast<const tdi::NotificationParams *>(notification_hdl);
  std::vector<uint64_t> vec;
  auto status = notification_param->getValue(field_id, &vec);
  *array_size = vec.size();
  return status;
}
