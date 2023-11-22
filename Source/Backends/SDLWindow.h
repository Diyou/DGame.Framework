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
	SDL_Window *window;
	std::string title;
	int width, height;

	bool isAlive = true;

	wgpu::BackendType BackendType;

	Window(const char *title, int width, int height);

	/**
	 * @brief Retrieves the Window pointer from the associated SDL_Window pointer
	 * @param window The SDL_Window pointer
	 * @return DGame::Window pointer
	 */
	static Window *FromSDLWindow(SDL_Window *window);

	void onWindowEvent(SDL_WindowEvent &event)
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

	void onMouseButtonEvent(SDL_MouseButtonEvent &event)
	{
		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			std::cout << "button pressed" << std::endl;
			break;
		case SDL_MOUSEBUTTONUP:
			std::cout << "button released" << std::endl;
			break;
		}
	}

	void onMouseMotionEvent(SDL_MouseMotionEvent &event)
	{
		std::cout << "(" << event.x << "," << event.y << ")" << std::endl;
	}

	std::unique_ptr<wgpu::ChainedStruct> createSurfaceDescriptor();

	virtual ~Window();
};
} // namespace DGame