/**
 * SDLWindow.cpp - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "SDLWindow.h"

#include <SDL_syswm.h>
#include <future>
#include <sstream>
#include <thread>

using namespace std;
using namespace chrono;
using namespace wgpu;

namespace DGame {
struct SDLRuntime
{
  vector<BackendType> SupportedBackends;
  vector<string> AvailableDrivers;
  string Driver, PreferredDriver;
  /**
   * @brief Delay in microseconds
   */
  uint64_t PollingDelay = 1000;
  bool isRunning = true;

  SDLRuntime(SDLRuntime &) = delete;
  void operator=(const SDLRuntime &) = delete;

  ~SDLRuntime()
  {
    SDL_VideoQuit();
    SDL_Quit();
  }

  promise<int> Exit;

  explicit SDLRuntime()
  {
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

#if defined(__EMSCRIPTEN__)
    SupportedBackends = {BackendType::WebGPU};
#elif defined(_WIN32)
    SupportedBackends
      = {BackendType::D3D12, BackendType::D3D11, BackendType::Vulkan};
#elif defined(__APPLE__)
    SupportedBackends = {BackendType::Metal};
#elif defined(__linux__)
    SupportedBackends = {BackendType::Vulkan};
    PreferredDriver = "wayland";
#endif

    // Check supported Drivers
    SDL_Init(0);

    for(int i = 0; i < SDL_GetNumVideoDrivers(); i++)
    {
      const char *driver = SDL_GetVideoDriver(i);
      if(0 == SDL_VideoInit(driver))
      {
        AvailableDrivers.push_back(driver);
      }
    }

    auto SDL_VIDEODRIVER = getenv("SDL_VIDEODRIVER");
    string requestedDriver = SDL_VIDEODRIVER ? SDL_VIDEODRIVER : "";

    auto foundDriver
      = find(AvailableDrivers.begin(), AvailableDrivers.end(), requestedDriver);
    if(foundDriver != AvailableDrivers.end() && SDL_VideoInit(requestedDriver.c_str()) == 0)
    {
      cout << "User asked for " << requestedDriver << '\n';
      Driver = requestedDriver;
    }
    else
    {
      unsetenv("SDL_VIDEODRIVER");
      if(PreferredDriver.empty() || SDL_VideoInit(PreferredDriver.c_str()) != 0)
      {
        SDL_VideoInit(NULL);
      }

      Driver = SDL_GetCurrentVideoDriver();
    }

    stringstream driverOutput;
    const char *separator = "";
    for_each(
      AvailableDrivers.begin(),
      AvailableDrivers.end(),
      [&driverOutput, &separator](const string driver) {
        driverOutput << separator << driver;
        separator = ",";
      }
    );
    cout << "Usable Driver(" << driverOutput.str()
         << ")\nInitiated with: " << Driver << endl;

    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
#if defined(__EMSCRIPTEN__)
    EM_ASM({
      if(navigator["gpu"])
      {
        navigator["gpu"]["requestAdapter"]().then(
          function(adapter) {
            adapter["requestDevice"]().then(function(device) {
              Module["preinitializedWebGPUDevice"] = device;
              _main(0, "");
            });
          },
          function() { console.error("No WebGPU adapter; not starting"); }
        );
      }
      else
      {
        console.error("No support for WebGPU; not starting");
      }
    });
#else
    thread([this]() {
      while(isRunning)
      {
        ProcessEvents();
        this_thread::sleep_for(microseconds(PollingDelay));
      }
      Exit.set_value(EXIT_SUCCESS);
    }).detach();
#endif
  }

  void
  ProcessEvents()
  {
    SDL_Event Event;
    while(SDL_PollEvent(&Event))
    {
      // see https://wiki.libsdl.org/SDL2/SDL_Event
      switch(Event.type)
      {
      default:
        if(SDL_GetWindowFromID(Event.window.windowID) == NULL)
        {
          break;
        }
      case SDL_WINDOWEVENT:
        Window::FromSDLWindowID(Event.window.windowID)
          ->onWindowEvent(Event.window);
        break;
      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN:
        Window::FromSDLWindowID(Event.window.windowID)
          ->onMouseButtonEvent(Event.button);
        break;
      case SDL_MOUSEMOTION:
        Window::FromSDLWindowID(Event.window.windowID)
          ->onMouseMotionEvent(Event.motion);
        break;
      case SDL_QUIT:
        isRunning = false;
        break;
      }
    }
  }

  static SDLRuntime *Instance;
};

SDLRuntime *SDLRuntime::Instance = new SDLRuntime();

RunTimeExit::operator int()
{
#if defined(__EMSCRIPTEN__)
  while(SDLRuntime::Instance->isRunning)
  {
    SDLRuntime::Instance->ProcessEvents();
    emscripten_sleep(0);
  }
  return EXIT_SUCCESS;
#else
  return SDLRuntime::Instance->Exit.get_future().get();
#endif
};

Window::Window(const char *title, int width, int height)
{
  BackendType = SDLRuntime::Instance->SupportedBackends[0];

  window = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    width,
    height,
    SDL_WINDOW_HIDDEN
  );
  if(window == NULL)
  {
    cerr << SDL_GetError() << endl;
    throw runtime_error("Could not create SDL Window");
  }
  SDL_SetWindowData(window, "&", this);
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
  return (Window *)SDL_GetWindowData(window, "&");
}

Window *
Window::FromSDLWindowID(Uint32 &id)
{
  return FromSDLWindow(SDL_GetWindowFromID(id));
}

unique_ptr<ChainedStruct>
Window::createSurfaceDescriptor()
{
  // see https://wiki.libsdl.org/SDL2/SDL_SysWMinfo
  SDL_SysWMinfo sysWMInfo;
  SDL_VERSION(&sysWMInfo.version);
  SDL_GetWindowWMInfo(window, &sysWMInfo);

  auto driver = SDLRuntime::Instance->Driver;

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
#elif defined(__APPLE__)
  auto descriptor = make_unique<SurfaceDescriptorFromMetalLayer>();
  descriptor->layer = sysWMInfo.info.cocoa.window;
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
#endif
}

Window::~Window()
{
  SDL_DestroyWindow(window);
}
} // namespace DGame
