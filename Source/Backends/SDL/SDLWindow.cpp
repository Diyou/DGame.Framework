/**
 * SDLWindow.cpp - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "SDLRuntime.h"

#include <SDL_syswm.h>

using namespace std;
using namespace chrono;
using namespace wgpu;

namespace DGame {

constexpr char WindowRequestString[] = "&";

Window::Window(const char *title, int width, int height, int posX, int posY)
: BackendType(SDLRuntime::Instance().SupportedBackends[0])
, clickSound("./Resources/Sound/mixkit-arcade-game-jump-coin-216.wav")
{
  window = SDL_CreateWindow(
    title,
    posX,
    posY,
    width,
    height,
    SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE
  );
  if(window == NULL)
  {
    cerr << SDL_GetError() << endl;
    throw runtime_error("Could not create SDL Window");
  }
  SDL_SetWindowData(window, WindowRequestString, this);

#ifdef __EMSCRIPTEN__
  ID = "SDLWindow" + to_string(SDL_GetWindowID(window));
  Selector = "canvas#" + ID;
#endif
}

const char *
Window::Title() const
{
  return SDL_GetWindowTitle(window);
}

Window::Dimension
Window::Size() const
{
  Dimension size{};
  SDL_GetWindowSize(window, &size.width, &size.height);
  return size;
}

Window *
Window::FromSDLWindow(SDL_Window *window)
{
  return (Window *)SDL_GetWindowData(window, WindowRequestString);
}

Window *
Window::FromSDLWindowID(Uint32 &id)
{
  return FromSDLWindow(SDL_GetWindowFromID(id));
}

#if !defined(__APPLE__)
unique_ptr<ChainedStruct>
Window::createSurfaceDescriptor()
{
  // see https://wiki.libsdl.org/SDL2/SDL_SysWMinfo
  SDL_SysWMinfo sysWMInfo;
  SDL_VERSION(&sysWMInfo.version);
  SDL_GetWindowWMInfo(window, &sysWMInfo);

  auto driver = SDLRuntime::Instance().Driver;

// Create surface
#if defined(__EMSCRIPTEN__)
  auto descriptor = make_unique<SurfaceDescriptorFromCanvasHTMLSelector>();
  descriptor->sType = SType::SurfaceDescriptorFromCanvasHTMLSelector;
  descriptor->selector = Selector.c_str();
  return descriptor;
#elif defined(_WIN32)
  auto descriptor = make_unique<SurfaceDescriptorFromWindowsHWND>();
  descriptor->hwnd = sysWMInfo.info.win.window;
  descriptor->hinstance = sysWMInfo.info.win.hinstance;
  return std::move(descriptor);
#elif defined(__linux__)
  if(driver.compare("wayland") == 0)
  {
    auto descriptor = make_unique<SurfaceDescriptorFromWaylandSurface>();
    descriptor->surface = sysWMInfo.info.wl.surface;
    descriptor->display = sysWMInfo.info.wl.display;
    return std::move(descriptor);
  }
  else
  {
    auto descriptor = make_unique<SurfaceDescriptorFromXlibWindow>();
    descriptor->window = sysWMInfo.info.x11.window;
    descriptor->display = sysWMInfo.info.x11.display;
    return std::move(descriptor);
  }
#elif defined(__ANDROID__)
  auto descriptor = make_unique<SurfaceDescriptorFromAndroidNativeWindow>();
  descriptor->window = sysWMInfo.info.android.window;
  return std::move(descriptor);
#else
#error "Unsupported WGPU target platform"
#endif
}
#endif

Window::~Window()
{
  SDL_DestroyWindow(window);
}
} // namespace DGame
