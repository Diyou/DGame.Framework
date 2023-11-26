/**
 * Backend.h - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "webgpu/webgpu_cpp.h"

#ifdef __EMSCRIPTEN__
// Hookup async initiation
#define main __main__
#endif

#include <memory>

namespace DGame
{

class Backend
{
  struct IBackend;
  std::unique_ptr<IBackend> implementation;

  bool &IsRendering;

protected:
  wgpu::Device device;
  wgpu::Surface surface;
  wgpu::SwapChain swapchain;
  wgpu::Queue queue;
  virtual void draw() = 0;

public:
  /** @fn void Backend::Backend(const char *windowTitle, int windowWidth, int windowHeight)
   *  @brief Creates a DAWN rendering context.
   *  @param windowTitle:		The window title.
   *  @param windowWidth: 	The window width.
   *  @param windowHeight:	The window height.
   */
  Backend(const char *windowTitle, int windowWidth, int windowHeight);

  /** @fn void Backend::Start()
   *  @brief Start Rendering
   */
  void Start();

  virtual ~Backend();
};
} // namespace DGame
