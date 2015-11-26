// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "realsense/enhanced_photography/win/enhanced_photography_instance.h"

#include <string>

// This file is auto-generated by enhanced_photography.idl
#include "enhanced_photography.h"  // NOLINT
// This file is auto-generated by paster.idl
#include "paster.h" // NOLINT
// This file is auto-generated by photo_utils.idl
#include "photo_utils.h" // NOLINT

#include "base/json/json_string_value_serializer.h"
#include "realsense/enhanced_photography/win/enhanced_photography_object.h"
#include "realsense/enhanced_photography/win/depth_photo_object.h"
#include "realsense/enhanced_photography/win/paster_object.h"
#include "realsense/enhanced_photography/win/photo_capture_object.h"
#include "realsense/enhanced_photography/win/photo_utils_object.h"

namespace realsense {
namespace enhanced_photography {

using namespace xwalk::common; // NOLINT
using jsapi::enhanced_photography::EnhancedPhotographyConstructor::Params;

EnhancedPhotographyInstance::EnhancedPhotographyInstance()
    : handler_(this),
      store_(&handler_),
      ep_ext_thread_("EPExtensionThread") {
  ep_ext_thread_.Start();
  handler_.Register("enhancedPhotographyConstructor",
      base::Bind(&EnhancedPhotographyInstance::OnEnhancedPhotographyConstructor,
                 base::Unretained(this)));
  handler_.Register("depthPhotoConstructor",
      base::Bind(&EnhancedPhotographyInstance::OnDepthPhotoConstructor,
                 base::Unretained(this)));
  handler_.Register("pasterConstructor",
      base::Bind(&EnhancedPhotographyInstance::OnPasterConstructor,
                 base::Unretained(this)));
  handler_.Register("photoCaptureConstructor",
      base::Bind(&EnhancedPhotographyInstance::OnPhotoCaptureConstructor,
                 base::Unretained(this)));
  handler_.Register("photoUtilsConstructor",
      base::Bind(&EnhancedPhotographyInstance::OnPhotoUtilsConstructor,
                 base::Unretained(this)));
}

EnhancedPhotographyInstance::~EnhancedPhotographyInstance() {
  ep_ext_thread_.Stop();
}

void EnhancedPhotographyInstance::HandleMessage(const char* msg) {
  JSONStringValueDeserializer str_deserializer(msg);

  int error_code = 0;
  std::string error_message;
  scoped_ptr<base::Value> value(
      str_deserializer.Deserialize(&error_code, &error_message));
  CHECK(value.get());
  CHECK_EQ(0, error_code);
  CHECK(error_message.empty());

  ep_ext_thread_.message_loop()->PostTask(
      FROM_HERE,
      base::Bind(&EnhancedPhotographyInstance::OnHandleMessage,
                 base::Unretained(this),
                 base::Passed(&value)));
}

void EnhancedPhotographyInstance::HandleBinaryMessage(
    const char* msg, const size_t size) {
  scoped_ptr<base::Value> value = scoped_ptr<base::Value>(
      base::BinaryValue::CreateWithCopiedBuffer(msg, size));

  ep_ext_thread_.message_loop()->PostTask(
    FROM_HERE,
    base::Bind(&EnhancedPhotographyInstance::OnHandleBinaryMessage,
               base::Unretained(this),
               base::Passed(&value)));
}

void EnhancedPhotographyInstance::HandleSyncMessage(const char* msg) {
  JSONStringValueDeserializer str_deserializer(msg);

  int error_code = 0;
  std::string error_message;
  scoped_ptr<base::Value> value(
      str_deserializer.Deserialize(&error_code, &error_message));
  CHECK(value.get());
  CHECK_EQ(0, error_code);
  CHECK(error_message.empty());

  ep_ext_thread_.message_loop()->PostTask(
      FROM_HERE,
      base::Bind(&EnhancedPhotographyInstance::OnHandleSyncMessage,
                 base::Unretained(this),
                 base::Passed(&value)));
}

void EnhancedPhotographyInstance::OnHandleMessage(scoped_ptr<base::Value> msg) {
  DCHECK_EQ(ep_ext_thread_.message_loop(), base::MessageLoop::current());
  handler_.HandleMessage(msg.Pass());
}

void EnhancedPhotographyInstance::OnHandleBinaryMessage(
    scoped_ptr<base::Value> msg) {
  DCHECK_EQ(ep_ext_thread_.message_loop(), base::MessageLoop::current());
  handler_.HandleBinaryMessage(msg.Pass());
}

void EnhancedPhotographyInstance::OnHandleSyncMessage(
    scoped_ptr<base::Value> msg) {
  DCHECK_EQ(ep_ext_thread_.message_loop(), base::MessageLoop::current());
  handler_.HandleSyncMessage(msg.Pass());
}

void EnhancedPhotographyInstance::OnEnhancedPhotographyConstructor(
    scoped_ptr<XWalkExtensionFunctionInfo> info) {
  DCHECK_EQ(ep_ext_thread_.message_loop(), base::MessageLoop::current());
  scoped_ptr<enhanced_photography::EnhancedPhotographyConstructor::
      Params>params(enhanced_photography::EnhancedPhotographyConstructor::
          Params::Create(*info->arguments()));

  scoped_ptr<BindingObject> obj(new EnhancedPhotographyObject(this));
  store_.AddBindingObject(params->object_id, obj.Pass());
}

void EnhancedPhotographyInstance::OnDepthPhotoConstructor(
    scoped_ptr<XWalkExtensionFunctionInfo> info) {
  DCHECK_EQ(ep_ext_thread_.message_loop(), base::MessageLoop::current());
  scoped_ptr<jsapi::depth_photo::DepthPhotoConstructor::
      Params> params(jsapi::depth_photo::DepthPhotoConstructor::
          Params::Create(*info->arguments()));

  scoped_ptr<BindingObject> obj(new DepthPhotoObject(this));
  store_.AddBindingObject(params->object_id, obj.Pass());
}

void EnhancedPhotographyInstance::OnPasterConstructor(
    scoped_ptr<XWalkExtensionFunctionInfo> info) {
  DCHECK_EQ(ep_ext_thread_.message_loop(), base::MessageLoop::current());
  scoped_ptr<base::Value> result(new base::FundamentalValue(false));
  scoped_ptr<jsapi::paster::PasterConstructor::
      Params> params(jsapi::paster::PasterConstructor::
          Params::Create(*info->arguments()));

  if (!params) {
    info->PostResult(result.Pass());
    return;
  }

  DepthPhotoObject* depthPhotoObject = static_cast<DepthPhotoObject*>(
      store_.GetBindingObjectById(params->photo.object_id));
  if (!depthPhotoObject || !depthPhotoObject->GetPhoto()) {
    info->PostResult(result.Pass());
    return;
  }

  scoped_ptr<BindingObject> obj(
      new PasterObject(this, depthPhotoObject->GetPhoto()));
  store_.AddBindingObject(params->object_id, obj.Pass());
  result.reset(new base::FundamentalValue(true));
  info->PostResult(result.Pass());
}

void EnhancedPhotographyInstance::OnPhotoCaptureConstructor(
  scoped_ptr<XWalkExtensionFunctionInfo> info) {
  DCHECK_EQ(ep_ext_thread_.message_loop(), base::MessageLoop::current());
  scoped_ptr<jsapi::photo_capture::PhotoCaptureConstructor::
      Params> params(jsapi::photo_capture::PhotoCaptureConstructor::
          Params::Create(*info->arguments()));

  scoped_ptr<BindingObject> obj(new PhotoCaptureObject(this));
  store_.AddBindingObject(params->object_id, obj.Pass());
}

void EnhancedPhotographyInstance::OnPhotoUtilsConstructor(
  scoped_ptr<XWalkExtensionFunctionInfo> info) {
  DCHECK_EQ(ep_ext_thread_.message_loop(), base::MessageLoop::current());
  scoped_ptr<jsapi::photo_utils::PhotoUtilsConstructor::
      Params> params(jsapi::photo_utils::PhotoUtilsConstructor::
          Params::Create(*info->arguments()));

  scoped_ptr<BindingObject> obj(new PhotoUtilsObject(this));
  store_.AddBindingObject(params->object_id, obj.Pass());
}

void EnhancedPhotographyInstance::AddBindingObject(const std::string& object_id,
    scoped_ptr<BindingObject> obj) {
  store_.AddBindingObject(object_id, obj.Pass());
}

BindingObject* EnhancedPhotographyInstance::GetBindingObjectById(
    const std::string& object_id) {
  return store_.GetBindingObjectById(object_id);
}

}  // namespace enhanced_photography
}  // namespace realsense
