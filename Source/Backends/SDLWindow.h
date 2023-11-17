#pragma once

#include "webgpu/webgpu_cpp.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <memory>
#include <sstream>

namespace DGame
{
/**
 * @note On Linux set environment variable SDL_VIDEODRIVER=x11
 *       to overwrite the preferred driver wayland
 */
struct Window
{
	SDL_Window *window;
	std::string title, driver;
	int width, height;

	bool supportWayland = false;

	wgpu::BackendType BackendType;

	Window(const char *title, int width, int height);
	std::unique_ptr<wgpu::ChainedStruct> createSurfaceDescriptor();

	virtual ~Window();
};
} // namespace DGame