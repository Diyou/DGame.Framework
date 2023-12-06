/**
 * SDLWindow.h - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include "DGame/Context.h"

#include <SDL2/SDL.h>

namespace DGame {
/**
 * @note On Linux set environment variable SDL_VIDEODRIVER=x11
 *       to overwrite the preferred driver wayland
 */
struct Window
{
  struct Dimension
  {
    int width, height;
  };

  SDL_Window *window;
#ifdef __EMSCRIPTEN__
  std::string ID, Selector;
#endif

  bool isAlive = true;

  const wgpu::BackendType BackendType;

  Window(const char *title, int width, int height, int posX, int posY);

  const char *Title() const;
  Dimension Size() const;

  void onWindowEvent(SDL_WindowEvent &event);

  void onMouseButtonEvent(SDL_MouseButtonEvent &event);

  void onMouseMotionEvent(SDL_MouseMotionEvent &event);

  std::unique_ptr<wgpu::ChainedStruct> createSurfaceDescriptor();

  /**
   * @brief Retrieves the Window pointer from the associated SDL_Window pointer
   * @param window The SDL_Window pointer
   * @return DGame::Window pointer
   */
  static Window *FromSDLWindow(SDL_Window *window);
  static Window *FromSDLWindowID(Uint32 &id);

  virtual ~Window();
};
} // namespace DGame
