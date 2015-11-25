// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REALSENSE_ENHANCED_PHOTOGRAPHY_ENHANCED_PHOTOGRAPHY_INSTANCE_H_
#define REALSENSE_ENHANCED_PHOTOGRAPHY_ENHANCED_PHOTOGRAPHY_INSTANCE_H_

#include <string>

#include "base/threading/thread.h"
#include "base/values.h"
#include "xwalk/common/extension.h"
#include "xwalk/common/binding_object_store.h"
#include "xwalk/common/xwalk_extension_function_handler.h"

namespace realsense {
namespace enhanced_photography {

using xwalk::common::Instance;
using xwalk::common::XWalkExtensionFunctionInfo;

class EnhancedPhotographyInstance : public Instance {
 public:
  EnhancedPhotographyInstance();
  ~EnhancedPhotographyInstance() override;

  // common::Instance implementation.
  void HandleMessage(const char* msg) override;
  void HandleBinaryMessage(const char* msg, const size_t size) override;

  void AddBindingObject(const std::string& object_id,
      scoped_ptr<xwalk::common::BindingObject> obj);

  xwalk::common::BindingObject* GetBindingObjectById(
      const std::string& object_id);

 private:
  void OnHandleMessage(scoped_ptr<base::Value> msg);
  void OnHandleBinaryMessage(scoped_ptr<base::Value> msg);
  void OnEnhancedPhotographyConstructor(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnDepthPhotoConstructor(scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnPhotoUtilsConstructor(scoped_ptr<XWalkExtensionFunctionInfo> info);

  xwalk::common::XWalkExtensionFunctionHandler handler_;
  xwalk::common::BindingObjectStore store_;
  base::Thread ep_ext_thread_;
};

}  // namespace enhanced_photography
}  // namespace realsense

#endif  // REALSENSE_ENHANCED_PHOTOGRAPHY_ENHANCED_PHOTOGRAPHY_INSTANCE_H_
