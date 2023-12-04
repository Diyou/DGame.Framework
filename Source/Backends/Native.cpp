/**
 * Native.cpp- DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __EMSCRIPTEN__

#include "DGame/Context.h"
#include "SDLWindow.h"
#include "dawn/dawn_proc.h"
#include "dawn/native/DawnNative.h"

#include <sstream>
#include <thread>

using namespace std;
using namespace chrono;
using namespace wgpu;
using namespace dawn;

namespace DGame {
static vector<string> enableToggles;
static vector<string> disableToggles;

constexpr TextureFormat
GetPreferredSwapChainTextureFormat()
{
  // TODO(https://bugs.chromium.org/p/dawn/issues/detail?id=1362):
  // Return the adapter's preferred format when implemented.
  return TextureFormat::BGRA8Unorm;
}

constexpr wgpu::AdapterType adapterType = AdapterType::DiscreteGPU;

void
PrintDeviceError(WGPUErrorType errorType, const char *message, void *)
{
  const char *errorTypeName = "";
  switch(errorType)
  {
  case WGPUErrorType_Validation:
    errorTypeName = "Validation";
    break;
  case WGPUErrorType_OutOfMemory:
    errorTypeName = "Out of memory";
    break;
  case WGPUErrorType_Unknown:
    errorTypeName = "Unknown";
    break;
  case WGPUErrorType_DeviceLost:
    errorTypeName = "Device lost";
    break;
  default:
    errorTypeName = "Unknown";
    return;
  }
  cerr << errorTypeName << " error: " << message;
}

void
DeviceLostCallback(
  WGPUDeviceLostReason reason,
  const char *message,
  void *userData
)
{
  cerr << "Device lost: " << message;
}

void
PrintGLFWError(int code, const char *message)
{
  cerr << "GLFW error: " << code << " - " << message;
}

void
DeviceLogCallback(WGPULoggingType type, const char *message, void *)
{
  cerr << "Device log: " << message;
}

static DawnProcTable procs = dawn::native::GetProcs();

struct Context::Backend : public Window
{
  unique_ptr<native::Instance> instance;

  /**
   * @brief
   * @param windowTitle
   * @param windowWidth
   * @param windowHeight
   */
  Backend(
    const char *windowTitle,
    int windowWidth,
    int windowHeight,
    int x,
    int y
  )
  : Window(windowTitle, windowWidth, windowHeight, x, y)
  {
    // Create Instance
    InstanceDescriptor descriptor{};
    descriptor.features.timedWaitAnyEnable = true;

    instance = make_unique<dawn::native::Instance>(
      (WGPUInstanceDescriptor *)&descriptor
    );

    // procs.deviceSetUncapturedErrorCallback(device.Get(), PrintDeviceError,
    // nullptr); procs.deviceSetDeviceLostCallback(device.Get(),
    // DeviceLostCallback, nullptr);
    // procs.deviceSetLoggingCallback(device.Get(), DeviceLogCallback, nullptr);
  }

  native::Adapter
  requestAdapter()
  {
    RequestAdapterOptions options{};
    options.backendType = BackendType;

    auto adapters = instance->EnumerateAdapters(&options);
    DawnAdapterPropertiesPowerPreference power_props{};

    AdapterProperties adapterProperties{};
    adapterProperties.nextInChain = &power_props;
    // Find the first adapter which satisfies the adapterType requirement.
    auto isAdapterType = [&adapterProperties](const auto &adapter) -> bool {
      // picks the first adapter when adapterType is unknown.
      if(adapterType == AdapterType::Unknown)
      {
        return true;
      }
      adapter.GetProperties(&adapterProperties);
      stringstream ss;
      ss //<< "Type:" << adapterProperties.adapterType
        << "\nGPU: " << adapterProperties.name
        << "\nDriver: " << adapterProperties.driverDescription;

      cout << ss.str() << endl;
      return adapterProperties.adapterType == adapterType;
    };

    auto adapter = find_if(adapters.begin(), adapters.end(), isAdapterType);
    if(adapter == adapters.end())
    {
      cerr << "Failed to find an adapter! Please try another adapter type."
           << endl;
      return native::Adapter();
    }
    return *adapter;
  }

  Device
  createDevice()
  {
    vector<const char *> enableToggleNames;
    vector<const char *> disabledToggleNames;
    for(const string &toggle: enableToggles)
    {
      enableToggleNames.push_back(toggle.c_str());
    }
    for(const string &toggle: disableToggles)
    {
      disabledToggleNames.push_back(toggle.c_str());
    }

    DawnTogglesDescriptor toggles;
    toggles.sType = SType::DawnTogglesDescriptor;
    toggles.nextInChain = nullptr;
    toggles.enabledToggles = enableToggleNames.data();
    toggles.enabledToggleCount = enableToggleNames.size();
    toggles.disabledToggles = disabledToggleNames.data();
    toggles.disabledToggleCount = disabledToggleNames.size();

    DeviceDescriptor deviceDesc;
    deviceDesc.nextInChain = &toggles;

    return Device::Acquire(requestAdapter().CreateDevice(&deviceDesc));
  }

  Surface
  createSurface()
  {
    auto descriptor = createSurfaceDescriptor();
    return Surface::Acquire(procs.instanceCreateSurface(
      instance->Get(),
      (WGPUSurfaceDescriptor *)&descriptor
    ));
  }

  SwapChain
  createSwapChain(Device device, Surface surface)
  {
    auto size = Size();
    SwapChainDescriptor descriptor;
    descriptor.usage = TextureUsage::RenderAttachment;
    descriptor.format = GetPreferredSwapChainTextureFormat();
    descriptor.width = size.width;
    descriptor.height = size.height;
    descriptor.presentMode = PresentMode::Fifo;

    return SwapChain::Acquire(procs.deviceCreateSwapChain(
      device.Get(),
      surface.Get(),
      (WGPUSwapChainDescriptor *)&descriptor
    ));
  }

  void
  iterate()
  {
    native::InstanceProcessEvents(instance->Get());
  }
};

Context::Context(
  const char *windowTitle,
  int windowWidth,
  int windowHeight,
  std::optional<int> posX,
  std::optional<int> posY
)
: implementation(new Context::Backend(
  windowTitle,
  windowWidth,
  windowHeight,
  posX.has_value() ? posX.value() : SDL_WINDOWPOS_UNDEFINED,
  posY.has_value() ? posY.value() : SDL_WINDOWPOS_UNDEFINED
))
, IsRendering(implementation->isAlive)
{
  device = implementation->createDevice();
  surface = implementation->createSurface();
  swapchain = implementation->createSwapChain(device, surface);
  queue = device.GetQueue();

  device.SetDeviceLostCallback(DeviceLostCallback, this);
}

void
Context::Start()
{
  auto start = std::thread([this]() {
    while(IsRendering)
    {
      implementation->iterate();

      // Render Frame
      draw();
      swapchain.Present();
      this_thread::sleep_for(10ms);
    }
  });
  start.detach();
  SDL_ShowWindow(implementation->window);
}

Context::~Context()
{
  device.SetDeviceLostCallback(nullptr, nullptr);
}

} // namespace DGame
#endif
