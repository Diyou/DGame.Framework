/**
 * SDLWindow.h - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
	static Window *FromSDLWindowID(Uint32 id);

	void
	onWindowEvent(SDL_WindowEvent &event)
	{
		switch(event.event)
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
		switch(event.type)
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
