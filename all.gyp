# Copyright (c) 2015 Intel Corporation. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'all_extensions',
      'type': 'none',
      'dependencies': [
        'benchmarks/benchmarks.gyp:*',
        'realsense/realsense.gyp:*',
      ]
    },
  ],
}
