#include "SDLWindow.h"

#include <SDL_syswm.h>

#include <iostream>
#include <sstream>

using namespace std;
using namespace wgpu;

namespace DGame
{
struct SDLRuntime
{

	vector<BackendType> SupportedBackends;
	vector<string> AvailableDrivers;
	string Driver, PreferredDriver;

	SDLRuntime(SDLRuntime &) = delete;
	void operator=(const SDLRuntime &) = delete;

	~SDLRuntime()
	{
		SDL_VideoQuit();
		SDL_Quit();
	}

private:
	explicit SDLRuntime()
	{
		SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

#if defined(_WIN32)
		SupportedBackends = {BackendType::D3D12, BackendType::D3D11, BackendType::Vulkan};
#elif defined(__APPLE__)
		SupportedBackends = {BackendType::Metal};
#elif defined(__linux__)
		SupportedBackends = {BackendType::Vulkan};
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

		SDL_Init(SDL_INIT_GAMECONTROLLER);
	}

public:
	static unique_ptr<SDLRuntime> Instance;
};

unique_ptr<SDLRuntime> SDLRuntime::Instance = unique_ptr<SDLRuntime>(new SDLRuntime());

Window::Window(const char *title, int width, int height)
    : title(title)
    , width(width)
    , height(height)
{
	BackendType = SDLRuntime::Instance->SupportedBackends[0];

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);

	if (window == NULL)
	{
		cerr << SDL_GetError() << endl;
		throw runtime_error("Could not create SDL Window");
	}
	SDL_SetWindowData(window, "DGame::Window", this);
}

Window *Window::FromSDLWindow(SDL_Window *window)
{
	return (Window*)SDL_GetWindowData(window, "DGame::Window");
}

unique_ptr<ChainedStruct> Window::createSurfaceDescriptor()
{
	// see https://wiki.libsdl.org/SDL2/SDL_SysWMinfo
	SDL_SysWMinfo sysWMInfo;
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(window, &sysWMInfo);

	auto driver = SDLRuntime::Instance->Driver;

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