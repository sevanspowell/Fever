#pragma once

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <Fever/Fever.h>
#include <Fever/PersistentHandleDataStore.h>

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
#define FV_MTL_RELEASE(obj)                                                    \
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

struct SubpassWrapper {
    MTLRenderPassDescriptor *mtlRenderPass;
    MTLRenderPipelineDescriptor *mtlPipelineDescriptor;
};

struct RenderPassWrapper {
    std::vector<SubpassWrapper> subpasses;
};

class MetalWrapper {
  public:
    static const uint32_t MAX_NUM_LIBRARIES     = 64;
    static const uint32_t MAX_NUM_RENDER_PASSES = 64;

    MetalWrapper()
        : metalLayer(NULL), device(nil), commandQueue(nil),
          libraries(MAX_NUM_LIBRARIES), renderPasses(MAX_NUM_RENDER_PASSES) {}

    FvResult init(const FvInitInfo *initInfo);

    void shutdown();

    FvResult renderPassCreate(FvRenderPass *renderPass,
                              const FvRenderPassCreateInfo *createInfo);

    void renderPassDestroy(FvRenderPass renderPass);

    FvResult pipelineLayoutCreate(FvPipelineLayout *layout,
                                  const FvPipelineLayoutCreateInfo *createInfo);

    void pipelineLayoutDestroy(FvPipelineLayout layout);

    FvResult shaderModuleCreate(FvShaderModule *shaderModule,
                                const FvShaderModuleCreateInfo *createInfo);

    void shaderModuleDestroy(FvShaderModule shaderModule);

  private:
    static MTLLoadAction toMtlLoadAction(FvLoadOp loadOp);

    static MTLStoreAction toMtlStoreAction(FvStoreOp storeOp);

    static MTLPixelFormat toMtlPixelFormat(FvFormat format);

    CAMetalLayer *metalLayer;
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;

    PersistentHandleDataStore<id<MTLLibrary>> libraries;
    PersistentHandleDataStore<RenderPassWrapper> renderPasses;
};
}
