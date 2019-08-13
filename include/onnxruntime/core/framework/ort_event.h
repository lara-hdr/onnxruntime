// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "core/common/common.h"
#include "core/platform/ort_mutex.h"
#include "core/session/onnxruntime_c_api.h"

//TODO: export it through C API to support OrtAsyncRun

struct OnnxRuntimeEvent {
 public:
  onnxruntime::OrtMutex finish_event_mutex;
  onnxruntime::OrtCondVar finish_event_data;
  bool finished = false;
  OnnxRuntimeEvent() = default;

  ORT_DISALLOW_COPY_ASSIGNMENT_AND_MOVE(OnnxRuntimeEvent);
};

using ORT_EVENT = OnnxRuntimeEvent*;

ORT_API_STATUS(OrtWaitAndCloseEvent, ORT_EVENT finish_event);
ORT_API(void, OrtSignalEvent, ORT_EVENT finish_event);
ORT_API_STATUS(OrtCreateEvent, ORT_EVENT* out);