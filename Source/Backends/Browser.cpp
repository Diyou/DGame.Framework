/**
 * Browser.cpp - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifdef __EMSCRIPTEN__

#include "DGame/Context.h"
#include "SDL/SDLWindow.h"

#include <cassert>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>

using namespace std;
using namespace wgpu;

namespace DGame {
struct Context::Backend : public Window
{
  Instance instance;

  Backend(
    const char *windowTitle,
    int windowWidth,
    int windowHeight,
    int x,
    int y
  )
  : instance(CreateInstance())
  , Window(windowTitle, windowWidth, windowHeight, x, y)
  {
    MAIN_THREAD_EM_ASM(
      {
        let canvas = document.querySelector('canvas#canvas');
        canvas.style.aspectRatio = $2 / $3;
        let original = canvas.cloneNode(true);
        original.style.position = 'absolute';
        original.style.zIndex = -1;
        canvas.removeAttribute('style');
        canvas.style.aspectRatio = original.style.aspectRatio;
        canvas.id = UTF8ToString($0);
        canvas.title = UTF8ToString($1);
        canvas.width = $2;
        canvas.height = $3;
        document.body.insertBefore(original, canvas.nextSibling);
      },
      ID.c_str(),
      Title(),
      windowWidth,
      windowHeight
    );
  }

  /**
   * Utility function to get a WebGPU adapter, so that
   *     Adapter adapter = requestAdapter(options);
   * is roughly equivalent to
   *     const adapter = await navigator.gpu.requestAdapter(options);
   */
  Adapter
  requestAdapter(const RequestAdapterOptions *options)
  {
    // A simple structure holding the local information shared with the
    // onAdapterRequestEnded callback.
    struct UserData
    {
      Adapter adapter = nullptr;
      bool requestEnded = false;
    };

    UserData userData;

    auto onAdapterRequestEnded = [](
                                   WGPURequestAdapterStatus status,
                                   WGPUAdapter adapter,
                                   const char *message,
                                   void *pUserData
                                 ) {
      auto &userData = *(UserData *)pUserData;
      if(status == WGPURequestAdapterStatus_Success)
      {
        userData.adapter = Adapter::Acquire(adapter);
      }
      else
      {
        cout << "Could not get WebGPU adapter: " << message << endl;
      }
      userData.requestEnded = true;
    };

    // Call to the WebGPU request adapter procedure
    instance.RequestAdapter(options, onAdapterRequestEnded, &userData);

    // In theory we should wait until onAdapterReady has been called, which
    // could take some time (what the 'await' keyword does in the JavaScript
    // code). In practice, we know that when the RequestAdapter()
    // function returns its callback has been called.
    while(!userData.requestEnded)
    {
      emscripten_sleep(0);
    }
    assert(userData.requestEnded);

    return userData.adapter;
  }

  /**
   * Utility function to get a WebGPU device, so that
   *     Device device = requestDevice(adapter, options);
   * is roughly equivalent to
   *     const device = await adapter.requestDevice(descriptor);
   * It is very similar to requestAdapter
   */
  Device
  requestDevice(Adapter &adapter, const DeviceDescriptor *descriptor)
  {
    struct UserData
    {
      Device device = nullptr;
      bool requestEnded = false;
    };

    UserData userData;

    auto onDeviceRequestEnded = [](
                                  WGPURequestDeviceStatus status,
                                  WGPUDevice device,
                                  const char *message,
                                  void *pUserData
                                ) {
      auto &userData = *(UserData *)pUserData;
      if(status == WGPURequestDeviceStatus_Success)
      {
        userData.device = Device::Acquire(device);
      }
      else
      {
        cout << "Could not get WebGPU device: " << message << endl;
      }
      userData.requestEnded = true;
    };

    adapter.RequestDevice(descriptor, onDeviceRequestEnded, &userData);

    while(!userData.requestEnded)
    {
      emscripten_sleep(0);
    }
    assert(userData.requestEnded);

    return userData.device;
  }

  Device
  createDevice(Surface &surface)
  {
    RequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapterOpts.compatibleSurface = surface;
    Adapter adapter = requestAdapter(&adapterOpts);

    DeviceDescriptor deviceDesc = {};
    return requestDevice(adapter, &deviceDesc);
  }

  Surface
  createSurface()
  {
    auto canvas = createSurfaceDescriptor();

    SurfaceDescriptor descriptor{};
    descriptor.nextInChain = canvas.get();

    return instance.CreateSurface(&descriptor);
  }

  SwapChain
  createSwapChain(
    Device &device,
    Surface &surface,
    SwapChainDescriptor &descriptor
  )
  {
    return device.CreateSwapChain(surface, &descriptor);
  }
}; // namespace DGame

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
  surface = implementation->createSurface();
  device = implementation->createDevice(surface);

  SWDescriptor.usage = TextureUsage::RenderAttachment;
  SWDescriptor.format = TextureFormat::BGRA8Unorm;
  SWDescriptor.presentMode = PresentMode::Fifo;

  swapchain = implementation->createSwapChain(device, surface, SWDescriptor);
  queue = device.GetQueue();

  implementation->MouseDown.connect(MouseDown);
  implementation->MouseUp.connect(MouseUp);
  implementation->MouseMove.connect(MouseMove);
}

void
Context::Start()
{
#if defined(__EMSCRIPTEN_PTHREADS__)
  auto task = [this]() {
    emscripten_set_main_loop_arg(
      [](void *userData) {
        auto &self = *(Context *)userData;
        self.draw();
      },
      this,
      0,
      true
    );
  };
  task();
#else
  auto task = [this]() {
    emscripten_request_animation_frame_loop(
      [](double time, void *userData) -> EM_BOOL {
        auto &self = *(Context *)userData;
        self.draw();
        return self.IsRendering;
      },
      this
    );
  };
  task();
#endif

  SDL_ShowWindow(implementation->window);
}

Context::~Context()
{}

std::mutex Context::RuntimeLock;
} // namespace DGame
#endif
