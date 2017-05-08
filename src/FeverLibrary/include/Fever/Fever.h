/*===-- Fever/Fever.h - Fever Graphics Library API ------------------*- C -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * \brief Public API for the Fever Graphics Library.
 *
 * Heavily based off the Vulkan (https://www.khronos.org/vulkan/) and Metal
 * (https://developer.apple.com/metal/) APIs.
 *
 *===----------------------------------------------------------------------===*/
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <Fever/FeverConstants.h>
#include <Fever/FeverPlatform.h>

#define FV_NULL_HANDLE 0
#define FV_DEFINE_HANDLE(object) typedef struct object##_t *object;

typedef union FvClearColor {
    float float32[4];
    int32_t int32[4];
    uint32_t uint32[4];
} FvClearColor;

typedef struct FvClearDepthStencil {
    float depth;
    uint32_t stencil;
} FvClearDepthStencil;

typedef union FvClearValue {
    FvClearColor color;
    FvClearDepthStencil depthStencil;
} FvClearValue;

typedef struct FvOffset2D {
    int32_t x;
    int32_t y;
} FvOffset2D;

typedef struct FvExtent2D {
    uint32_t width;
    uint32_t height;
} FvExtent2D;

typedef struct FvExtent3D {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} FvExtent3D;

typedef struct FvRect2D {
    FvOffset2D offset;
    FvExtent2D extent;
} FvRect2D;

/* /\** Structure specifying creation parameters for a buffer. *\/ */
/* typedef struct FvBufferCreateInfo { */
/*     FvBufferType type; /\** Type of the buffer. *\/ */
/*     const void *data;  /\** Buffer data. *\/ */
/*     size_t size;       /\** Size of the buffer data in bytes. *\/ */
/* } FvBufferCreateInfo; */

/* /\** Opaque handle to buffer object. *\/ */
/* FV_DEFINE_HANDLE(FvBuffer); */

/* extern void fvBufferCreate(FvBuffer *buffer, */
/*                            const FvBufferCreateInfo *createInfo); */

/* extern void fvBufferDestroy(FvBuffer buffer); */

/** Opaque handle to shader object. */
FV_DEFINE_HANDLE(FvShaderModule);

/** Structure specifying creation parameters for a shader. */
typedef struct FvShaderModuleCreateInfo {
    /** Shader data */
    const void *data;
    /** Size of the shader data in bytes. */
    size_t size;
} FvShaderModuleCreateInfo;

extern FvResult
fvShaderModuleCreate(FvShaderModule *shaderModule,
                     const FvShaderModuleCreateInfo *createInfo);

extern void fvShaderModuleDestroy(FvShaderModule shaderModule);

/** Opaque handle to image object. */
FV_DEFINE_HANDLE(FvImage);

/** Structure specifying creation parameters for a image. */
typedef struct FvImageCreateInfo {
    /** Format of each pixel in the image */
    FvFormat format;
    /** Dimensionality of the image */
    FvImageType imageType;
    /** Dimensions of image */
    FvExtent3D extent;
    /** Number of mipmap levels */
    uint32_t numMipmapLevels;
    /** Number of layers in image */
    uint32_t arrayLayers;
    /** Number of samples in each pixel */
    FvSampleCount numSamples;
    /** How the image will be used (bitmask of FvImageUsage) */
    FvImageUsage usage;
} FvImageCreateInfo;

extern FvResult fvImageCreate(FvImage *image,
                              const FvImageCreateInfo *createInfo);

extern void fvImageDestroy(FvImage image);

