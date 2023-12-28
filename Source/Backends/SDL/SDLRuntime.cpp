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
  if(IsRunning)
  {
    return;
  }
  IsRunning = true;
#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop_arg(
    [](void *userData) {
      auto &Runtime = *(SDLRuntime *)userData;
      Runtime.ProcessEvents();
    },
    this,
    0,
    false
  );
#else
  auto Task = [this]() {
    while(IsRunning)
    {
      ProcessEvents();
      this_thread::sleep_for(PollingDelay);
    }
    Exit.set_value(EXIT_SUCCESS);
  };
  Loop = async(launch::deferred, Task);
  // Tasks.Defer(Task);
#endif
}

void
RunTime::Start()
{
  SDLRuntime::Instance().Start();
}

RunTime::operator int()
{
  auto &Runtime = SDLRuntime::Instance();
#if defined(__EMSCRIPTEN__)
  return EXIT_SUCCESS;
#else
  Runtime.Loop.wait();
  Tasks.join();
  cout << "Exiting. " << endl;
  return Runtime.Exit.get_future().get();
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
