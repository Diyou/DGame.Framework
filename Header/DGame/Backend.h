// MIT License
//
// Copyright (c) 2023 Diyou
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "webgpu/webgpu_cpp.h"

#ifdef __EMSCRIPTEN__
// Hookup async initiation
#define main __main__
#endif

#include <memory>

namespace DGame
{

class Backend
{
	struct IBackend;
	std::unique_ptr<IBackend> implementation;

	bool &IsRendering;

protected:
	wgpu::Device device;
	wgpu::Surface surface;
	wgpu::SwapChain swapchain;
	wgpu::Queue queue;
	virtual void draw() = 0;

public:
	/** @fn void Backend::Backend(const char *windowTitle, int windowWidth, int windowHeight)
	 *  @brief Creates a DAWN rendering context.
	 *  @param windowTitle:		The window title.
	 *  @param windowWidth: 	The window width.
	 *  @param windowHeight:	The window height.
	 */
	Backend(const char *windowTitle, int windowWidth, int windowHeight);

	/** @fn void Backend::Start()
	 *  @brief Start Rendering
	 */
	void Start();

	virtual ~Backend();
};
} // namespace DGame