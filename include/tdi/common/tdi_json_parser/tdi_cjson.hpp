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
#ifndef _TDI_CJSON_HPP
#define _TDI_CJSON_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

/* tdi_includes */

#include <target-utils/third-party/cJSON/cJSON.h>

namespace tdi {

class CjsonObjHandler {
 public:
  CjsonObjHandler(const std::string &fileContent);
  ~CjsonObjHandler();
  cJSON *rootGet() { return root; }

 private:
  cJSON *root = nullptr;
};

class Cjson {
 public:
  Cjson(const Cjson &parent, const std::string &key);
  //  Cjson(const std::string &fileContent);
  Cjson(const Cjson &parent, int &index);
  static Cjson createCjsonFromFile(const std::string &fileContent);
  Cjson(){};

  // Copy ctor
  Cjson(const Cjson &node);
  // assignment operator
  Cjson &operator=(const Cjson &other);
  // rule of three need not be followed since the shared_ptr will
  // be destroyed automatically when out of scope

  std::vector<std::shared_ptr<Cjson>> getCjsonChildVec() const;
  std::vector<std::string> getCjsonChildStringVec() const;
  std::string getCjsonKey() const;
  void addObject(const std::string &name, const Cjson &item);
  bool exists() const { return root; };
  uint32_t array_size() const;
  operator int() const;
  operator unsigned int() const;
  operator std::string() const;
  operator bool() const;
  operator uint64_t() const;
  operator float() const;
  Cjson operator[](const char *key) const;
  Cjson operator[](const std::string &key) const;
  Cjson operator[](int index) const;
  Cjson &operator+=(const Cjson &other);
  friend std::ostream &operator<<(std::ostream &out, const Cjson &c);
  void updateChildNode(const std::string &key, const std::string &val);

 private:
  static void createCjsonFromFileInternal(const std::string &fileContent,
                                          Cjson &obj);
  cJSON *root = nullptr;
  std::shared_ptr<CjsonObjHandler> cjson_mem_tracker = nullptr;
};

}  // namespace tdi

#endif
