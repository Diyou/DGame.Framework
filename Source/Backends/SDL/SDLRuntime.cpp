/**
 * SDLRuntime.cpp - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "SDLRuntime.h"

namespace DGame {

void
SDLRuntime::Start()
{
#if DGAME_THREADS
  Loop = async(launch::deferred, [this]() {
    IsRunning = true;
    cout << "Start Polling" << endl;
    while(IsRunning)
    {
      ProcessEvents();
      this_thread::sleep_for(PollingDelay);
    }
    Exit.set_value(EXIT_SUCCESS);
  });
#endif
}

RunTimeExit::operator int()
{
  auto &Runtime = SDLRuntime::Instance();
#if !defined(__EMSCRIPTEN__)
  Runtime.Loop.wait();
  cout << "Exiting. " << endl;
  return Runtime.Exit.get_future().get();
#else
  Runtime.IsRunning = true;

  emscripten_set_main_loop_arg(
    [](void *userData) {
      auto &Runtime = *reinterpret_cast<SDLRuntime *>(userData);
      Runtime.ProcessEvents();
    },
    &Runtime,
    0,
    false
  );
  return EXIT_SUCCESS;
#endif
};

} // namespace DGame

int main(int argc, char *argv[]);

#if defined(_WIN32)
int WINAPI
WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nShowCmd
)
{
  return main(__argc, __argv);
}
#endif
