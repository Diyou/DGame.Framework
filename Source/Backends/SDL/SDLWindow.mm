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

#import <QuartzCore/CAMetalLayer.h>

namespace DGame{
void * Window::GetLayerFromCocoaWindow(NSWindow *window){
@autoreleasepool {
    NSView* view = [window contentView];

    // Create a CAMetalLayer that covers the whole window that will be passed to
    // CreateSurface.
    [view setWantsLayer:YES];
    [view setLayer:[CAMetalLayer layer]];

    // Use retina if the window was created with retina support.
    [[view layer] setContentsScale:[window backingScaleFactor]];

    return [view layer];
}
};
}
