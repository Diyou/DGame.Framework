/**
 * Context.h - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "webgpu/webgpu_cpp.h"

#include <optional>

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
   *  @brief Creates a webgpu rendering context.
   *  @param windowTitle:   Title of window.
   *  @param windowWidth:   Width of the windowh.
   *  @param windowHeight:  Height of the window.
   *  @param posX:  Horitonal position of the window.
   *  @param posY:  Vertical positon of the window.
   */
  Context(
    const char *windowTitle,
    int windowWidth,
    int windowHeight,
    std::optional<int> posX = std::nullopt,
    std::optional<int> posY = std::nullopt
  );

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
