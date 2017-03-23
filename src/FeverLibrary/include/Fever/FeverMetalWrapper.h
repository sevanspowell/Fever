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

struct GraphicsPipelineWrapper {
    MTLRenderPassDescriptor *renderPass;
    MTLCullMode cullMode;
    MTLWinding windingOrder;
    MTLDepthClipMode depthClipMode;
    id<MTLDepthStencilState> depthStencilState;
    id<MTLRenderPipelineState> renderPipelineState;
    MTLViewport viewport;
    MTLScissorRect scissor;
};

class MetalWrapper {
  public:
    static const uint32_t MAX_NUM_LIBRARIES          = 64;
    static const uint32_t MAX_NUM_RENDER_PASSES      = 64;
    static const uint32_t MAX_NUM_GRAPHICS_PIPELINES = 64;
    static const uint32_t MAX_NUM_TEXTURES           = 256;
    static const uint32_t MAX_NUM_TEXTURE_VIEWS      = 256;

    MetalWrapper()
        : metalLayer(NULL), device(nil), commandQueue(nil),
          libraries(MAX_NUM_LIBRARIES), renderPasses(MAX_NUM_RENDER_PASSES),
          graphicsPipelines(MAX_NUM_GRAPHICS_PIPELINES),
          textures(MAX_NUM_TEXTURES), textureViews(MAX_NUM_TEXTURE_VIEWS) {}

    FvResult init(const FvInitInfo *initInfo);

    void shutdown();

    FvResult imageViewCreate(FvImageView *imageView,
                             const FvImageViewCreateInfo *createInfo);

    void imageViewDestroy(FvImageView imageView);

    FvResult imageCreate(FvImage *image, const FvImageCreateInfo *createInfo);

    void imageDestroy(FvImage image);

    FvResult
    graphicsPipelineCreate(FvGraphicsPipeline *graphicsPipeline,
                           const FvGraphicsPipelineCreateInfo *createInfo);

    void graphicsPipelineDestroy(FvGraphicsPipeline graphicsPipeline);

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

    static MTLBlendFactor toMtlBlendFactor(FvBlendFactor factor);

    static MTLBlendOperation toMtlBlendOperation(FvBlendOp op);

    static BOOL toObjCBool(bool b);

    static MTLWinding toMtlWindingOrder(FvWindingOrder winding);

    static MTLCullMode toMtlCullMode(FvCullMode cull);

    static MTLStencilOperation toMtlStencilOperation(FvStencilOp stencil);

    static MTLCompareFunction toMtlCompareFunction(FvCompareFunc compare);

    static uint32_t toMtlSampleCount(FvSampleCount samples);

    static MTLTextureUsage toMtlTextureUsage(FvImageUsage imageUsage);

    CAMetalLayer *metalLayer;
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;

    PersistentHandleDataStore<id<MTLLibrary>> libraries;
    PersistentHandleDataStore<RenderPassWrapper> renderPasses;
    PersistentHandleDataStore<GraphicsPipelineWrapper> graphicsPipelines;
    PersistentHandleDataStore<id<MTLTexture>> textures;
    PersistentHandleDataStore<id<MTLTexture>> textureViews;
};
}
