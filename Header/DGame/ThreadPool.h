/**
 * ThreadPool.h- DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#pragma once
#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
// Compatibility Header
#include "DGame/ThreadPoolCompat.h"
#else
#define DGAME_THREADS 1
#include <boost/asio/defer.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

namespace DGame {
struct ThreadPool : public boost::asio::thread_pool
{
  const int size;
  ThreadPool(int threads);

  template<typename T>
  void
  Post(T Task)
  {
    boost::asio::post(*this, Task);
  }

  template<typename T>
  void
  Dispatch(T Task)
  {
    boost::asio::dispatch(*this, Task);
  }

  template<typename T>
  void
  Defer(T Task)
  {
    boost::asio::defer(*this, Task);
  }

  static ThreadPool Instance;
};

inline ThreadPool &Tasks = ThreadPool::Instance;
} // namespace DGame
#endif
