#pragma once

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <Fever/Fever.h>

namespace fv {
// // clang-format off
// #define MTL_CLASS(name)                                                        \
// class name                                                                     \
// {                                                                              \
// public:                                                                        \
//     name(id<MTL##name> obj_ = nil) : obj(obj_)                                 \
//     {                                                                          \
//     }                                                                          \
//     operator id<MTL##name>() const                                             \
//     {                                                                          \
//         return obj;                                                            \
//     }                                                                          \
//     id<MTL##name> obj;

// #define MTL_CLASS_END                                                          \
//     };

// clang-format off
#define MTL_RELEASE(obj)                                                       \
    do {                                                                       \
        [obj release];                                                         \
        obj = nil;                                                             \
    } while (0)
// clang-format on

// namespace mtl {
// MTL_CLASS(Device)
// id<MTLCommandQueue> newCommandQueue() { return [obj newCommandQueue]; }
// MTL_CLASS_END

// MTL_CLASS(CommandQueue)
// id<MTLCommandBuffer> commandBuffer() { return [obj commandBuffer]; }
// MTL_CLASS_END
// }

// class CommandQueueWrapper {
//   public:
//     void init(mtl::Device device);
//     void shutdown();
//     void release(NSObject *ptr);
//     void consume();

//     mtl::CommandQueue commandQueue;
// };

class MetalWrapper {
  public:
    MetalWrapper() : metalLayer(NULL), device(nil), commandQueue(nil) {}

    FvResult init(const FvInitInfo *initInfo);

    void shutdown();

  private:
    CAMetalLayer *metalLayer;
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
};
}