typedef struct FvStencilOperationState {
    /** Operation performed to update the values in the stencil attachment when
     * the stencil test fails */
    FvStencilOp stencilFailOp;
    /** Operation performed to update the values in the stencil attachment when
     * the stencil test passes, but the depth test fails */
    FvStencilOp depthFailOp;
    /** Operation performed to update the values in the stencil attachment when
     * both the depth and stencil tests pass */
    FvStencilOp depthStencilPassOp;
    /** Comparison function used between the masked reference value and a masked
     * value in the stencil attachment to determine pass/fail of stencil test */
    FvCompareFunc stencilCompareFunc;
    /** Bit mask dictating which bits the stencil comparison test can read */
    uint32_t readMask;
    /** Bit mask dictating which bits the stencil comparison test can write */
    uint32_t writeMask;
} FvStencilOperationState;

typedef struct FvPipelineDepthStencilStateDescription {
    /** Comparison function used to compare fragment's depth value and depth
     * value in the attachment, deciding whether or not to discard the fragment.
     */
    FvCompareFunc depthCompareFunc;
    /** True if depth writing to attachment is enabled, false otherwise. */
    bool depthWriteEnable;
    /** Stencil descriptor for back-facing primitives. */
    FvStencilOperationState backFaceStencil;
    /** Stencil descriptor for front-facing primitives. */
    FvStencilOperationState frontFaceStencil;
} FvPipelineDepthStencilStateDescription;

typedef struct FvColorBlendAttachmentState {
    /** True if blending is enabled, false otherwise. If disabled, source
     * fragment's color for this attachment is not modified. */
    bool blendEnable;
    /** Source blend factor to be used by color blend operation */
    FvBlendFactor srcColorBlendFactor;
    /** Destination blend factor to be used by color blend operation */
    FvBlendFactor dstColorBlendFactor;
    /** Blend operation to use for color data */
    FvBlendOp colorBlendOp;
    /** Source blend factor to be used by alpha blend operation */
    FvBlendFactor srcAlphaBlendFactor;
    /** Destination blend factor to be used by alpha blend operation */
    FvBlendFactor dstAlphaBlendFactor;
    /** Blend operation to use for alpha data */
    FvBlendOp alphaBlendOp;
    /** Bitmask which restricts the color components that may be written to
*/
    FvColorComponentFlags colorWriteMask;
} FvColorBlendAttachmentState;

typedef struct FvPipelineColorBlendStateDescription {
    /* Number of color blend attachment states */
    uint32_t attachmentCount;
    /** Array of color blend attachments */
    const FvColorBlendAttachmentState *attachments;
} FvPipelineColorBlendStateDescription;

typedef struct FvPipelineShaderStageDescription {
    /** Stage of the shader pipeline */
    FvShaderStage stage;
    /** Name of function that is the entry point for this shader stage */
    const char *entryFunctionName;
    /** Shader code bundle to use */
    FvShaderModule shaderModule;
} FvPipelineShaderStageDescription;

/** Region of the framebuffer the output will be rendered to */
typedef struct FvViewport {
    /** Viewport's upper left corner */
    float x, y;
    /** Viewport's width and height */
    float width, height;
    /** Depth range of the viewport [0, 1] */
    float minDepth, maxDepth;
} FvViewport;

/** Describes viewport to render through */
typedef struct FvPipelineViewportDescription {
    FvViewport viewport;
    FvRect2D scissor;
} FvPipelineViewportDescription;

/** Rasterizer turns geometry into fragments, this struct is used to
configure
 * it */
/** NOTE: FvPipelineConfig ??? */
typedef struct FvPipelineRasterizerDescription {
    /** Clamp fragments beyond near and far planes instead of discarding
them */
    bool depthClampEnable;
    /** If true, geometry never passes through rasterizer stage */
    /* bool rasterizerDiscardEnable; */
    /** Cull front or back faces or none */
    FvCullMode cullMode;
    /** Winding order of front-facing primitives */
    FvWindingOrder frontFacing;
} FvPipelineRasterizerDescription;

