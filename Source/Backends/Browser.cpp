int main()
{
	return 0;
}

#include "DGame/Backend.h"

#include <emscripten.h>
#include <emscripten/em_js.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>

#include <SDL2/SDL.h>

#include <cassert>
#include <functional>
#include <future>
#include <iostream>

#ifndef KEEP_IN_MODULE
#define KEEP_IN_MODULE extern "C" __attribute__((used, visibility("default")))
#endif
int main(int, char **);

KEEP_IN_MODULE void _async_main()
{
	main(0, nullptr);
}

using namespace std;
using namespace wgpu;

namespace DGame
{
struct Backend::IBackend
{
	Instance instance;
	int windowWidth, windowHeight;

	IBackend(const char *windowTitle, int windowWidth, int windowHeight)
	    : instance(CreateInstance())
	    , windowWidth(windowWidth)
	    , windowHeight(windowHeight)
	{
	}

	Device createDevice()
	{
		return Device::Acquire(emscripten_webgpu_get_device());
	}

	Surface createSurface(const char *canvasID)
	{
		SurfaceDescriptorFromCanvasHTMLSelector canvas{};
		canvas.sType = SType::SurfaceDescriptorFromCanvasHTMLSelector;
		canvas.selector = canvasID;

		SurfaceDescriptor descriptor{};
		descriptor.nextInChain = &canvas;

		return instance.CreateSurface(&descriptor);
	}

	SwapChain createSwapChain(Device device, Surface surface)
	{
		SwapChainDescriptor descriptor;
		descriptor.usage = TextureUsage::RenderAttachment;
		descriptor.format = TextureFormat::BGRA8Unorm;
		descriptor.width = windowWidth;
		descriptor.height = windowHeight;
		descriptor.presentMode = PresentMode::Fifo;

		return device.CreateSwapChain(surface, &descriptor);
	}
};

Backend::Backend(const char *windowTitle, int windowWidth, int windowHeight)
    : implementation(new Backend::IBackend(windowTitle, windowWidth, windowHeight))
{
	assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) == 0);

	device = implementation->createDevice();
	surface = implementation->createSurface("canvas#DGameViewport");
	swapchain = implementation->createSwapChain(device, surface);
	queue = device.GetQueue();

	/*queue.OnSubmittedWorkDone(WGPUQueueWorkDoneStatus_Success, [](WGPUQueueWorkDoneStatus status, void* userData){
	    cout << status << endl;
	},this);
	*/
}

void Backend::Start()
{
	auto cb = [](double time, void *userData) -> EM_BOOL {
		auto _this = (Backend *)userData;
		_this->draw();
		return _this->IsRendering;
	};
	emscripten_request_animation_frame_loop(cb, this);
}

void Backend::Yield()
{
	emscripten_sleep(0);
}

Backend::~Backend()
{
}

} // namespace DGame

__attribute__((constructor)) void init()
{
	EM_ASM({
		if (navigator["gpu"])
		{
			navigator["gpu"]["requestAdapter"]().then(
			    function(adapter) {
				    adapter["requestDevice"]().then(function(device) {
					    Module["preinitializedWebGPUDevice"] = device;
					    __async_main();
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
}