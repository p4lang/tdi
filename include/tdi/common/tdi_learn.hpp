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
/** @file tdi_learn.hpp
 *
 *  @brief Contains TDI Learn Object APIs
 */
#ifndef _TDI_LEARN_HPP
#define _TDI_LEARN_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

#include <tdi/common/tdi_defs.h>
#include <tdi/common/c_frontend/tdi_learn.h>
#include <tdi/common/tdi_target.hpp>
#include <tdi/common/tdi_table_data.hpp>
#include <tdi/common/tdi_session.hpp>
#include <tdi/common/tdi_utils.hpp>
namespace tdi {

/**
 * @brief Only the below functions in comments are supported for tdi::LearnData.
 * \n Use of anything else will return a BF_NOT_SUPPORTED error
 * \n tdi::TableData::getValue(const tdi_id_t &field_id, uint64_t *val) const;
 * \n tdi::TableData::getValue(const tdi_id_t &field_id, const size_t &size,
 *                     uint8_t *val) const;
 */
using LearnData = TableData;

class LearnInfo;

/**
 * @brief Learn Callback Function. This gets called upon receiving learn digests
 * from the device. Onlye one cb can be associated with a Learn object at a
 *time.
 * If a session already has registered, then another session cannot overwrite
 *
 * @param[in] tdi_tgt Bf Rt target associated with the learn data
 * @param[in] session @c std::shared_ptr to the session
 * @param[in] learnData Vector of learn data objs
 * @param[in] learn_msg_hdl Handle for the msg which can be used to notify ack
 * @param[in] cookie Cookie registered
 */
typedef std::function<tdi_status_t(
    const tdi::Target &tdi_tgt,
    const std::shared_ptr<tdi::Session> session,
    std::vector<std::unique_ptr<tdi::LearnData>> learnDataVec,
    tdi_learn_msg_hdl *const learn_msg_hdl,
    const void *cookie)> tdiCbFunction;

// from tdi_learn_impl.hpp
class LearnField {
 public:
  LearnField(tdi_id_t id, size_t s, std::string n, size_t off);
  ~LearnField() = default;

  tdi_id_t getFieldId() const { return field_id; };
  const std::string &getName() const { return name; };
  const size_t &getSize() const { return size; };
  const size_t &getOffset() const { return offset; };
  const bool &getIsPtr() const { return is_ptr; };

 private:
  tdi_id_t field_id;
  size_t size;
  std::string name;
  bool is_ptr;
  size_t offset;
};

/**
 * @brief Class to contain metadata of Learn Obj and perform functions
 *  like register and deregister Learn Callback <br>
 * <B>Creation: </B> Cannot be created. can only be queried from \ref
 * tdi::TdiInfo
 */
class Learn {
 public:
  virtual ~Learn() = default;

  /**
   * @brief Register Callback function to be called on a Learn event
   *
   * @param[in] session @c std::shared_ptr to the session. shared_ptr is used
   * in all of learn APIs because the callbacks return back a valid session
   * object as well. So it is required for us to make sure that the session
   * exists when the callback is being called. Hence the necessity to create
   * a shared_ptr to share ownership
   * @param[in] dev_tgt Device target
   * @param[in] callback_fn Callback function to be called upon incurring a
   *learn event
   * @param[in] cookie Optional cookie to be received with the callback
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tdiLearnCallbackRegister(
      const std::shared_ptr<tdi::Session> /*session*/,
      const tdi_target_t * /*dev_tgt*/,
      const tdi_cb_function /*callback_c*/,
      //const tdiCbFunction & /*callback_fn*/,
      const void * /*cookie*/) const {
    return TDI_SUCCESS;
  };
  virtual tdi_status_t tdiLearnCallbackRegisterHelper(
      const std::shared_ptr<tdi::Session> /*session*/,
      const Target & /*dev_tgt*/,
      const tdiCbFunction & /*callback_cpp*/,
      const tdi_cb_function /*callback_c*/,
      const void */*cookie*/) const {
    return TDI_SUCCESS;
  }