/** Describes what kind of geometry will be drawn */
typedef struct FvPipelineInputAssemblyDescription {
    /** Primitive type */
    FvPrimitiveType primitiveType;
    /** If true the assembly is restarted if a special index value is
     * encountered (0xFFFFFFFF when index type is 32-bit uint or 0xFFFF when
     * index type is 16-bit uint). Is not allowed for 'list' primitive types. */
    bool primitiveRestartEnable;
} FvPipelineInputAssemblyDescription;

/** Describes at what rate to load vertex data from memory */
typedef struct FvVertexInputBindingDescription {
    /** Index of binding in array of bindings */
    uint32_t binding;
    /** Number of bytes from one entry to next */
    uint32_t stride;
    /** When to move to next data entry */
    FvVertexInputRate inputRate;
} FvVertexInputBindingDescription;

/** Describes how to extract a vertex attribute from vertex data */
typedef struct FvVertexInputAttributeDescription {
    /** Shader binding location for attribute */
    uint32_t location;
    /** Index of binding in array of bindings */
    uint32_t binding;
    /** Format of the vertext attribute (number of color channels of format
     * should match number of components in shader data type) */
    FvFormat format;
    /** Number of bytes from start of per-vertex data to begin reading from */
    uint32_t offset;
} FvVertexInputAttributeDescription;

typedef struct FvPipelineVertexInputDescription {
    /** Number of vertex binding descriptions */
    uint32_t vertexBindingDescriptionCount;
    /** Array of vertex binding descriptions (number in array should match
     * 'vertexBindingDescriptionCount' field). */
    const FvVertexInputBindingDescription *vertexBindingDescriptions;
    /** Number of vertex attribute descriptions */
    uint32_t vertexAttributeDescriptionCount;
    /** Array of vertex attribute descriptions (number in array should match
     * 'vertexAttributeDescriptionCount' field). */
    const FvVertexInputAttributeDescription *vertexAttributeDescriptions;
} FvPipelineVertexInputDescription;

FV_DEFINE_HANDLE(FvPipelineLayout);
FV_DEFINE_HANDLE(FvDescriptorSetLayout);

typedef struct FvPushConstantRange {
    /** Bitmask of shader stages that access this range of push constants */
    FvShaderStage stageFlags;
    /** Start offset of range (must be multiple of 4) */
    uint32_t offset;
    /** Size of range (must be multiple of 4) */
    uint32_t size;
} FvPushConstantRange;

/** Used to specify uniform values in shader. Describes complete set of
    resources that can be accessed by a pipline. */
typedef struct FvPipelineLayoutCreateInfo {
    /** Number of set layouts */
    uint32_t setLayoutCount;
    /** Array of descriptor set layouts */
    FvDescriptorSetLayout *setLayouts;
    /** Number of push constant ranges */
    uint32_t pushConstantRangeCount;
    /** Array of push constant ranges */
    const FvPushConstantRange *pushConstantRanges;
} FvPipelineLayoutCreateInfo;

extern FvResult
fvPipelineLayoutCreate(FvPipelineLayout *layout,
                       const FvPipelineLayoutCreateInfo *createInfo);

extern void fvPipelineLayoutDestroy(FvPipelineLayout layout);

FV_DEFINE_HANDLE(FvRenderPass);

typedef struct FvAttachmentDescription {
    /** Format of this image to be used as attachment */
    FvFormat format;
    /** Number of samples of image */
    FvSampleCount samples;
    /** Operation to perform on this attachment at start of rendering pass */
    FvLoadOp loadOp;
    /** Operation to perform on this attachment at end of rendering pass */
    FvStoreOp storeOp;
    /** Operation to perform on this attachment at start of rendering pass */
    FvLoadOp stencilLoadOp;
    /** Operation to perform on this attachment at end of rendering pass */
    FvStoreOp stencilStoreOp;
} FvAttachmentDescription;

typedef struct FvAttachmentReference {
    /** Index of attachment of render pass. Corresponds to index of attachment
     * in FvRenderPassCreateInfo struct. */
    uint32_t attachment;
} FvAttachmentReference;

