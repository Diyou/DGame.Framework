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
#include "SDL/SDLWindow.h"
#include "dawn/dawn_proc.h"
#include "dawn/native/DawnNative.h"

#include <sstream>
#include <thread>

using namespace std;
using namespace chrono;
using namespace wgpu;
using namespace dawn;

namespace DGame {

constexpr AdapterType adapterType = AdapterType::DiscreteGPU;

static vector<string> enableToggles;
static vector<string> disableToggles;

constexpr TextureFormat
GetPreferredSwapChainTextureFormat()
{
  // TODO(https://bugs.chromium.org/p/dawn/issues/detail?id=1362):
  // Return the adapter's preferred format when implemented.
  return TextureFormat::BGRA8Unorm;
}

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
DeviceLogCallback(WGPULoggingType type, const char *message, void *)
{
  cerr << "Device log: " << message;
}

static DawnProcTable procs = native::GetProcs();

struct Context::Backend : public Window
{
  unique_ptr<native::Instance> instance;
  native::Adapter adapter;

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

    instance
      = make_unique<native::Instance>((WGPUInstanceDescriptor *)&descriptor);

    adapter = requestAdapter();

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

    return Device::Acquire(adapter.CreateDevice(&deviceDesc));
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
  createSwapChain(
    Device &device,
    Surface &surface,
    SwapChainDescriptor &descriptor
  )
  {
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
, SWDescriptor{}
{
  device = implementation->createDevice();
  surface = implementation->createSurface();

  SWDescriptor.usage = TextureUsage::RenderAttachment;
  SWDescriptor.format = GetPreferredSwapChainTextureFormat();
  SWDescriptor.width = windowWidth;
  SWDescriptor.height = windowHeight;
  SWDescriptor.presentMode = PresentMode::Immediate;

  swapchain = implementation->createSwapChain(device, surface, SWDescriptor);
  queue = device.GetQueue();

  device.SetDeviceLostCallback(DeviceLostCallback, this);

  implementation->WindowClosed.connect(bind(&Context::Close, this));
}

void
Context::Start()
{
  SDL_ShowWindow(implementation->window);
  while(IsRendering)
  {
    implementation->iterate();
    // Render Frame
    draw();
    swapchain.Present();
    if(SWDescriptor.presentMode == PresentMode::Immediate)
    {
      this_thread::sleep_for(milliseconds(1000 / FPSLimit));
    }
  }
}

void
Context::Restart()
{
  ShouldRestart = true;
  Close();
}

void
Context::Close()
{
  cout << "Closing: " << implementation->Title() << endl;
  IsRendering = false;
}

Context::~Context()
{
  device.SetDeviceLostCallback(nullptr, nullptr);
  cout << "got closed" << endl;
}

} // namespace DGame
#endif
