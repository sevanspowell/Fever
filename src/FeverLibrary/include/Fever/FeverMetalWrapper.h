#pragma once

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <Fever/Fever.h>
#include <Fever/PersistentHandleDataStore.h>

namespace fv {
// clang-format off
#define FV_MTL_RELEASE(obj)                                                    \
    do {                                                                       \
        [obj release];                                                         \
        obj = nil;                                                             \
    } while (0)
// clang-format on

struct ImageWrapper {
    ImageWrapper() : isDrawable(false), texture(nil) {}

    bool isDrawable;
    id<MTLTexture> texture;
};

struct ImageViewWrapper {
    ImageViewWrapper() : referencesDrawable(false), texture(nil) {}

    bool referencesDrawable;
    id<MTLTexture> texture;
};

struct SubpassWrapper {
    MTLRenderPassDescriptor *mtlRenderPass;
    MTLRenderPipelineDescriptor *mtlPipelineDescriptor;

    std::vector<FvAttachmentReference> inputAttachments;
    std::vector<FvAttachmentReference> colorAttachments;
    std::vector<FvAttachmentReference> depthStencilAttachment;
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
    MTLPrimitiveType primitiveType;
    FvPipelineVertexInputDescription vertexInputDescription;

    std::vector<FvAttachmentReference> inputAttachments;
    std::vector<FvAttachmentReference> colorAttachments;
    std::vector<FvAttachmentReference> depthStencilAttachment;
};

struct FramebufferWrapper {
    std::vector<ImageViewWrapper> attachments;
};

typedef enum DrawCallType {
    DRAW_CALL_TYPE_NON_INDEXED,
    DRAW_CALL_TYPE_INDEXED,
} DrawCallType;

struct DrawCallNonIndexed {
    DrawCallType type;
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
};

struct DrawCallIndexed {
    DrawCallType type;
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;
};

union DrawCall {
    DrawCallType type;
    DrawCallNonIndexed nonIndexed;
    DrawCallIndexed indexed;
};

struct BufferWrapper {
    id<MTLBuffer> mtlBuffer;

    // Relevant to vertex buffers
    FvSize bindingPoint;

    // Relevant to index buffers
    MTLIndexType mtlIndexType;

    // Relevant to either
    FvSize offset;
};

struct CommandBufferWrapper {
    CommandBufferWrapper()
        : commandQueue(nil), readyForSubmit(false),
          graphicsPipeline(FV_NULL_HANDLE), indexBuffer(FV_NULL_HANDLE) {}

    id<MTLCommandQueue> commandQueue;
    // FvGraphicsPipeline graphicsPipelineHandle;
    std::vector<FvClearValue> clearValues;
    std::vector<ImageViewWrapper> attachments;
    bool readyForSubmit;

    FvGraphicsPipeline graphicsPipeline;
    DrawCall drawCall;

    std::vector<FvBuffer> vertexBuffers;
    FvBuffer indexBuffer;

    std::vector<FvDescriptorSet> descriptorSets;
};

struct SemaphoreWrapper {
  public:
    SemaphoreWrapper() { semaphore = dispatch_semaphore_create(0); }

    /**
     * Signals (increments) the semaphore.
     *
     * Increment the semaphore. If the previous value was less than zero, this
     * method will wake a thread currently waiting.
     */
    void signal() { dispatch_semaphore_signal(semaphore); }

    /**
     * Waits for (decrements) the semaphore.
     *
     * If the resulting value of the semaphore is less than zero, this method
     * will block until the semaphore is signalled.
     */
    void wait() { dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER); }

    void release() { dispatch_release(semaphore); }

  private:
    dispatch_semaphore_t semaphore;
};

struct SwapchainWrapper {
    FvExtent3D extent;
};

struct DescriptorBufferBinding {
    FvDescriptorBufferInfo bufferInfo;
    FvDescriptorSetLayoutBinding binding;
};

struct DescriptorSetLayoutWrapper {
    std::vector<FvDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
};

struct DescriptorSetWrapper {
    FvDescriptorSetLayout descriptorSetLayout;
    std::vector<DescriptorBufferBinding> bufferBindings;
};

