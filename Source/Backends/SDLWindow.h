#pragma once

#include "webgpu/webgpu_cpp.h"

#include <SDL.h>

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

	wgpu::BackendType BackendType;

	Window(const char *title, int width, int height);

	/**
	 * @brief Retrieves the Window pointer from the associated SDL_Window pointer
	 * @param window The SDL_Window pointer
	 * @return DGame::Window pointer
	 */
	static Window *FromSDLWindow(SDL_Window *window);

	std::unique_ptr<wgpu::ChainedStruct> createSurfaceDescriptor();

	virtual ~Window();
};
} // namespace DGame