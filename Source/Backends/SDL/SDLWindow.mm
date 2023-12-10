/**
 * SDLWindow.mm - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "SDLWindow.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <SDL_syswm.h>

using namespace std;
using namespace wgpu;

namespace DGame {
unique_ptr<ChainedStruct>
Window::createSurfaceDescriptor()
{
  @autoreleasepool
  {
    SDL_SysWMinfo sysWMInfo;
    SDL_VERSION(&sysWMInfo.version);
    SDL_GetWindowWMInfo(window, &sysWMInfo);

    auto cocoaWindow = sysWMInfo.info.cocoa.window;

    auto descriptor = make_unique<SurfaceDescriptorFromMetalLayer>();

    NSView *view = [cocoaWindow contentView];

    // Create a CAMetalLayer that covers the whole window that will be passed to
    // CreateSurface.
    [view setWantsLayer:YES];
    [view setLayer:[CAMetalLayer layer]];

    // Use retina if the window was created with retina support.
    [[view layer] setContentsScale:[cocoaWindow backingScaleFactor]];
      
    descriptor->layer = [view layer];
    return std::move(descriptor);
  }
};
} // namespace DGame
