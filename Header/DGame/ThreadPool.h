/**
 * ThreadPool.h- DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <functional>

namespace DGame {
class ThreadPool
{
  struct IThreadPool;
  std::unique_ptr<IThreadPool> implementation;

public:
  const int size;
  ThreadPool(int threads);

  void Stop();

  template<typename T>
  void Post(std::function<T> Task);

  inline void Join();

  static ThreadPool Instance;
};

inline ThreadPool &Tasks = ThreadPool::Instance;
} // namespace DGame
