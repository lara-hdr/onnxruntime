// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "core/common/common.h"
#include "core/platform/ort_mutex.h"
#include "core/session/onnxruntime_c_api.h"

//ORT_EVENT is a simple object that:
//1. It's either signaled or not signaled.
//2. By default, it's not signaled
//3. Use the OrtSignalEvent function to change it to a signaled state, no way to switch the state back.
//4. Not reusable.
//On Windows, it can be used with IOCP(and also the win32 thread pool). Which means, in the same thread pool, you could submit a task and wait it to be finished, without having deadlock risk when all the threads are occupied.
#ifdef _WIN32
using ORT_EVENT = void*;  //WIN32 handle
#else
struct OnnxRuntimeEvent {
 public:
  onnxruntime::OrtMutex finish_event_mutex;
  onnxruntime::OrtCondVar finish_event_data;
  bool finished = false;
  OnnxRuntimeEvent() = default;

  ORT_DISALLOW_COPY_ASSIGNMENT_AND_MOVE(OnnxRuntimeEvent);
};

using ORT_EVENT = OnnxRuntimeEvent*;
#endif

//TODO: move them to c_api.h when it's ready to publish
ORT_API_STATUS(OrtWaitAndCloseEvent, ORT_EVENT finish_event);
ORT_API(void, OrtReleaseEvent, _Frees_ptr_opt_ ORT_EVENT finish_event);
ORT_API(void, OrtSignalEvent, ORT_EVENT finish_event);
ORT_API_STATUS(OrtCreateEvent, ORT_EVENT* out);