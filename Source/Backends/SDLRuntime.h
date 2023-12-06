/**
 * SDLRuntime.h - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "SDLWindow.h"

#include <future>
#include <sstream>
#include <thread>
#include <vector>

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

  microseconds PollingDelay{1000};
  bool isRunning = true;

  SDLRuntime(SDLRuntime &) = delete;
  void operator=(const SDLRuntime &) = delete;

  ~SDLRuntime()
  {
    SDL_VideoQuit();
    SDL_Quit();
  }

  promise<int> Exit;
  future<void> Loop;

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
#ifdef _WIN32
      SetEnvironmentVariable("SDL_VIDEODRIVER", NULL);
#else
      unsetenv("SDL_VIDEODRIVER");
#endif
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
          adapter => {
            adapter["requestDevice"]().then(function(device) {
              Module["preinitializedWebGPUDevice"] = device;
              _main(0, "");
            });
          },
          () => {
            console.error("No WebGPU adapter; not starting");
          }
        );
      }
      else
      {
        console.error("This Browser does not support WebGPU.\nSee https://github.com/gpuweb/gpuweb/wiki/Implementation-Status");
      }
    });
#else
    Loop = async(launch::deferred, [this]() {
      while(isRunning)
      {
        ProcessEvents();
        this_thread::sleep_for(PollingDelay);
      }
      Exit.set_value(EXIT_SUCCESS);
    });
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
  SDLRuntime::Instance->Loop.wait();
  return SDLRuntime::Instance->Exit.get_future().get();
#endif
};
} // namespace DGame