  /**
   * @brief Deregister the callback from the device
   *
   * @param[in] session @c std::shared_ptr to the session
   * @param[in] dev_tgt Device target
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tdiLearnCallbackDeregister(
      const std::shared_ptr<tdi::Session> /*session*/,
      const tdi_target_t * /*dev_tgt*/) const {
    /*
    if (session==nullptr) {
      LOG_ERROR("invalid session %d", session);
      return TDI_INVALID_ARG;
    }
    */
    //uint32_t value;
    //dev_tgt.getValue(TDI_TARGET_CORE, &value);
    //LOG_DBG("invalid session %d", dev_tgt);
    return TDI_SUCCESS;
  };

  /**
   * @brief Notify the device that the learn_msg was received and
   * processed. This is required to be done in order to free up digest
   * related resources in the device
   *
   * @param[in] session Shared_ptr to the session
   * @param[in] learn_msg_hdl Handle of the msg to be notified
   *
   * @return Status of the API call
   */
  virtual tdi_status_t tdiLearnNotifyAck(
      const std::shared_ptr<tdi::Session> /*session*/,
      tdi_learn_msg_hdl *const /*learn_msg_hdl*/) const {
    return TDI_SUCCESS;
  };

  const LearnInfo &learnInfoGet() const { return *(learn_info_.get()); }

  // following from LearnObj
  // hidden functions
  const LearnField *getLearnField(const tdi_id_t &field_id) const;
  size_t getLearnMsgSize() const { return learn_msg_size; };
  uint32_t learnFieldListSize() const { return lrn_fields.size(); };

 private:
  std::unique_ptr<LearnInfo> learn_info_;
  // from LearnObj
  std::map<tdi_id_t, std::unique_ptr<LearnField>> lrn_fields;
  size_t learn_msg_size;
};

class LearnFieldInfo {
 public:
  LearnFieldInfo(tdi_id_t id, size_t s, std::string n);
  ~LearnFieldInfo() = default;

  /**
   * @brief Get the size of the Learn Data Field
   *
   * @param[in] field_id Data field ID
   * @param[out] size Size of the Data field
   *
   * @return Status of the API call
   */
  tdi_status_t learnFieldSizeGet(size_t *size) const;

  /**
   * @brief Find out whether the Learn Data field is a pointer or not
   *
   * @param[in] field_id Data field ID
   * @param[out] is_ptr Is it a pointer?
   *
   * @return Status of the API call
   */
  tdi_status_t learnFieldIsPtrGet(bool *is_ptr) const;

  /**
   * @brief Get the Name of the Learn Data field
   *
   * @param[in] field_id Data Field ID
   * @param[out] name Data Field Name
   *
   * @return Status of the API call
   */
  tdi_status_t learnFieldNameGet(std::string *name) const;

  /**
   * @brief Get the ID of the Learn Data field
   *
   * @param[out] ID Data Field ID
   *
   * @return Status of the API call
   */
  tdi_status_t learnFieldIdGet(tdi_id_t *field_id) const;

  const std::string &getName() const { return name_; };
  const tdi_id_t &getFieldId() const { return field_id_; };

 private:
  tdi_id_t field_id_;
  size_t size_;
  std::string name_;
  bool is_ptr_;
};

class LearnInfo {
 public:
  /**
   * @brief Get ID of the learn Object
   *
   * @param [out] id ID of the learn Object
   *
   * @return Status of the API call
   */
  tdi_status_t learnIdGet(tdi_id_t *id) const;

  /**
   * @brief Get Name of the learn Object
   *
   * @param [out] name Name of the learn Object
   *
   * @return Status of the API call
   */
  tdi_status_t learnNameGet(std::string *name) const;

  /**
   * @brief Get the list of IDs of the Data Fields associated with
   *    the Learn obj
   *
   * @param[out] id_vec Vector of IDs to contain the list of IDs
   *
   * @return Status of the API call
   */
  tdi_status_t learnFieldIdListGet(std::vector<tdi_id_t> *id_vec) const;

  /**
   * @brief Get the ID of the Learn data field
   *
   * @param[in] name Data field Name
   * @param[out] field_id Field ID of the Data field
   *
   * @return Status of the API call
   */
  tdi_status_t learnFieldIdGet(const std::string &name,
                               tdi_id_t *field_id) const;

 private:
  tdi_id_t learn_id_;
  std::string learn_name_;
  std::map<tdi_id_t, std::unique_ptr<const LearnFieldInfo>> lrn_fields_;
  size_t learn_msg_size_;
};

}  // namespace tdi

#endif
