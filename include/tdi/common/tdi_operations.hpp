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
/** @file tdi_table_operations.hpp
 *
 *  @brief Contains TDI Table Operations APIs
 */
#ifndef _TDI_TABLE_OPERATIONS_HPP
#define _TDI_TABLE_OPERATIONS_HPP

#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <memory>
#include <unordered_map>
#include <functional>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/tdi_session.hpp>

namespace tdi {

class Table;

class TableOperations {
public:
    virtual ~TableOperations() = default;
    TableOperations(const Table* table, const tdi_operations_type_e &oper_type)
        : table_(table), oper_type_(oper_type){};
    tdi_status_t setValue(tdi_operations_field_type_e  /*type*/, const uint64_t & /*value*/) { return TDI_NOT_SUPPORTED; };
    tdi_status_t getValue(tdi_operations_field_type_e  /*type*/, uint64_t * /*value*/) {return TDI_NOT_SUPPORTED;};
private:
    const Table* table_;
    tdi_operations_type_e oper_type_;
};

}  // tdi

#endif  // _TDI_TABLE_OPERATIONS_HPP