struct DescriptorPoolWrapper {
    struct PoolInfo {
        FvDescriptorType descriptorType;
        uint32_t descriptorCount;
    };
    struct Pool {
        std::vector<FvDescriptorSet> descriptorSets;
    };

    std::vector<PoolInfo> poolInfos;
    Pool pool;
    uint32_t maxSets;
};

class MetalWrapper {
  public:
    static const uint32_t MAX_NUM_LIBRARIES              = 64;
    static const uint32_t MAX_NUM_RENDER_PASSES          = 64;
    static const uint32_t MAX_NUM_GRAPHICS_PIPELINES     = 64;
    static const uint32_t MAX_NUM_TEXTURES               = 256;
    static const uint32_t MAX_NUM_TEXTURE_VIEWS          = 256;
    static const uint32_t MAX_NUM_FRAMEBUFFERS           = 64;
    static const uint32_t MAX_NUM_COMMAND_QUEUES         = 64;
    static const uint32_t MAX_NUM_COMMAND_BUFFERS        = 64;
    static const uint32_t MAX_NUM_DRAWABLES              = 32;
    static const uint32_t MAX_NUM_SEMAPHORES             = 32;
    static const uint32_t MAX_NUM_SWAPCHAINS             = 16;
    static const uint32_t MAX_NUM_BUFFERS                = 256;
    static const uint32_t MAX_NUM_DESCRIPTOR_SET_LAYOUTS = 256;
    static const uint32_t MAX_NUM_DESCRIPTOR_POOLS       = 64;
    static const uint32_t MAX_NUM_DESCRIPTOR_SETS        = 512;

    MetalWrapper()
        : metalLayer(NULL), device(nil), libraries(MAX_NUM_LIBRARIES),
          renderPasses(MAX_NUM_RENDER_PASSES),
          graphicsPipelines(MAX_NUM_GRAPHICS_PIPELINES),
          textures(MAX_NUM_TEXTURES), textureViews(MAX_NUM_TEXTURE_VIEWS),
          framebuffers(MAX_NUM_FRAMEBUFFERS),
          commandQueues(MAX_NUM_COMMAND_QUEUES),
          commandBuffers(MAX_NUM_COMMAND_BUFFERS),
          semaphores(MAX_NUM_SEMAPHORES), swapchains(MAX_NUM_SWAPCHAINS),
          buffers(MAX_NUM_BUFFERS),
          descriptorSetLayouts(MAX_NUM_DESCRIPTOR_SET_LAYOUTS),
          descriptorPools(MAX_NUM_DESCRIPTOR_POOLS),
          descriptorSets(MAX_NUM_DESCRIPTOR_SETS) {}

    FvResult init(const FvInitInfo *initInfo);

    void shutdown();

    FvResult descriptorPoolCreate(FvDescriptorPool *descriptorPool,
                                  const FvDescriptorPoolCreateInfo *createInfo);

    void descriptorPoolDestroy(FvDescriptorPool descriptorPool);

    FvResult
    allocateDescriptorSets(FvDescriptorSet *descriptorSets,
                           const FvDescriptorSetAllocateInfo *allocateInfo);

    void updateDescriptorSets(uint32_t descriptorWriteCount,
                              const FvWriteDescriptorSet *descriptorWrites);

    FvResult descriptorSetLayoutCreate(
        FvDescriptorSetLayout *descriptorSetLayout,
        const FvDescriptorSetLayoutCreateInfo *createInfo);

    void descriptorSetLayoutDestroy(FvDescriptorSetLayout descriptorSetLayout);

    FvResult bufferCreate(FvBuffer *buffer,
                          const FvBufferCreateInfo *createInfo);

    void bufferDestroy(FvBuffer buffer);

    void bufferReplaceData(FvBuffer buffer, void *data, size_t dataSize);

    FvResult semaphoreCreate(FvSemaphore *semaphore);

    void semaphoreDestroy(FvSemaphore semaphore);

    FvResult acquireNextImage(FvSwapchain swapchain,
                              FvSemaphore imageAvailableSemaphore,
                              uint32_t *imageIndex);

    FvResult createSwapchain(FvSwapchain *swapchain,
                             const FvSwapchainCreateInfo *createInfo);