typedef struct FvSubpassDescription {
    /** Number of input attachments in this subpass */
    uint32_t inputAttachmentCount;
    /** Array of input attachments */
    const FvAttachmentReference *inputAttachments;
    /** Number of color attachment outputs */
    uint32_t colorAttachmentCount;
    /** Array of color attachment output references */
    const FvAttachmentReference *colorAttachments;
    /** Pointer to a single depth stencil attachment reference */
    const FvAttachmentReference *depthStencilAttachment;
    /** Number of preserved attachments */
    uint32_t preservereAttachmentCount;
    /** Array of indices specifying which attachments are not used by the
    subpass but whose contents must be preserved. Corresponds to index of
    attachment in FvRenderPassCreateInfo struct. */
    const uint32_t *preserveAttachments;
} FvSubpassDescription;

#define FV_SUBPASS_EXTERNAL 0xFFFFFFFF

typedef struct FvSubpassDependency {
    /** Index of first subpass in dependency or FV_SUBPASS_EXTERNAL */
    uint32_t srcSubpass;
    /** Index of second subpass in dependency or FV_SUBPASS_EXTERNAL */
    uint32_t dstSubpass;
    /** Stage of the pipeline that first subpass should wait on */
    FvPipelineStage srcStageMask;
    /** Bitmask of allowed access of first subpass */
    FvAccessFlags srcAccessMask;
    /** Stage of the pipeline that second subpass should wait on */
    FvPipelineStage dstStageMask;
    /** Bitmask of allowed access of second subpass */
    FvAccessFlags dstAccessMask;
} FvSubpassDependency;

typedef struct FvRenderPassCreateInfo {
    /** Number of attachments */
    uint32_t attachmentCount;
    /** Array of render attachments */
    const FvAttachmentDescription *attachments;
    /** Number of subpasses, each render pass must have atleast one subpass */
    uint32_t subpassCount;
    /** Array of subpasses */
    const FvSubpassDescription *subpasses;
    /** Number of dependencies */
    uint32_t dependencyCount;
    /** Array of subpass dependencies */
    const FvSubpassDependency *dependencies;
} FvRenderPassCreateInfo;

extern FvResult fvRenderPassCreate(FvRenderPass *renderPass,
                                   const FvRenderPassCreateInfo *createInfo);

extern void fvRenderPassDestroy(FvRenderPass renderPass);

FV_DEFINE_HANDLE(FvGraphicsPipeline);

typedef struct FvGraphicsPipelineCreateInfo {
    /** Number of shader stages */
    uint32_t stageCount;
    /** Array of shader stages, number in array must match 'stageCount' field */
    const FvPipelineShaderStageDescription *stages;
    /** Vertex input descriptor */
    const FvPipelineVertexInputDescription *vertexInputDescription;
    /** Input assembly descriptor */
    const FvPipelineInputAssemblyDescription *inputAssemblyDescription;
    /* Viewport descriptor */
    const FvPipelineViewportDescription *viewportDescription;
    /** Rasterizer state descriptor */
    const FvPipelineRasterizerDescription *rasterizerDescription;
    /** Color blending state descriptor */
    const FvPipelineColorBlendStateDescription *colorBlendStateDescription;
    /** Depth stencil state descriptor */
    const FvPipelineDepthStencilStateDescription *depthStencilDescription;
    /** Pipeline layout - shader constants */
    FvPipelineLayout layout;
    /** Render pass */
    FvRenderPass renderPass;
    /** Index of subpass in render pass to use */
    uint32_t subpass;
} FvGraphicsPipelineCreateInfo;

extern FvResult
fvGraphicsPipelineCreate(FvGraphicsPipeline *graphicsPipeline,
                         const FvGraphicsPipelineCreateInfo *createInfo);

extern void fvGraphicsPipelineDestroy(FvGraphicsPipeline graphicsPipeline);

