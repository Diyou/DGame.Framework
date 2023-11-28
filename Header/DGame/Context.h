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

namespace DGame {

struct RunTimeExit
{
  operator int();
};

/**
 * @brief Used to hold the Runtime before returning from the main function
 * @snippet Examples/HelloTriangle.cpp  Example Main
 */
static RunTimeExit Return{};

struct Context
{
  /**
   *  @brief Creates a DAWN rendering context.
   *  @param windowTitle:		The window title.
   *  @param windowWidth: 	The window width.
   *  @param windowHeight:	The window height.
   */
  Context(const char *windowTitle, int windowWidth, int windowHeight);

  /** @fn void Backend::Start()
   *  @brief Start Rendering
   */
  void Start();

  virtual ~Context();

protected:
  wgpu::Device device;
  wgpu::Surface surface;
  wgpu::SwapChain swapchain;
  wgpu::Queue queue;
  virtual void draw() = 0;

private:
  struct Backend;
  std::unique_ptr<Backend> implementation;

  bool &IsRendering;
};
} // namespace DGame