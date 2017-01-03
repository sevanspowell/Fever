/*===-- Fever/fever_private.h - Fever Graphics Library Internals ----*- C -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 *
 * These structures are for internal use only and should never be used by code
 * interfacing with the Fever API.
 //===---------------------------------------------------------------------===*/
#include <stddef.h>

#include "fever_constants.h"

typedef struct fvr_color_private_t {
    float red;
    float green;
    float blue;
    float alpha;
} fvr_color_private_t;

typedef struct fvr_buffer_private_t {
    fvr_bufferType_t type;
    void *data;
    size_t size;
} fvr_buffer_private_t;

typedef struct fvr_bufferLayoutDescriptor_private_t {
    fvr_vertexStepFunction_t stepFunction;
    unsigned int stepRate;
    unsigned int stride;
} fvr_bufferLayoutDescriptor_private_t;

typedef struct fvr_shader_private_t {
    fvr_shaderType_t type;
    void *data;
    size_t size;
} fvr_shader_private_t;

typedef struct fvr_texture_private_t {
    fvr_textureType_t type;
    fvr_textureFormat_t format;
    fvr_textureUsage_t usage;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    unsigned int numMipmaps;
} fvr_texture_private_t;

typedef struct fvr_textureDescriptor_private_t {
    fvr_textureType_t type;
    fvr_textureFormat_t format;
    fvr_textureUsage_t usage;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    unsigned int numMipmaps;
} fvr_textureDescriptor_private_t;

typedef struct fvr_renderPipelineDescriptor_private_t {
    fvr_shader_private_t vertexShader;
    fvr_shader_private_t fragmentShader;
    fvr_textureFormat_t
        colorAttachmentTextureFormats[FVR_MAX_COLOR_ATTACHMENTS];
    fvr_textureFormat_t depthAttachmentTextureFormat;
    fvr_textureFormat_t stencilAttachmentTextureFormat;
} fvr_renderPipelineDescriptor_private_t;

typedef struct fvr_renderPipelineState_private_t {
    int placeholder;
} fvr_renderPipelineState_private_t;

typedef struct fvr_renderPassAttachmentDescriptor_private_t {
    fvr_texture_private_t texture;
    int mipmapLevel;
    fvr_loadAction_t loadAction;
    fvr_storeAction_t storeAction;
} fvr_renderPassAttachmentDescriptor_private_t;

typedef struct fvr_colorAttachmentDescriptor_private_t {
    fvr_texture_private_t texture;
    int mipmapLevel;
    fvr_loadAction_t loadAction;
    fvr_storeAction_t storeAction;
    fvr_color_private_t clearColour;
} fvr_colorAttachmentDescriptor_private_t;

typedef struct fvr_depthAttachmentDescriptor_private_t {
    fvr_texture_private_t texture;
    int mipmapLevel;
    fvr_loadAction_t loadAction;
    fvr_storeAction_t storeAction;
    float clearDepth;
} fvr_depthAttachmentDescriptor_private_t;

typedef struct fvr_stencilAttachmentDescriptor_private_t {
    fvr_texture_private_t texture;
    int mipmapLevel;
    fvr_loadAction_t loadAction;
    fvr_storeAction_t storeAction;
    unsigned int clearStencil;
} fvr_stencilAttachmentDescriptor_private_t;

typedef struct fvr_renderPassDescriptor_private_t {
    fvr_colorAttachmentDescriptor_private_t colorAttachments[FVR_MAX_COLOR_ATTACHMENTS];
    fvr_depthAttachmentDescriptor_private_t depthAttachment;
    fvr_stencilAttachmentDescriptor_private_t stencilAttachment;
} fvr_renderPassDescriptor_private_t;