FV_DEFINE_HANDLE(FvImageView);

/** Image views are used to access image data from shaders */
typedef struct FvImageViewCreateInfo {
    /** Handle to image object */
    FvImage image;
    /** Type of image view */
    FvImageViewType viewType;
    /** Image view format */
    FvFormat format;
} FvImageViewCreateInfo;

extern FvResult fvImageViewCreate(FvImageView *imageView,
                                  const FvImageViewCreateInfo *createInfo);

extern void fvImageViewDestroy(FvImageView imageView);

FV_DEFINE_HANDLE(FvFramebuffer);

typedef struct FvFramebufferCreateInfo {
    /** Render pass object the framebuffer should be compatible with. */
    FvRenderPass renderPass;
    /** Number of image attachments */
    uint32_t attachmentCount;
    /** Array of image attachments */
    const FvImageView *attachments;
    /** Width of framebuffer */
    uint32_t width;
    /** Height of framebuffer */
    uint32_t height;
    /** Number of layers in image arrays */
    uint32_t layers;
} FvFramebufferCreateInfo;

extern FvResult fvFramebufferCreate(FvFramebuffer *framebuffer,
                                    const FvFramebufferCreateInfo *createInfo);

extern void fvFramebufferDestroy(FvFramebuffer framebuffer);

FV_DEFINE_HANDLE(FvCommandPool);

typedef struct FvCommandPoolCreateInfo {
} FvCommandPoolCreateInfo;

extern FvResult fvCommandPoolCreate(FvCommandPool *commandPool,
                                    const FvCommandPoolCreateInfo *createInfo);

extern void fvCommandPoolDestroy(FvCommandPool commandPool);

FV_DEFINE_HANDLE(FvCommandBuffer);

/** Create a command buffer from a command pool, command buffers are
 * automatically destroyed when their command pool is destroyed. */
extern FvResult fvCommandBufferCreate(FvCommandBuffer *commandBuffer,
                                      FvCommandPool commandPool);

extern void fvCommandBufferBegin(FvCommandBuffer commandBuffer);

extern FvResult fvCommandBufferEnd(FvCommandBuffer commandBuffer);

typedef struct FvRenderPassBeginInfo {
    /** Render pass to begin recording commands for */
    FvRenderPass renderPass;
    /** Framebuffer containing attachments to use for this render pass */
    FvFramebuffer framebuffer;
    /** Number of clear values */
    uint32_t clearValueCount;
    /** Array of clear values. One clear value for each attachment. Array is
     * indexed by attachment number */
    const FvClearValue *clearValues;
} FvRenderPassBeginInfo;

extern void fvCmdBeginRenderPass(FvCommandBuffer commandBuffer,
                                 const FvRenderPassBeginInfo *renderPassInfo);

extern void fvCmdEndRenderPass(FvCommandBuffer commandBuffer);

extern void fvCmdBindGraphicsPipeline(FvCommandBuffer commandBuffer,
                                      FvGraphicsPipeline graphicsPipeline);

/**
 * Record a non-indexed draw call into a command buffer.
 *
 * \param commandBuffer CommandBuffer to record draw call into.
 * \param vertexCount   Number of vertices to draw.
 * \param instanceCount Number of instances to draw.
 * \param firstVertex   Index of the first vertex to draw.
 * \param firstInstance Instance ID of the first instance to draw.
 */
extern void fvCmdDraw(FvCommandBuffer commandBuffer, uint32_t vertexCount,
                      uint32_t instanceCount, uint32_t firstVertex,
                      uint32_t firstInstance);

FV_DEFINE_HANDLE(FvSemaphore);

FvResult fvSemaphoreCreate(FvSemaphore *semaphore);

void fvSemaphoreDestroy(FvSemaphore semaphore);

