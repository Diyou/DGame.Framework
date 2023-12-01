/**
 * ThreadPool.cpp - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#if !defined(__EMSCRIPTEN__) || defined(__EMSCRIPTEN_PTHREADS__)
#include "DGame/ThreadPool.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost;

namespace DGame {
struct ThreadPool::IThreadPool : public asio::thread_pool
{
  IThreadPool(int threads)
  : asio::thread_pool(threads)
  {}
};

ThreadPool::ThreadPool(int threads)
: size(threads)
, implementation(new IThreadPool(threads))
{}

void
ThreadPool::Stop()
{
  implementation->stop();
}

template<typename T>
void
ThreadPool::Post(std::function<T> Task)
{
  asio::post(*implementation.get(), Task);
}

ThreadPool ThreadPool::Instance(thread::hardware_concurrency());
} // namespace DGame
#endif
