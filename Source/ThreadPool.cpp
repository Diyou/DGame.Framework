/**
 * ThreadPool.cpp - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "DGame/ThreadPool.h"

#if !defined(__EMSCRIPTEN__) || defined(__EMSCRIPTEN_PTHREADS__)
#include <boost/thread/thread.hpp>

using namespace boost;
using namespace asio;

namespace DGame {

ThreadPool::ThreadPool(int threads)
: size(threads)
, thread_pool(threads)
{
  std::cout << "Threads: " << threads << std::endl;
}

#ifdef __EMSCRIPTEN__
inline const int EMSCRIPTEN_WORKERS = []() {
  return EM_ASM_INT(return PThread.unusedWorkers.length);
}();

ThreadPool ThreadPool::Instance(EMSCRIPTEN_WORKERS);
#else
ThreadPool ThreadPool::Instance(thread::hardware_concurrency());
#endif
} // namespace DGame
#else
DGame::ThreadPool DGame::ThreadPool::Instance(0);
#endif
