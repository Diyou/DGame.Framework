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

#include <SDL.h>

#include <iostream>
#include <memory>
#include <string>

namespace DGame
{
/**
 * @note On Linux set environment variable SDL_VIDEODRIVER=x11
 *       to overwrite the preferred driver wayland
 */
struct Window
{
	struct Dimension
	{
		int width, height;
	};

	SDL_Window *window;

	bool isAlive = true;

	wgpu::BackendType BackendType;

	Window(const char *title, int width, int height);

	const char *Title() const;
	Dimension Size() const;
	/**
	 * @brief Retrieves the Window pointer from the associated SDL_Window pointer
	 * @param window The SDL_Window pointer
	 * @return DGame::Window pointer
	 */
	static Window *FromSDLWindow(SDL_Window *window);

	void
	onWindowEvent(SDL_WindowEvent &event)
	{
		switch (event.event)
		{
		case SDL_WINDOWEVENT_MINIMIZED:
			std::cout << "mini me" << std::endl;
			break;
		case SDL_WINDOWEVENT_CLOSE:
			std::cout << "I died" << std::endl;
			isAlive = false;
			break;
		}
	}

	void
	onMouseButtonEvent(SDL_MouseButtonEvent &event)
	{
		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			std::cout << "ID:" << event.windowID << " button pressed" << std::endl;
			break;
		case SDL_MOUSEBUTTONUP:
			std::cout << "button released" << std::endl;
			break;
		}
	}

	void
	onMouseMotionEvent(SDL_MouseMotionEvent &event)
	{
		std::cout << "(" << event.x << "," << event.y << ")" << std::endl;
	}

	std::unique_ptr<wgpu::ChainedStruct> createSurfaceDescriptor();

	virtual ~Window();
};
} // namespace DGame
