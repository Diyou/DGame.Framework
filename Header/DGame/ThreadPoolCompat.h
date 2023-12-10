/**
 * ThreadPoolWOT.h- DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * This Header serves as a replacement for ThreadPool.h
 * in case pthreads aren't available
 */
#pragma once
#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
#define DGAME_THREADS 0

namespace DGame {
struct ThreadPool
{
  const int size;

  ThreadPool(int threads)
  : size(threads)
  {
    std::cout << "Threads: " << threads << std::endl;
  }

  template<typename T>
  void
  Post(T Task)
  {
    emscripten_async_call(
      [](void *userData) {
        auto callback = *reinterpret_cast<T *>(userData);
        callback();
      },
      &Task,
      0
    );
    emscripten_sleep(0);
  }

  static ThreadPool Instance;
};

inline ThreadPool &Tasks = ThreadPool::Instance;
} // namespace DGame
#endif
