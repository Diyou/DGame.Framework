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

using namespace std;

namespace DGame {

void
Window::onMouseButtonEvent(SDL_MouseButtonEvent &event) const
{
  switch(event.type)
  {
  case SDL_MOUSEBUTTONDOWN:
    cout << "ID:" << event.windowID << " button pressed" << endl;
    break;
  case SDL_MOUSEBUTTONUP:
    cout << "button released" << endl;
    break;
  }
}

void
Window::onMouseMotionEvent(SDL_MouseMotionEvent &event) const
{
  // cout << "(" << event.x << "," << event.y << ")" << endl;
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
      cout << "mini me" << endl;
      break;
    case SDL_WINDOWEVENT_CLOSE:
      WindowClosed();
      break;
    case SDL_WINDOWEVENT_RESIZED:
#if defined(__EMSCRIPTEN__)
      MAIN_THREAD_EM_ASM(
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
