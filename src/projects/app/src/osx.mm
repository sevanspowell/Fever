#include "osx.h"

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

FvResult getOSXSurface(FvSurface *surface, SDL_SysWMinfo windowInfo) {
    FvResult result = FV_RESULT_FAILURE;

    if (surface != NULL) {
        NSWindow *nsWindow = (NSWindow *)windowInfo.info.cocoa.window;

        FvCocoaSurfaceCreateInfo surfaceCreateInfo;
        surfaceCreateInfo.view = (void *)(nsWindow.contentView);

        result = fvCreateCocoaSurface(surface, &surfaceCreateInfo);
    }

    return result;
}
