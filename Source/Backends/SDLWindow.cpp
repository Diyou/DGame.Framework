#include "SDLWindow.h"

#include <SDL2/SDL.h>
#include <SDL_syswm.h>

using namespace std;
using namespace wgpu;

namespace DGame
{
Window::Window(const char *title, int width, int height)
    : title(title)
    , width(width)
    , height(height)
{
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

	string requestDriver("wayland");
	string requestedDriver;
	if (const char *askingFor = getenv("SDL_VIDEODRIVER"))
	{
		cout << "User asked for " << askingFor << '\n';
		requestedDriver = askingFor;
	}
	bool foundPreferred = false;



#if defined(_WIN32)
	BackendType = BackendType::D3D12;
#elif defined(__APPLE__)
	BackendType = BackendType::Metal;
#elif defined(__linux__)
	BackendType = BackendType::Vulkan;

	SDL_Init(0);
	stringstream usableDriver;
	const char *separator = "";
	for (int i = 0; i < SDL_GetNumVideoDrivers(); i++)
	{
		const char *driver = SDL_GetVideoDriver(i);
		if (0 == SDL_VideoInit(driver))
		{
			usableDriver << separator << driver;
			separator = ",";
			if (requestedDriver.compare(driver) == 0)
			{
				foundPreferred = true;
			}
			if (requestDriver.compare(driver) == 0)
			{
				supportWayland = true;
			}
		}
		SDL_VideoQuit();
	}
	cout << "Usable Driver(" << usableDriver.str() << ")" << endl;
#endif

	if (foundPreferred)
	{
		SDL_VideoInit(requestedDriver.c_str());
		if (requestedDriver.compare(requestDriver) != 0)
		{
			supportWayland = false;
		}
	}
	else if (supportWayland)
	{
		SDL_VideoInit(requestDriver.c_str());
	}
	else
	{
		SDL_VideoInit(NULL);
	}

	SDL_Init(SDL_INIT_GAMECONTROLLER);

	driver = SDL_GetCurrentVideoDriver();

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);

	if (window == NULL)
	{
		cerr << SDL_GetError() << endl;
		throw runtime_error("Could not create SDL Window");
	}
	else
	{
		stringstream ss;
		ss << "Successfully created window(" << driver << ")";
		cout << ss.str() << endl;
	}
}

unique_ptr<ChainedStruct> Window::createSurfaceDescriptor()
{
	// see https://wiki.libsdl.org/SDL2/SDL_SysWMinfo
	SDL_SysWMinfo sysWMInfo;
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(window, &sysWMInfo);

	// Create surface
#ifdef _WIN32
	auto descriptor = make_unique<SurfaceDescriptorFromWindowsHWND>();
	descriptor->hwnd = sysWMInfo.info.win.window;
	descriptor->hinstance = sysWMInfo.info.win.hinstance;
	return std::move(descriptor);
#elif defined(__APPLE__)
	auto descriptor = make_unique<SurfaceDescriptorFromMetalLayer>();
	descriptor->layer = sysWMInfo.info.cocoa.window;
	return std::move(descriptor);
#elif defined(__linux__)
	if (supportWayland)
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
	SDL_VideoQuit();
	SDL_Quit();
}
} // namespace DGame