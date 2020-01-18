// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "winrt_headers.h"
#include <dxgi1_6.h>
#include <initguid.h>
#include <d3d11.h>

#if __has_include("dxcore.h")
#define ENABLE_DXCORE 1
#endif
#ifdef ENABLE_DXCORE
#include <dxcore.h>
#endif

//
// Exception information
//
#ifndef FACILITY_VISUALCPP
#define FACILITY_VISUALCPP ((LONG)0x6d)
#endif

#define VcppException(sev, err) ((sev) | (FACILITY_VISUALCPP << 16) | err)

namespace CommonDeviceHelpers {
struct AdapterEnumerationSupport {
  bool has_dxgi;
  bool has_dxcore;
};

// uses Structured Exception Handling (SEH) to detect for delay load failures of target API.
// You cannot mix and match SEH with C++ exception and object unwinding
// In this case we will catch it, and report up to the caller via HRESULT so our callers can use
// C++ exceptions
template <typename TFunc, typename... TArgs>
HRESULT RunDelayLoadedApi(TFunc& tfunc, TArgs&&... args) {
  __try {
    return tfunc(std::forward<TArgs>(args)...);
  } __except (GetExceptionCode() == VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
    // this could be ok, just let people know that it failed to load
    return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
  }
}

HRESULT GetAdapterEnumerationSupport(AdapterEnumerationSupport* support);
bool IsFloat16Supported(ID3D12Device* device);
bool IsFloat16Supported(const winrt::Windows::AI::MachineLearning::LearningModelDevice& device);
}  // namespace CommonDeviceHelpers