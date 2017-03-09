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
    /** Stage of the render pipeline this shader should be used for. */
    FvShaderStage stage;
    /** Shader data */
    const void *data;
    /** Size of the shader data in bytes. */
    size_t size;
    /** Name of the function that is the entry point for the shader. */
    const char *entryFunctionName;
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
    FvTextureFormat format;
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
