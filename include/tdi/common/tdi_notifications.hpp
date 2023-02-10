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
/** @file tdi_table_notifications.hpp
 *
 *  @brief Contains TDI Table Notifications APIs
 */
#ifndef _TDI_TABLE_NOTIFICATIONS_HPP
#define _TDI_TABLE_NOTIFICATIONS_HPP

#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_table_key.hpp>
#include <tdi/common/tdi_session.hpp>
#include <tdi/common/tdi_table_data.hpp>

namespace tdi {

class Table;

class NotificationOutParams {
public:
    virtual ~NotificationOutParams() = default;
    NotificationOutParams(const Table* table, const tdi_id_t &tdi_id_type)
        : table_(table), notification_id_(tdi_id_type){};

    virtual tdi_status_t setValue(const tdi_id_t & /*field_id*/,
                                  const uint64_t & /*value*/) const {
      return TDI_NOT_SUPPORTED;
    };
    virtual tdi_status_t getValue(const tdi_id_t & /*field_id*/,
	                              uint64_t * /*value*/)  const {
      return TDI_NOT_SUPPORTED;
    };

    virtual tdi_status_t setValue(const tdi_id_t & /*field_id*/,
                                  const std::vector<uint8_t> & /*value*/) const {
      return TDI_NOT_SUPPORTED; 
	};
    virtual tdi_status_t getValue(const tdi_id_t & /*field_id*/,
                                  std::vector<uint8_t> * /*value*/) const {
      return TDI_NOT_SUPPORTED;
	};

    virtual tdi_status_t getValue(const tdi_id_t & /*field_id*/,
                                  uint8_t*const* /*value*/,
                                  size_t * /*size*/) const {
      return TDI_NOT_SUPPORTED;
    };

 private:
    const Table* table_; 
    tdi_id_t notification_id_;
};

typedef std::function<void(std::unique_ptr<tdi::TableKey> key,
                           std::unique_ptr<tdi::TableData> data,
                           std::unique_ptr<tdi::NotificationOutParams> params,
                           void *cookie)> tdiNotificationCallback;
}  // tdi

#endif  // _TDI_TABLE_OPERATIONS_HPP
