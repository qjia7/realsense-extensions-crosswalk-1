// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REALSENSE_SCENE_PERCEPTION_WIN_SCENE_PERCEPTION_OBJECT_H_
#define REALSENSE_SCENE_PERCEPTION_WIN_SCENE_PERCEPTION_OBJECT_H_

#include <string>
#include <vector>

// This file is auto-generated by scene_perception.idl
#include "scene_perception.h" // NOLINT

#include "base/message_loop/message_loop_proxy.h"
#include "base/time/time.h"
#include "base/threading/thread.h"
#include "third_party/libpxc/include/pxcsceneperception.h"
#include "third_party/libpxc/include/pxcsensemanager.h"
#include "xwalk/common/event_target.h"

namespace realsense {
namespace scene_perception {

using namespace realsense::jsapi::scene_perception; // NOLINT
using xwalk::common::XWalkExtensionFunctionInfo; // NOLINT

class ScenePerceptionObject : public xwalk::common::EventTarget {
 public:
  ScenePerceptionObject();
  ~ScenePerceptionObject() override;

  // EventTarget implementation.
  void StartEvent(const std::string& type) override;
  void StopEvent(const std::string& type) override;

 private:
  // Controllers.
  void OnInit(scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnStart(scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnStop(scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnReset(scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnDestroy(scoped_ptr<XWalkExtensionFunctionInfo> info);

  // Configuration changing APIs.
  void OnEnableReconstruction(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnEnableRelocalization(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnSetMeshingResolution(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnSetMeshingThresholds(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnSetCameraPose(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnSetMeshingUpdateConfigs(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnConfigureSurfaceVoxelsData(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnSetMeshingRegion(
      scoped_ptr<XWalkExtensionFunctionInfo> info);

  // Data and configurations getting APIs.
  void OnGetSample(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnQueryVolumePreview(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnGetVertices(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnGetNormals(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnIsReconstructionEnabled(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnGetVoxelResolution(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnGetVoxelSize(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnGetMeshingThresholds(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnGetMeshingResolution(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnGetMeshData(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnGetSurfaceVoxels(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnSaveMesh(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnClearMeshingRegion(
      scoped_ptr<XWalkExtensionFunctionInfo> info);

  // Run on sensemanager_thread_
  void OnCreateAndStartPipeline(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void applyInitialConfigs(
      InitialConfiguration* jsconfig);
  void OnStopAndDestroyPipeline(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnRunPipeline();
  void OnResetScenePerception(
      scoped_ptr<XWalkExtensionFunctionInfo> info);

  void DoPauseScenePerception(
      bool pause,
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoEnableReconstruction(
      bool enable,
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoEnableRelocalization(
      bool enable,
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoSetMeshingResolution(
      PXCScenePerception::MeshResolution resolution,
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoSetMeshingThresholds(
      float max, float avg,
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoSetCameraPose(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoCopySample(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoCopyVertices(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoCopyNormals(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoCopyVerticesOrNormals(scoped_ptr<uint8[]> data);

  void DoCheckReconstructionFlag(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoGetMeshingThresholds(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoGetMeshingResolution(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void OnMeshingResult();
  void ReleaseResources();
  void DoQueryVolumePreview(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoGetMeshData(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoSaveMesh(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoConfigureSurfaceVoxelsData(
      scoped_ptr<XWalkExtensionFunctionInfo> info);
  void DoGetSurfaceVoxels(
      scoped_ptr<XWalkExtensionFunctionInfo> info);

  // Run on meshing_thread_
  void DoMeshingUpdateOnMeshingThread(
      scoped_ptr<XWalkExtensionFunctionInfo> info);

  void StopSceneManagerThread();
  // Run on existing extension thread.
  void OnStopSceneManagerThread();

  void triggerError(std::string msg);

 private:
  enum State {
    // Before the pipeline is initialized.
    IDLE,

    // Pipeline is initialized, SP module is paused.
    // Possible output in this state:
    //     checking event(depth quality)
    //
    //     getSample(raw color/depth image)
    INITIALIZED,

    // SP module starts to work, receiving raw smaples and reconstructing the
    // volume space according to them.
    // Typically this state is triggerred when the depth quality
    // is acceptable.
    // SP module is on tracking, and on reconstructing if reconstruction
    // flag enabled.
    // Possible output in this state:
    //     sampleProcessed event(quality, tracking accuracy, camera pose)
    //     meshupdated event(no data)
    //
    //     getSample(processed sample including color/depth image)
    //     getCameraPose
    //     getVertices
    //     getNormals
    //     getMeshData
    STARTED
  };

  State state_;

  bool checking_event_on_;
  bool sampleprocessed_event_on_;
  bool meshupdated_event_on_;

  bool doing_meshing_updating_;
  base::Thread sensemanager_thread_;
  base::Thread meshing_thread_;
  scoped_refptr<base::MessageLoopProxy> message_loop_;

  // Initial configurations.
  int color_image_width_;
  int color_image_height_;
  float color_capture_framerate_;
  int depth_image_width_;
  int depth_image_height_;
  float depth_capture_framerate_;
  int max_block_mesh_;
  int max_faces_;
  int max_vertices_;
  bool b_use_color_;

  // Configurations for surface voxels data.
  int voxel_count_;
  bool voxel_use_color_;

  base::TimeTicks last_meshing_time_;

  PXCSession* session_;
  PXCSenseManager* sense_manager_;
  PXCScenePerception* scene_perception_;
  PXCScenePerception::ScenePerceptionIntrinsics sp_intrinsics_;

  PXCBlockMeshingData* block_meshing_data_;
  // All actions on surface_voxels_data_
  // should be taken on sensemanager_thread_;
  PXCSurfaceVoxelsData* surface_voxels_data_;
  PXCScenePerception::MeshingUpdateInfo  meshing_update_info_;
  pxcBool b_fill_holes_;

  PXCImage* latest_color_image_;
  PXCImage* latest_depth_image_;
  scoped_ptr<uint8[]> latest_vertices_;
  scoped_ptr<uint8[]> latest_normals_;

  scoped_ptr<uint8[]> sample_message_;
  size_t sample_message_size_;

  scoped_ptr<uint8[]> volume_preview_message_;
  size_t volume_preview_message_size_;

  scoped_ptr<uint8[]> vertices_normals_message_;
  size_t vertices_normals_message_size_;

  scoped_ptr<uint8[]> meshing_data_message_;
  size_t meshing_data_message_size_;
};

}  // namespace scene_perception
}  // namespace realsense

#endif  // REALSENSE_SCENE_PERCEPTION_WIN_SCENE_PERCEPTION_OBJECT_H_
