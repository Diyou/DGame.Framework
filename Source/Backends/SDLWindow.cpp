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

#include "SDLWindow.h"

#include <SDL_syswm.h>

#include <future>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;
using namespace chrono;
using namespace wgpu;

int __main__(int argc, const char *argv[]);

extern "C" __attribute__((used, visibility("default"))) int
_main(int argc, const char *argv[])
{
	return __main__(argc, argv);
}

namespace DGame
{
struct SDLRuntime
{

	vector<BackendType> SupportedBackends;
	vector<string> AvailableDrivers;
	string Driver, PreferredDriver;
	/**
	 * @brief Delay in microseconds
	 */
	uint64_t PollingDelay = 1000;
	bool isRunning = true;

	SDLRuntime(SDLRuntime &) = delete;
	void operator=(const SDLRuntime &) = delete;

	~SDLRuntime()
	{
		SDL_VideoQuit();
		SDL_Quit();
	}

	future<void> loop;

	explicit SDLRuntime()
	{
		SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

#if defined(__EMSCRIPTEN__)
		SupportedBackends = { BackendType::WebGPU };
#elif defined(_WIN32)
		SupportedBackends = { BackendType::D3D12, BackendType::D3D11, BackendType::Vulkan };
#elif defined(__APPLE__)
		SupportedBackends = { BackendType::Metal };
#elif defined(__linux__)
		SupportedBackends = { BackendType::Vulkan };
		PreferredDriver = "wayland";
#endif

		// Check supported Drivers
		SDL_Init(0);

		for (int i = 0; i < SDL_GetNumVideoDrivers(); i++)
		{
			const char *driver = SDL_GetVideoDriver(i);
			if (0 == SDL_VideoInit(driver))
			{
				AvailableDrivers.push_back(driver);
			}
		}

		auto SDL_VIDEODRIVER = getenv("SDL_VIDEODRIVER");
		string requestedDriver = SDL_VIDEODRIVER ? SDL_VIDEODRIVER : "";
		if (find(AvailableDrivers.begin(), AvailableDrivers.end(), requestedDriver) != AvailableDrivers.end()
		    && SDL_VideoInit(requestedDriver.c_str()) == 0)
		{
			cout << "User asked for " << requestedDriver << '\n';
			Driver = requestedDriver;
		}
		else
		{
			unsetenv("SDL_VIDEODRIVER");
			if (PreferredDriver.empty() || SDL_VideoInit(PreferredDriver.c_str()) != 0)
			{
				SDL_VideoInit(NULL);
			}

			Driver = SDL_GetCurrentVideoDriver();
		}

		stringstream driverOutput;
		const char *separator = "";
		for_each(
		    AvailableDrivers.begin(),
		    AvailableDrivers.end(),
		    [&driverOutput, &separator](const string driver) -> void {
			    driverOutput << separator << driver;
			    separator = ",";
		    }
		);
		cout << "Usable Driver(" << driverOutput.str() << ")\nInitiated with: " << Driver << endl;

		SDL_Init(SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
#if defined(__EMSCRIPTEN__)
		// emscripten_async_call([](void *arg) { ((SDLRuntime *)arg)->ProcessEvents(); }, this, 0);

		EM_ASM({
			if (navigator["gpu"])
			{
				navigator["gpu"]["requestAdapter"]().then(
				    function(adapter) {
					    adapter["requestDevice"]().then(function(device) {
						    Module["preinitializedWebGPUDevice"] = device;
						    __main(0, "");
					    });
				    },
				    function() { console.error("No WebGPU adapter; not starting"); }
				);
			}
			else
			{
				console.error("No support for WebGPU; not starting");
			}
		});

		emscripten_set_main_loop_arg([](void *arg) { ((SDLRuntime *)arg)->ProcessEvents(); }, this, 1000, false);
#else
		loop = async(launch::async, [this]() {
			while (isRunning)
			{
				ProcessEvents();
				this_thread::sleep_for(microseconds(PollingDelay));
			}
		});
#endif
	}

	void
	ProcessEvents()
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			// see https://wiki.libsdl.org/SDL2/SDL_Event
			switch (Event.type)
			{
			case SDL_QUIT:
				isRunning = false;
				loop.wait();
				break;
			default:
				if (SDL_GetWindowFromID(Event.window.windowID) == NULL)
				{
					break;
				}
			case SDL_WINDOWEVENT:
				break;
				Window::FromSDLWindow(SDL_GetWindowFromID(Event.window.windowID))->onWindowEvent(Event.window);
				break;
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				Window::FromSDLWindow(SDL_GetWindowFromID(Event.window.windowID))->onMouseButtonEvent(Event.button);
				break;
			case SDL_MOUSEMOTION:
				Window::FromSDLWindow(SDL_GetWindowFromID(Event.window.windowID))->onMouseMotionEvent(Event.motion);
				break;
			}
		}
	}