typedef struct FvSwapchainCreateInfo {
    /** Format of each pixel in the image */
    FvFormat format;
    /** Dimensions of image */
    FvExtent3D extent;
    /** Number of layers in image */
    uint32_t arrayLayers;
    /** How the image will be used (bitmask of FvImageUsage) */
    FvImageUsage usage;
} FvSwapchainCreateInfo;

FV_DEFINE_HANDLE(FvSwapchain);

extern FvResult fvCreateSwapchain(FvSwapchain *swapchain,
                                  const FvSwapchainCreateInfo *createInfo);

extern void fvDestroySwapchain(FvSwapchain swapchain);

extern void fvGetSwapchainImage(FvSwapchain swapchain, FvImage *swapchainImage);

/**
 * Backs swapchain image with next drawable image - do this as late as possible.
 *
 * Function returns immediately, semaphore signals when image is actually
 * available.
 *
 * \param swapchain Swapchain to acquire drawable image from.
 * \param imageAvailableSemaphore Semaphore that will be signaled when image is
 * available.
 * \param [out] imageIndex Index of available image in swapchain.
 * \return FV_RESULT_SUCCESS if success, FV_RESULT_FAILURE otherwise.
 */
extern FvResult fvAcquireNextImage(FvSwapchain swapchain,
                                   FvSemaphore imageAvailableSemaphore,
                                   uint32_t *imageIndex);

/* FV_DEFINE_HANDLE(FvSemaphore); */

/* typedef struct FvSemaphoreCreateInfo { */

/* } FvSemaphoreCreateInfo; */

/* extern void fvCreateSemaphore(FvSemaphore *semaphore, */
/*                               const FvSemaphoreCreateInfo *createInfo); */

/* extern void fvAcquireNextImage(uint32_t *imageIndex); */

typedef struct FvSubmitInfo {
    /** Number of semaphores to wait on */
    uint32_t waitSemaphoreCount;
    /** Array of semaphores to wait on before executing command buffers in
     * submission */
    const FvSemaphore *waitSemaphores;
    /** Number of command buffers */
    uint32_t commandBufferCount;
    /** Command buffers to submit */
    const FvCommandBuffer *commandBuffers;
    /** Number of semaphores to be signaled once commands have completed
     * execution  */
    uint32_t signalSemaphoreCount;
    /** Array of semaphores to be signaled once commands have completed
     * execution */
    const FvSemaphore *signalSemaphores;

    /* /\** Array of bitmasked pipeline stages. Each entry corresponds to a wait
     */
    /*  * semaphore. Waiting will occur on each semaphore at the given stages of
     */
    /*  * the pipeline. *\/ */
    /* const FvPipelineStage *waitStagesMask; */
} FvSubmitInfo;

/**
 * Make a collection of submissions.
 */
extern FvResult fvQueueSubmit(uint32_t submissionsCount,
                              const FvSubmitInfo *submissions);

typedef struct FvPresentInfo {
    /** Number of semaphores to wait on before presentation */
    uint32_t waitSemaphoreCount;
    /** Array of semaphores to wait on before presentation */
    const FvSemaphore *waitSemaphores;
    uint32_t swapchainCount;
    FvSwapchain *swapchains;
    uint32_t *imageIndices;
} FvPresentInfo;

/**
 * Queue an image for presentation.
 */
extern void fvQueuePresent(const FvPresentInfo *presentInfo);

extern void fvDeviceWaitIdle();

FV_DEFINE_HANDLE(FvSurface);

#if FV_PLATFORM_OSX
typedef struct FvCocoaSurfaceCreateInfo {
    const void *view;
} FvCocoaSurfaceCreateInfo;

extern FvResult
fvCreateCocoaSurface(FvSurface *surface,
                     const FvCocoaSurfaceCreateInfo *createInfo);
#endif

extern void fvDestroySurface(FvSurface surface);

typedef struct FvInitInfo { FvSurface surface; } FvInitInfo;

extern FvResult fvInit(const FvInitInfo *initInfo);

extern void fvShutdown();
