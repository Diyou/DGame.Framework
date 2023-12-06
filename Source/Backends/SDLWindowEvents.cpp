/**
 * SDLWindowEvents.cpp - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "SDLWindow.h"

namespace DGame {

void
Window::onMouseButtonEvent(SDL_MouseButtonEvent &event) const
{
  switch(event.type)
  {
  case SDL_MOUSEBUTTONDOWN:
    std::cout << "ID:" << event.windowID << " button pressed" << std::endl;
    break;
  case SDL_MOUSEBUTTONUP:
    std::cout << "button released" << std::endl;
    break;
  }
}

void
Window::onMouseMotionEvent(SDL_MouseMotionEvent &event) const
{
  std::cout << "(" << event.x << "," << event.y << ")" << std::endl;
}

void
Window::onWindowEvent(SDL_WindowEvent &event)
{
  {
    switch(event.event)
    {
    case SDL_WINDOWEVENT_SHOWN:
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      std::cout << "mini me" << std::endl;
      break;
    case SDL_WINDOWEVENT_CLOSE:
      std::cout << "I died" << std::endl;
      Close();
      break;
    case SDL_WINDOWEVENT_RESIZED:
#if defined(__EMSCRIPTEN__)
      EM_ASM(
      {
        let canvas = document.querySelector(UTF8ToString($0));
        canvas.width = $1;
        canvas.height = $2;
      },
      Selector.c_str(),
      event.data1,
      event.data2
    );
#endif
      break;
    }
  }
}
} // namespace DGame
