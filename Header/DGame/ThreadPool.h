/**
 * ThreadPool.h- DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <functional>

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

  static ThreadPool Instance;
};

inline ThreadPool &Tasks = ThreadPool::Instance;
} // namespace DGame
