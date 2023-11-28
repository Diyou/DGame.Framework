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
#include "SDLWindow.h"
#if defined(__EMSCRIPTEN_PTHREADS__)
#include <thread>
#endif

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>

using namespace std;
using namespace wgpu;

namespace DGame {
struct Context::Backend : public Window
{
  Instance instance;
  bool isAlive = true;

  Backend(const char *windowTitle, int windowWidth, int windowHeight)
  : instance(CreateInstance())
  , Window(windowTitle, windowWidth, windowHeight)
  {
    EM_ASM(
      {
        let canvas = document.querySelector("canvas#canvas");
        let original = canvas.cloneNode(true);
        original.style.position = "absolute";
        original.style.zIndex = -1;
        canvas.removeAttribute("style");
        canvas.id = UTF8ToString($0);
        canvas.title = UTF8ToString($1);
        document.body.insertBefore(original, canvas);
      },
      ID.c_str(),
      Title()
    );
  }

  Device
  createDevice()
  {
    return Device::Acquire(emscripten_webgpu_get_device());
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
  createSwapChain(Device device, Surface surface)
  {
    auto size = Size();
    SwapChainDescriptor descriptor;
    descriptor.usage = TextureUsage::RenderAttachment;
    descriptor.format = TextureFormat::BGRA8Unorm;
    descriptor.width = size.width;
    descriptor.height = size.height;
    descriptor.presentMode = PresentMode::Fifo;

    return device.CreateSwapChain(surface, &descriptor);
  }
}; // namespace DGame

Context::Context(const char *windowTitle, int windowWidth, int windowHeight)
: implementation(new Context::Backend(windowTitle, windowWidth, windowHeight))
, IsRendering(implementation->isAlive)
{
  device = implementation->createDevice();
  surface = implementation->createSurface();
  swapchain = implementation->createSwapChain(device, surface);
  queue = device.GetQueue();
}

void
Context::Start()
{
#if defined(__EMSCRIPTEN_PTHREADS__)
  thread([this]() {
    while(IsRendering)
    {
      draw();
    }
  }).detach();
#else
  auto cb = [](double time, void *userData) -> EM_BOOL {
    auto self = (Context *)userData;
    self->draw();
    self->Start();
    return self->IsRendering;
  };
  emscripten_request_animation_frame(cb, this);
#endif
  SDL_ShowWindow(implementation->window);
}

Context::~Context()
{}

} // namespace DGame
#endif
