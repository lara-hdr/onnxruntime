// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "core/framework/ort_event.h"

ORT_API_STATUS_IMPL(OrtWaitAndCloseEvent, ORT_EVENT finish_event) {
  std::unique_lock<onnxruntime::OrtMutex> l(finish_event->finish_event_mutex);
  while (!finish_event->finished)
    finish_event->finish_event_data.wait(l);
  delete finish_event;
  return nullptr;
}

ORT_EXPORT void ORT_API_CALL OrtSignalEvent(ORT_EVENT ort_event) NO_EXCEPTION {    
  {
    std::lock_guard<onnxruntime::OrtMutex> l(ort_event->finish_event_mutex);
    ort_event->finished = true;
  }
  ort_event->finish_event_data.notify_all();  
}

ORT_API_STATUS_IMPL(OrtCreateEvent, ORT_EVENT* out) {  
  *out = new OnnxRuntimeEvent();
  return nullptr;
}