    void destroySwapchain(FvSwapchain swapchain);

    void getSwapchainImage(FvSwapchain swapchain, FvImage *swapchainImage);

    void queuePresent(const FvPresentInfo *presentInfo);

    FvResult queueSubmit(uint32_t submissionsCount,
                         const FvSubmitInfo *submissions);

    // FvResult getDrawable(FvDrawable *drawable);

    // FvResult getDrawableImage(FvImage *drawableImage, FvDrawable drawable);

    void cmdBindGraphicsPipeline(FvCommandBuffer commandBuffer,
                                 FvGraphicsPipeline graphicsPipeline);

    void cmdBindVertexBuffers(FvCommandBuffer commandBuffer,
                              uint32_t firstBinding, uint32_t bindingCount,
                              const FvBuffer *buffers, const FvSize *offsets);

    void cmdBindIndexBuffer(FvCommandBuffer commandBuffer, FvBuffer buffer,
                            FvSize offset, FvIndexType indexType);

    void cmdBindDescriptorSets(FvCommandBuffer commandBuffer,
                               FvPipelineLayout layout, uint32_t firstSet,
                               uint32_t descriptorSetCount,
                               const FvDescriptorSet *descriptorSets);

    void cmdDraw(FvCommandBuffer commandBuffer, uint32_t vertexCount,
                 uint32_t instanceCount, uint32_t firstVertex,
                 uint32_t firstInstance);

    void cmdDrawIndexed(FvCommandBuffer commandBuffer, uint32_t indexCount,
                        uint32_t instanceCount, uint32_t firstIndex,
                        int32_t vertexOffset, uint32_t firstInstance);

    void cmdBeginRenderPass(FvCommandBuffer commandBuffer,
                            const FvRenderPassBeginInfo *renderPassInfo);

    void cmdEndRenderPass(FvCommandBuffer commandBuffer);

    FvResult commandBufferCreate(FvCommandBuffer *commandBuffer,
                                 FvCommandPool commandPool);

    void commandBufferBegin(FvCommandBuffer commandBuffer);

    FvResult commandBufferEnd(FvCommandBuffer commandBuffer);

    FvResult commandPoolCreate(FvCommandPool *commandPool,
                               const FvCommandPoolCreateInfo *createInfo);

    void commandPoolDestroy(FvCommandPool commandPool);

    FvResult framebufferCreate(FvFramebuffer *framebuffer,
                               const FvFramebufferCreateInfo *createInfo);

    void framebufferDestroy(FvFramebuffer framebuffer);

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
    static MTLIndexType toMtlIndexType(FvIndexType indexType);

    static MTLStepFunction toMtlStepFunction(FvVertexInputRate inputRate);

    static MTLVertexFormat toMtlVertexFormat(FvVertexFormat format);

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

    static MTLPrimitiveType toMtlPrimitiveType(FvPrimitiveType primitiveType);

    CAMetalLayer *metalLayer;
    id<MTLDevice> device;

    PersistentHandleDataStore<id<MTLLibrary>> libraries;
    PersistentHandleDataStore<RenderPassWrapper> renderPasses;
    PersistentHandleDataStore<GraphicsPipelineWrapper> graphicsPipelines;
    PersistentHandleDataStore<ImageWrapper> textures;
    PersistentHandleDataStore<ImageViewWrapper> textureViews;
    PersistentHandleDataStore<FramebufferWrapper> framebuffers;
    PersistentHandleDataStore<id<MTLCommandQueue>> commandQueues;
    PersistentHandleDataStore<CommandBufferWrapper> commandBuffers;
    PersistentHandleDataStore<SemaphoreWrapper> semaphores;
    PersistentHandleDataStore<SwapchainWrapper> swapchains;
    PersistentHandleDataStore<BufferWrapper> buffers;
    PersistentHandleDataStore<DescriptorSetLayoutWrapper> descriptorSetLayouts;
    PersistentHandleDataStore<DescriptorPoolWrapper> descriptorPools;
    PersistentHandleDataStore<DescriptorSetWrapper> descriptorSets;

    id<CAMetalDrawable> currentDrawable;
    id<MTLCommandQueue> currentCommandQueue;
};
}
