// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "core/framework/ort_event.h"
#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
ORT_API_STATUS_IMPL(OrtCreateEvent, ORT_EVENT* out) {
  HANDLE finish_event = CreateEvent(
      NULL,   // default security attributes
      TRUE,   // manual-reset event
      FALSE,  // initial state is nonsignaled
      NULL);
  if (finish_event == NULL) {
    return OrtCreateStatus(ORT_FAIL, "unable to create new event");
  }
  *out = finish_event;
  return nullptr;
}

ORT_API_STATUS_IMPL(OrtWaitAndCloseEvent, ORT_EVENT finish_event) {
  DWORD dwWaitResult = WaitForSingleObject(finish_event, INFINITE);
  (void)CloseHandle(finish_event);
  if (dwWaitResult != WAIT_OBJECT_0) {
    return OrtCreateStatus(ORT_FAIL, "WaitForSingleObject failed");
  }
  return nullptr;
}

ORT_EXPORT void ORT_API_CALL OrtSignalEvent(ORT_EVENT ort_event) NO_EXCEPTION {
  (void)SetEvent(ort_event);
}

ORT_API(void, OrtReleaseEvent, _Frees_ptr_opt_ ORT_EVENT finish_event) {
  if (finish_event != nullptr) (void)CloseHandle(finish_event);
}
#else
ORT_API_STATUS_IMPL(OrtWaitAndCloseEvent, ORT_EVENT finish_event) {
  {
    std::unique_lock<onnxruntime::OrtMutex> l(finish_event->finish_event_mutex);
    while (!finish_event->finished)
      finish_event->finish_event_data.wait(l);
  }
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

ORT_API(void, OrtReleaseEvent, _Frees_ptr_opt_ ORT_EVENT finish_event) {
  delete finish_event;
}
#endif