	static SDLRuntime *Instance;
};

SDLRuntime *SDLRuntime::Instance = new SDLRuntime();

Window::Window(const char *title, int width, int height)
{
	BackendType = SDLRuntime::Instance->SupportedBackends[0];

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	if (window == NULL)
	{
		cerr << SDL_GetError() << endl;
		throw runtime_error("Could not create SDL Window");
	}
	SDL_SetWindowData(window, "DGame::Window", this);

	/*
	emscripten_async_call([](void *arg) { ((SDLRuntime *)arg)->ProcessEvents(); }, SDLRuntime::Instance.get(), 0);
	*/
}

const char *
Window::Title() const
{
	return SDL_GetWindowTitle(window);
}

Window::Dimension
Window::Size() const
{
	Dimension size{};
	SDL_GetWindowSize(window, &size.width, &size.height);
	return size;
}

Window *
Window::FromSDLWindow(SDL_Window *window)
{
	return (Window *)SDL_GetWindowData(window, "DGame::Window");
}

unique_ptr<ChainedStruct>
Window::createSurfaceDescriptor()
{
	// see https://wiki.libsdl.org/SDL2/SDL_SysWMinfo
	SDL_SysWMinfo sysWMInfo;
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(window, &sysWMInfo);

	auto driver = SDLRuntime::Instance->Driver;

// Create surface
#if defined(__EMSCRIPTEN__)
	auto descriptor = make_unique<SurfaceDescriptorFromCanvasHTMLSelector>();
	descriptor->sType = SType::SurfaceDescriptorFromCanvasHTMLSelector;
	stringstream id;
	id << "canvas#SDLWindow" << to_string(SDL_GetWindowID(window));
	descriptor->selector = id.str().c_str();
	return std::move(descriptor);
#elif defined(_WIN32)
	auto descriptor = make_unique<SurfaceDescriptorFromWindowsHWND>();
	descriptor->hwnd = sysWMInfo.info.win.window;
	descriptor->hinstance = sysWMInfo.info.win.hinstance;
	return std::move(descriptor);
#elif defined(__APPLE__)
	auto descriptor = make_unique<SurfaceDescriptorFromMetalLayer>();
	descriptor->layer = sysWMInfo.info.cocoa.window;
	return std::move(descriptor);
#elif defined(__linux__)
	if (driver.compare("wayland") == 0)
	{
		auto descriptor = make_unique<SurfaceDescriptorFromWaylandSurface>();
		descriptor->surface = sysWMInfo.info.wl.surface;
		descriptor->display = sysWMInfo.info.wl.display;
		return std::move(descriptor);
	}
	else
	{
		auto descriptor = make_unique<SurfaceDescriptorFromXlibWindow>();
		descriptor->window = sysWMInfo.info.x11.window;
		descriptor->display = sysWMInfo.info.x11.display;
		return std::move(descriptor);
	}
#elif defined(__ANDROID__)
	auto descriptor = make_unique<SurfaceDescriptorFromAndroidNativeWindow>();
	descriptor->window = sysWMInfo.info.android.window;
	return std::move(descriptor);
#endif
}

Window::~Window()
{
	SDL_DestroyWindow(window);
}
} // namespace DGame

#undef main

int
main(int argc, const char *argv[])
{
	__main__(argc, argv);
	DGame::SDLRuntime::Instance->loop.wait();
	return 0;
}
