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

using namespace boost;

namespace DGame {

ThreadPool::ThreadPool(int threads)
: size(threads)
, asio::thread_pool(threads)
{}

ThreadPool ThreadPool::Instance(thread::hardware_concurrency());
} // namespace DGame
