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
#include "DGame/ThreadPool.h"
#include "SDLWindow.h"

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
    auto size = Size();
    EM_ASM(
      {
        let canvas = document.querySelector('canvas#canvas');
        let original = canvas.cloneNode(true);
        original.style.position = 'absolute';
        original.style.zIndex = -1;
        canvas.removeAttribute('style');
        canvas.id = UTF8ToString($0);
        canvas.title = UTF8ToString($1);
        canvas.width = $2;
        canvas.height = $3;
        document.body.insertBefore(original, canvas.nextSibling);
      },
      ID.c_str(),
      Title(),
      size.width,
      size.height
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
, IsRendering(implementation->isAlive)
, SWDescriptor{}
{
  device = implementation->createDevice();
  surface = implementation->createSurface();

  SWDescriptor.usage = TextureUsage::RenderAttachment;
  SWDescriptor.format = TextureFormat::BGRA8Unorm;
  SWDescriptor.presentMode = PresentMode::Fifo;

  swapchain = implementation->createSwapChain(device, surface, SWDescriptor);
  queue = device.GetQueue();
}

void
Context::Start()
{
  auto task = [this]() {
    emscripten_request_animation_frame_loop(
      [](double time, void *userData) -> EM_BOOL {
        auto self = (Context *)userData;
        self->draw();
        return self->IsRendering;
      },
      this
    );
  };

  Tasks.Post(task);

  SDL_ShowWindow(implementation->window);
}

Context::~Context()
{}

} // namespace DGame
#endif
