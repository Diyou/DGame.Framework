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

#include "DGame/ThreadPool.h"
#include "webgpu/webgpu_cpp.h"

#include <optional>
#include <thread>
#include <type_traits>

namespace DGame {

struct RunTime
{
  void Start();
  operator int();
};

/**
 * @brief Used to hold the Runtime before returning from the main function
 * @snippet Examples/HelloTriangle.cpp  Example Main
 */
static RunTime FromRuntime{};

struct Context
{
  template<class T, class... Args>
  friend inline void Launch(Args &&...args);
  /**
   * @brief Default FPSLimit is 60
   */
  int FPSLimit = 60;

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
  void Restart();
  void Close();

  virtual ~Context();

protected:
  wgpu::Device device;
  wgpu::Surface surface;
  wgpu::SwapChainDescriptor SWDescriptor;
  wgpu::SwapChain swapchain;
  wgpu::Queue queue;
  virtual void draw() = 0;

private:
  struct Backend;
  std::unique_ptr<Backend> implementation;

  bool IsRendering = true;
  bool ShouldRestart = false;
};

template<class T, class... Args>
inline void
Launch(Args &&...args)
{
  static_assert(std::is_base_of<Context, T>::value);
  FromRuntime.Start();
#if DGAME_THREADS
  auto Task = [... args = std::forward<Args>(args)] {
    std::cout << "Created in: " << std::this_thread::get_id() << std::endl;
    bool restart = false;
    do
    {
      T ctx(args...);
      ctx.Start();
      restart = ctx.ShouldRestart;
    } while(restart);
  };
  Tasks.Post(Task);
#else
  /**
   * @todo Restarting not supported yet
   */
  auto &ctx = *new T(args...);
  ctx.Start();
#endif
}
} // namespace DGame
