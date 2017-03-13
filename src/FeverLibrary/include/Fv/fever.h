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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "fever_constants.h"

#define FV_DEFINE_HANDLE(objectToHandle) typedef uint32_t objectToHandle;

typedef union FvClearColor {
    float float32[4];
    int32_t int32[4];
    uint32_t uint32[4];
} FvClearColor;

typedef struct FvClearDepthStencil {
    float depth;
    uint32_t stencil;
} FvClearDepthStencil;

typedef union FvClearData {
    FvClearColor color;
    FvClearDepthStencil depthStencil;
} FvClearData;

/** Structure specifying creation parameters for a buffer. */
typedef struct FvBufferCreateInfo {
    FvBufferType type; /** Type of the buffer. */
    const void *data;  /** Buffer data. */
    size_t size;       /** Size of the buffer data in bytes. */
} FvBufferCreateInfo;

/** Opaque handle to buffer object. */
FV_DEFINE_HANDLE(FvBuffer);

extern void fvBufferCreate(FvBuffer *buffer,
                           const FvBufferCreateInfo *createInfo);

extern void fvBufferDestroy(FvBuffer buffer);

/** Structure specifying creation parameters for a shader. */
typedef struct FvShaderCreateInfo {
    /** Shader data */
    const void *data;
    /** Size of the shader data in bytes. */
    size_t size;
} FvShaderCreateInfo;

/** Opaque handle to shader object. */
FV_DEFINE_HANDLE(FvShader);

extern void fvShaderCreate(FvShader *shader,
                           const FvShaderCreateInfo *createInfo);

extern void fvShaderDestroy(FvShader shader);

/** Opaque handle to texture object. */
FV_DEFINE_HANDLE(FvTexture);

/** Structure specifying creation parameters for a texture. */
typedef struct FvTextureCreateInfo {
    /** Format of each pixel in the texture */
    FvPixelFormat format;
    /** Width of the texture */
    uint32_t width;
    /** Height of the texture */
    uint32_t height;
    /** Depth of the texture */
    uint32_t depth;
    /** Number of mipmap levels */
    uint32_t numMipmapLevels;
    /** Number of samples in each pixel */
    uint32_t numSamples;
    /** How the texture will be used */
    FvTextureUsage usage;
} FvTextureCreateInfo;

extern void fvTextureCreate(FvTexture *texture,
                            const FvTextureCreateInfo *createInfo);

extern void fvTextureDestroy(FvTexture texture);

/** Opaque handle to depth stencil state object. */
FV_DEFINE_HANDLE(FvDepthStencilState);

typedef struct FvStencilDescriptor {
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
} FvStencilDescriptor;

typedef struct FvDepthStencilStateCreateInfo {
    /** Comparison function used to compare fragment's depth value and depth
     * value in the attachment, deciding whether or not to discard the fragment.
     */
    FvCompareFunc depthCompareFunc;
    /** True if depth writing to attachment is enabled, false otherwise. */
    bool enableDepthWrite;
    /** Stencil descriptor for back-facing primitives. */
    FvStencilDescriptor backFaceStencil;
    /** Stencil descriptor for front-facing primitives. */
    FvStencilDescriptor frontFaceStencil;
} FvDepthStencilStateCreateInfo;

void fvDepthStencilStateCreate(FvDepthStencilState *depthStencilState,
                               const FvDepthStencilStateCreateInfo *createInfo);

void fvDepthStencilStateDestroy(FvDepthStencilState depthStencilState);

/* typedef union FvAttachmentDescriptor { */
/*     FvLoadOp loadOp; */
/*     FvStoreOp storeOp; */
/* } FvAttachmentDescriptor; */

typedef struct FvColorAttachmentDescriptor {
    /** Format of color attachment's texture */
    FvPixelFormat format;
    /** True if blending is enabled, false otherwise. If disabled, source
     * fragment's color for this attachment is not modified. */
    bool enableBlending;
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
    /** Bitmask which restricts the color components that may be written to */
    FvColorComponentFlags colorWriteMask;
} FvColorAttachmentDescriptor;

typedef struct FvShaderStageDescriptor {
    /** Stage of the shader pipeline */
    FvShaderStage stage;
    /** Name of function that is the entry point for this shader stage */
    const char *entryFunctionName;
    /** Shader code bundle to use */
    FvShader shader;
} FvShaderStageDescriptor;

/** The 'interface' for a render pipeline */
typedef struct FvRenderPipelineDescriptor {
    /** Array of color attachments */
    const FvColorAttachmentDescriptor *colorAttachmentDescriptors;
    /** Depth attachment pixel format */
    FvPixelFormat depthAttachmentFormat;
    /** Stencil attachment pixel format */
    FvPixelFormat stencilAttachmentFormat;
    /** For multisampling - number of samples for each fragment */
    uint32_t numSamples;
    /** Information regarding the programmable parts of the graphics pipeline */
    const FvShaderStageDescriptor *shaderStages;
} FvRenderPipelineDescriptor;

typedef struct FvRenderPassColorAttachment {
    /** Color to clear this attachment to */
    FvClearColor clearColor;
    /** Texture object associated with this attachment */
    FvTexture *texture;
    /** Operation to perform on this attachment at start of rendering pass */
    FvLoadOp loadOp;
    /** Operation to perform on this attachment at end of rendering pass */
    FvStoreOp storeOp;
} FvRenderPassColorAttachment;

typedef struct FvRenderPassDepthAttachment {
    /** Depth value to clear this attachment to */
    float clearDepth;
    /** Texture object associated with this attachment */
    FvTexture *texture;
    /** Operation to perform on this attachment at start of rendering pass */
    FvLoadOp loadOp;
    /** Operation to perform on this attachment at end of rendering pass */
    FvStoreOp storeOp;
} FvRenderPassDepthAttachment;

typedef struct FvRenderPassStencilAttachment {
    /** Stencil value to clear this attachment to */
    uint32_t clearDepth;
    /** Texture object associated with this attachment */
    FvTexture *texture;
    /** Operation to perform on this attachment at start of rendering pass */
    FvLoadOp loadOp;
    /** Operation to perform on this attachment at end of rendering pass */
    FvStoreOp storeOp;
} FvRenderPassStencilAttachment;

/** What a render pipeline operates on */
/** Render attachment descriptor ??? */
typedef struct FvRenderPassDescriptor {
    /** Array of color attachments */
    const FvRenderPassColorAttachment **colorAttachments;
    /** Depth attachment */
    const FvRenderPassDepthAttachment *depthAttachment;
    /** Stencil attachment */
    const FvRenderPassStencilAttachment *stencilAttachment;
} FvRenderPassDescriptor;
