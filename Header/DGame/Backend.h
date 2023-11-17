#pragma once

#include "webgpu/webgpu_cpp.h"

#ifdef __EMSCRIPTEN__
// Hookup async initiation
#define main __main__
#endif

#include <memory>

namespace DGame
{

class Backend
{
	struct IBackend;
	std::unique_ptr<IBackend> implementation;

protected:
	wgpu::Device device;
	wgpu::Surface surface;
	wgpu::SwapChain swapchain;
	wgpu::Queue queue;
	virtual void draw() = 0;

public:
	/** @fn void Backend::Backend(const char *windowTitle, int windowWidth, int windowHeight)
	 *  @brief Creates a DAWN rendering context.
	 *  @param windowTitle:		The window title.
	 *  @param windowWidth: 	The window width.
	 *  @param windowHeight:	The window height.
	 */
	Backend(const char *windowTitle, int windowWidth, int windowHeight);
	
	bool IsRendering = true;

	/** @fn void Backend::Start()
	 *  @brief Start Rendering
	 */
	void Start();

	/** @fn void Backend::Yield()
	 *  @brief Yield control back to the Backend.
	 */
	void Yield();

	virtual ~Backend();
};
} // namespace DGame