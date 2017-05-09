#include <iostream>
#include <sstream>
#include <stdexcept>

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

int main(void) {
        @autoreleasepool {
            try {
                // Initialize SDL2
                if (SDL_Init(SDL_INIT_VIDEO)) {
                    throw std::runtime_error("Failed to initialize SDL2");
                }

                uint32_t flags = 0x0;
                flags |= SDL_WINDOW_RESIZABLE;

                // Create window
                SDL_Window *window =
                    SDL_CreateWindow("Test bed", SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, 800, 600, flags);

                if (window == nullptr) {
                    std::stringstream err;
                    err << "Could not create window: " << SDL_GetError();
                    throw std::runtime_error(err.str());
                }

                // Create surface
                SDL_SysWMinfo windowInfo;
                SDL_VERSION(&windowInfo.version);
                if (!SDL_GetWindowWMInfo(window, &windowInfo)) {
                    throw std::runtime_error("Failed to get SDL2 window info.");
                }

                NSWindow *nsWindow = (NSWindow *)windowInfo.info.cocoa.window;

                void *view = (void *)(nsWindow.contentView);

                CAMetalLayer *metalLayer = nullptr;

                // Attempt to get a CAMetalLayer from MTKView
                if (NSClassFromString(@"MTKView") != NULL) {
                    MTKView *mtkView = (MTKView *)view;

                    if (mtkView != NULL &&
                        [mtkView isKindOfClass:NSClassFromString(@"MTKView")]) {
                        metalLayer = (CAMetalLayer *)mtkView.layer;
                    }
                }

                // Attempt to get CAMetalLayer from NSWindow if previous attempt
                // failed.
                if (NSClassFromString(@"CAMetalLayer") != NULL) {
                    if (metalLayer == NULL) {
                        NSView *nsView = (NSView *)view;
                        [nsView setWantsLayer:YES];
                        metalLayer = [CAMetalLayer layer];
                        [nsView setLayer:metalLayer];
                    }
                }

                if (metalLayer == nullptr) {
                    throw std::runtime_error("Failed to create metal layer!");
                }

                id<MTLDevice> device = MTLCreateSystemDefaultDevice();
                if (device == nil) {
                    throw std::runtime_error("Failed to create device!");
                }

                metalLayer.device = device;
                device            = nil;

                MTLRenderPassDescriptor *renderdesc =
                    [MTLRenderPassDescriptor renderPassDescriptor];
                MTLRenderPassColorAttachmentDescriptor *colorattachment =
                    renderdesc.colorAttachments[0];

                /* Clear to a red-orange color when beginning the render pass.
                 */
                colorattachment.clearColor =
                    MTLClearColorMake(1.0, 0.3, 0.0, 1.0);
                colorattachment.loadAction  = MTLLoadActionClear;
                colorattachment.storeAction = MTLStoreActionStore;

                id<MTLCommandQueue> queue = [metalLayer.device newCommandQueue];

                bool shouldQuit      = false;
                bool shouldDrawFrame = true;
                while (!shouldQuit) {
                    @autoreleasepool {
                        SDL_Event event;
                        while (SDL_PollEvent(&event)) {
                            switch (event.type) {
                            case SDL_QUIT: {
                                shouldQuit = true;
                                break;
                            }
                            case SDL_KEYUP: {
                                shouldDrawFrame = true;
                                break;
                            }
                            }
                        }

                        
                        if (shouldDrawFrame) {
                            id<MTLCommandBuffer> cmdbuf = [queue commandBuffer];

                            id<CAMetalDrawable> drawable =
                                [metalLayer nextDrawable];
                            colorattachment.texture = drawable.texture;

                            id<MTLRenderCommandEncoder> encoder = [cmdbuf
                                renderCommandEncoderWithDescriptor:renderdesc];
                            [encoder endEncoding];

                            [cmdbuf presentDrawable:drawable];
                            [cmdbuf commit];
                        }
                        
                        // shouldDrawFrame = false;
                    }
                }

                SDL_DestroyWindow(window);

                SDL_Quit();
            } catch (const std::runtime_error &err) {
                std::cerr << err.what() << std::endl;
                return EXIT_FAILURE;
            }
        }

    return EXIT_SUCCESS;
}
