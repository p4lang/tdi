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
#ifndef _TDI_TABLE_INFO_INTERNAL_HPP
#define _TDI_TABLE_INFO_INTERNAL_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

/* tdi_includes */

namespace tdi {
namespace tdi_json {

const std::string SCHEMA_VERSION = "schema_version";
const std::string TABLES = "tables";
const std::string TABLE_NAME = "name";
const std::string TABLE_ID = "id";
const std::string TABLE_TYPE = "table_type";
const std::string TABLE_SIZE = "size";
const std::string TABLE_ANNOTATIONS = "annotations";
const std::string TABLE_DEPENDS_ON = "depends_on";
const std::string TABLE_HAS_CONST_DEFAULT_ACTION = "has_const_default_action";
const std::string TABLE_IS_CONST = "is_const";

const std::string TABLE_KEY = "key";
const std::string TABLE_KEY_ID = "id";
const std::string TABLE_KEY_NAME = "name";
const std::string TABLE_KEY_REPEATED = "repeated";
const std::string TABLE_KEY_MANDATORY = "mandatory";
const std::string TABLE_KEY_ANNOTATIONS = "annotations";
const std::string TABLE_KEY_MATCH_TYPE = "match_type";
const std::string TABLE_KEY_TYPE = "type";
const std::string TABLE_KEY_TYPE_TYPE = "type";
const std::string TABLE_KEY_TYPE_WIDTH = "width";

const std::string TABLE_ACTION_SPECS = "action_specs";
const std::string TABLE_ACTION_ID = "id";
const std::string TABLE_ACTION_NAME = "name";
const std::string TABLE_ACTION_ACTION_SCOPE = "action_scope";
const std::string TABLE_ACTION_ANNOTATIONS = "annotations";
const std::string TABLE_ACTION_DATA = "data";

const std::string TABLE_DATA = "data";
const std::string TABLE_DATA_ID = "id";
const std::string TABLE_DATA_NAME = "name";
const std::string TABLE_DATA_REPEATED = "repeated";
const std::string TABLE_DATA_MANDATORY = "mandatory";
const std::string TABLE_DATA_ANNOTATIONS = "annotations";
const std::string TABLE_DATA_TYPE = "type";
const std::string TABLE_DATA_TYPE_TYPE = "type";
const std::string TABLE_DATA_TYPE_WIDTH = "width";
const std::string TABLE_DATA_TYPE_DEFAULT_VALUE = "default_value";

const std::string LEARNS = "learns";
const std::string LEARN_NAME = "name";
const std::string LEARN_ID = "id";
const std::string LEARN_ANNOTATIONS = "annotations";
const std::string LEARN_DATA = "data";

}  // namespace tdi_json
}  // namespace tdi

#endif
