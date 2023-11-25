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

int
main()
{
	return 0;
}

#include "DGame/Backend.h"

#include <emscripten.h>
#include <emscripten/em_js.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>

#include "SDLWindow.h"

#include <cassert>
#include <functional>
#include <future>
#include <iostream>

/*
#ifndef KEEP_IN_MODULE
#define KEEP_IN_MODULE extern "C" __attribute__((used, visibility("default")))
#endif
int main(int, const char **);

KEEP_IN_MODULE void _async_main()
{
    main(0, nullptr);
}*/

using namespace std;
using namespace wgpu;

namespace DGame
{
struct Backend::IBackend : public Window
{
	Instance instance;
	bool isAlive = true;

	IBackend(const char *windowTitle, int windowWidth, int windowHeight)
	    : instance(CreateInstance())
	    , Window(windowTitle, windowWidth, windowHeight)
	{
		EM_ASM(
	    {
		    let canvas = document.querySelector("canvas#canvas");
		    let original = canvas.cloneNode(true);
		    original.style.position = "absolute";
		    original.style.zIndex = -1;
		    canvas.removeAttribute("style");
		    canvas.id = `SDLWindow${$1}`;
		    canvas.title = UTF8ToString($0);
		    document.body.insertBefore(original, canvas);
	    },
	    Title(),
	    SDL_GetWindowID(window)
	);
	}

	Device
	createDevice()
	{
		return Device::Acquire(emscripten_webgpu_get_device());
	}

	Surface
	createSurface()
	{
		auto canvas = createSurfaceDescriptor();

		SurfaceDescriptor descriptor{};
		descriptor.nextInChain = canvas.get();

		return instance.CreateSurface(&descriptor);
	}

	SwapChain
	createSwapChain(Device device, Surface surface)
	{
		auto size = Size();
		SwapChainDescriptor descriptor;
		descriptor.usage = TextureUsage::RenderAttachment;
		descriptor.format = TextureFormat::BGRA8Unorm;
		descriptor.width = size.width;
		descriptor.height = size.height;
		descriptor.presentMode = PresentMode::Fifo;

		return device.CreateSwapChain(surface, &descriptor);
	}
};

Backend::Backend(const char *windowTitle, int windowWidth, int windowHeight)
    : implementation(new Backend::IBackend(windowTitle, windowWidth, windowHeight))
    , IsRendering(implementation->isAlive)
{
	device = implementation->createDevice();
	surface = implementation->createSurface();
	swapchain = implementation->createSwapChain(device, surface);
	queue = device.GetQueue();

	/*queue.OnSubmittedWorkDone(WGPUQueueWorkDoneStatus_Success, [](WGPUQueueWorkDoneStatus status, void* userData){
	    cout << status << endl;
	},this);
	*/
	emscripten_async_call(
	    [](void *userData) {
		    auto _this = (Backend *)userData;
		    _this->Start();
	    },
	    this,
	    0
	);
}

void
Backend::Start()
{
	auto cb = [](double time, void *userData) -> EM_BOOL {
		auto _this = (Backend *)userData;
		_this->draw();
		_this->Start();
		return _this->IsRendering;
	};
	emscripten_request_animation_frame(cb, this);
}

Backend::~Backend()
{
}

} // namespace DGame

/*
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
*/
