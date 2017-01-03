/*===-- Fever/fever.h - Fever Graphics Library API ------------------*- C -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * This file contains the public API for the Fever Graphics Library.
 *
 * Buffers
 * A generic store for data to be placed on the graphics card.
 *
 * - data: data contained in the buffer
 * - datasize: size of the data in the buffer
 * - type: type of the buffer
 *
 * Buffer Layout Descriptors
 * Used to describe the data layout of a buffer.
 *
 * - stepFunction: How often new data is fetched from the buffer.
 * - stepRate: interval at which data is fetched from the buffer.
 * - stride: distance (in bytes) between data items in the buffer.
 //===----------------------------------------------------------------------===*/

#include "fever_private.h"

/* Colour */
typedef struct fvr_color_t { fvr_color_private_t private; } fvr_color_t;

/* Buffers */
typedef struct fvr_buffer_t { fvr_buffer_private_t private; } fvr_buffer_t;

extern void fvr_buffer_create(fvr_buffer_t *const buffer,
                              fvr_bufferType_t bufferType,
                              const void *const data, size_t dataSize);

extern void fvr_buffer_setData(fvr_buffer_t *const buffer,
                               const void *const data, size_t dataSize);
extern const void *fvr_buffer_getData(const fvr_buffer_t *const buffer);

extern void fvr_buffer_setDataSize(fvr_buffer_t *const buffer, size_t dataSize);
extern size_t fvr_buffer_getDataSize(const fvr_buffer_t *const buffer);

extern void fvr_buffer_setType(fvr_buffer_t *const buffer,
                               fvr_bufferType_t bufferType);
extern fvr_bufferType_t fvr_buffer_getType(const fvr_buffer_t *const buffer);

extern void fvr_buffer_destroy(fvr_buffer_t *const buffer);

/* Buffer Layout Descriptors */
typedef struct fvr_bufferLayoutDescriptor_t {
    fvr_bufferLayoutDescriptor_private_t private;
} fvr_bufferLayoutDescriptor_t;

extern void fvr_bufferLayoutDescriptor_create(
    fvr_bufferLayoutDescriptor_t *const bufferLayoutDescriptor);

extern void fvr_bufferLayoutDescriptor_setstepFunction(
    fvr_bufferLayoutDescriptor_t *const bufferLayoutDescriptor,
    fvr_vertexStepFunction_t stepFunction);
extern fvr_vertexStepFunction_t fvr_bufferLayoutDescriptor_getStepFunction(
    const fvr_bufferLayoutDescriptor_t *const bufferLayoutDescriptor);

extern void fvr_bufferLayoutDescriptor_setsteprate(
    fvr_bufferLayoutDescriptor_t *const bufferLayoutDescriptor,
    unsigned int stepRate);
extern unsigned int fvr_bufferLayoutDescriptor_getStepRate(
    const fvr_bufferLayoutDescriptor_t *const bufferLayoutDescriptor);

extern void fvr_bufferLayoutDescriptor_setStride(
    fvr_bufferLayoutDescriptor_t *const bufferLayoutDescriptor,
    unsigned int stride);
extern unsigned int fvr_bufferLayoutDescriptor_getStride(
    const fvr_bufferLayoutDescriptor_t *const bufferLayoutDescriptor);

extern void fvr_bufferLayoutDescriptor_destroy(
    fvr_bufferLayoutDescriptor_t *const bufferLayoutDescriptor);

/* Shaders */
typedef struct fvr_shader_t { fvr_shader_private_t private; } fvr_shader_t;

/* Maybe use descriptor to create shader object, instead of making user use
 * gets/sets
 * e.g. struct shaderDescriptor { shaderType, data, datasize }*/
extern void fvr_shader_create(fvr_shader_t *const shader,
                              fvr_shaderType_t shaderType,
                              const void *const data, size_t dataSize);

extern void fvr_shader_setData(fvr_shader_t *const shader,
                               const void *const data, size_t dataSize);
extern const void *fvr_shader_getData(const fvr_shader_t *const shader);

extern void fvr_shader_setDataSize(fvr_shader_t *const shader, size_t dataSize);
extern size_t fvr_shader_getDataSize(const fvr_shader_t *const shader);

extern void fvr_shader_setType(fvr_shader_t *const shader,
                               fvr_shaderType_t shaderType);
extern fvr_shaderType_t fvr_shader_getType(const fvr_shader_t *const shader);

extern void fvr_shader_destroy(fvr_shader_t *const shader);

/* Texture Descriptors */

typedef struct fvr_textureDescriptor_t {
    fvr_textureDescriptor_private_t private;
} fvr_textureDescriptor_t;

extern void
fvr_textureDescriptor_create(fvr_textureDescriptor_t *const textureDescriptor);

extern void
fvr_textureDescriptor_setType(fvr_textureDescriptor_t *const textureDescriptor,
                              fvr_textureType_t textureType);
extern fvr_textureType_t fvr_textureDescriptor_getType(
    const fvr_textureDescriptor_t *const textureDescriptor);

extern void fvr_textureDescriptor_setFormat(
    fvr_textureDescriptor_t *const textureDescriptor,
    fvr_textureFormat_t format);
extern fvr_textureFormat_t fvr_textureDescriptor_getFormat(
    const fvr_textureDescriptor_t *const textureDescriptor);

extern void
fvr_textureDescriptor_setUsage(fvr_textureDescriptor_t *const textureDescriptor,
                               fvr_textureUsage_t usage);
extern fvr_textureUsage_t fvr_textureDescriptor_getUsage(
    const fvr_textureDescriptor_t *const textureDescriptor);

extern void
fvr_textureDescriptor_setWidth(fvr_textureDescriptor_t *const textureDescriptor,
                               unsigned int width);
extern unsigned int fvr_textureDescriptor_getWidth(
    const fvr_textureDescriptor_t *const textureDescriptor);

extern void fvr_textureDescriptor_setHeight(
    fvr_textureDescriptor_t *const textureDescriptor, unsigned int height);
extern unsigned int fvr_textureDescriptor_getHeight(
    const fvr_textureDescriptor_t *const textureDescriptor);

extern void
fvr_textureDescriptor_setDepth(fvr_textureDescriptor_t *const textureDescriptor,
                               unsigned int depth);
extern unsigned int fvr_textureDescriptor_getDepth(
    const fvr_textureDescriptor_t *const textureDescriptor);

extern void fvr_textureDescriptor_setNumMipmaps(
    fvr_textureDescriptor_t *const textureDescriptor, unsigned int numMipmaps);
extern unsigned int fvr_textureDescriptor_getNumMipmaps(
    const fvr_textureDescriptor_t *const textureDescriptor);

extern void
fvr_textureDescriptor_destroy(fvr_textureDescriptor_t *const textureDescriptor);

/* Textures */
typedef struct fvr_texture_t { fvr_texture_private_t private; } fvr_texture_t;

extern void
fvr_texture_create(fvr_texture_t *const texture,
                   const fvr_textureDescriptor_t *const textureDescriptor);

extern void fvr_texture_setType(fvr_texture_t *const texture,
                                fvr_textureType_t textureType);
extern fvr_textureType_t
fvr_texture_getType(const fvr_texture_t *const texture);

extern void fvr_texture_setFormat(fvr_texture_t *const texture,
                                  fvr_textureFormat_t format);
extern fvr_textureFormat_t
fvr_texture_getFormat(const fvr_texture_t *const texture);

extern void fvr_texture_setUsage(fvr_texture_t *const texture,
                                 fvr_textureUsage_t usage);
extern fvr_textureUsage_t
fvr_texture_getUsage(const fvr_texture_t *const texture);

extern void fvr_texture_setWidth(fvr_texture_t *const texture,
                                 unsigned int width);
extern unsigned int fvr_texture_getWidth(const fvr_texture_t *const texture);

extern void fvr_texture_setHeight(fvr_texture_t *const texture,
                                  unsigned int height);
extern unsigned int fvr_texture_getHeight(const fvr_texture_t *const texture);

extern void fvr_texture_setDepth(fvr_texture_t *const texture,
                                 unsigned int depth);
extern unsigned int fvr_texture_getDepth(const fvr_texture_t *const texture);

extern void fvr_texture_setNumMipmaps(fvr_texture_t *const texture,
                                      unsigned int numMipmaps);
extern unsigned int
fvr_texture_getNumMipmaps(const fvr_texture_t *const texture);

extern void fvr_texture_destroy(fvr_texture_t *const texture);

/* Render Pipeline Descriptor */

typedef struct fvr_renderPipelineDescriptor_t {
    fvr_renderPipelineDescriptor_private_t private;
} fvr_renderPipelineDescriptor_t;

extern void fvr_renderPipelineDescriptor_create(
    fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor);

extern void fvr_renderPipelineDescriptor_setVertexShader(
    fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor,
    const fvr_shader_t *const vertexShader);
extern const fvr_shader_t *fvr_renderPipelineDescriptor_getVertexShader(
    const fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor);

extern void fvr_renderPipelineDescriptor_setFragmentShader(
    fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor,
    const fvr_shader_t *const fragmentShader);
extern const fvr_shader_t *fvr_renderPipelineDescriptor_getFragmentShader(
    const fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor);

extern void fvr_renderPipelineDescriptor_setColorAttachmentTextureFormat(
    fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor,
    unsigned int index, fvr_textureFormat_t textureFormat);
extern fvr_textureFormat_t
fvr_renderPipelineDescriptor_getColorAttachmentTextureFormat(
    fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor,
    unsigned int index);

extern void fvr_renderPipelineDescriptor_setDepthAttachmentTextureFormat(
    fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor,
    fvr_textureFormat_t textureFormat);
extern fvr_textureFormat_t
fvr_renderPipelineDescriptor_getDepthAttachmentTextureFormat(
    const fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor);

extern void fvr_renderPipelineDescriptor_setStencilAttachmentTextureFormat(
    fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor,
    fvr_textureFormat_t textureFormat);
extern fvr_textureFormat_t
fvr_renderPipelineDescriptor_getStencilAttachmentTextureFormat(
    const fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor);

extern void fvr_renderPipelineDescriptor_destroy(
    fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor);

/* Render Pipeline State */
typedef struct fvr_renderPipelineState_t {
    fvr_renderPipelineState_private_t private;
} fvr_renderPipelineState_t;

extern void fvr_renderPipelineState_create(
    fvr_renderPipelineState_t *const renderPipelineState,
    const fvr_renderPipelineDescriptor_t *const renderPipelineDescriptor);

extern void fvr_renderPipelineState_destroy(
    fvr_renderPipelineState_t *const renderPipelineState);

/* Render Pass Descriptor */
typedef struct fvr_renderPassDescriptor_t {
    fvr_renderPassDescriptor_private_t private;
} fvr_renderPassDescriptor_t;

extern void fvr_renderPassDescriptor_create(
    fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setColorAttachmentTexture(
    fvr_renderPassDescriptor_t *const renderPassDescriptor, unsigned int index,
    const fvr_texture_t *const texture);
extern const fvr_texture_t *fvr_renderPassDescriptor_getColorAttachmentTexture(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor,
    unsigned int index);

extern void fvr_renderPassDescriptor_setColorAttachmentMipmapLevel(
    fvr_renderPassDescriptor_t *const renderPassDescriptor, unsigned int index,
    int mipmapLevel);
extern int fvr_renderPassDescriptor_getColorAttachmentMipmapLevel(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor,
    unsigned int index);

extern void fvr_renderPassDescriptor_setColorAttachmentLoadAction(
    fvr_renderPassDescriptor_t *const renderPassDescriptor, unsigned int index,
    fvr_loadAction_t loadAction);
extern fvr_loadAction_t fvr_renderPassDescriptor_getColorAttachmentLoadAction(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor,
    unsigned int index);

extern void fvr_renderPassDescriptor_setColorAttachmentStoreAction(
    fvr_renderPassDescriptor_t *const renderPassDescriptor, unsigned int index,
    fvr_storeAction_t storeAction);
extern fvr_storeAction_t fvr_renderPassDescriptor_getColorAttachmentStoreAction(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor,
    unsigned int index);

extern void fvr_renderPassDescriptor_setColorAttachmentClearColor(
    fvr_renderPassDescriptor_t *const renderPassDescriptor, unsigned int index,
    const fvr_color_t *const clearColor);
extern const fvr_color_t *fvr_renderPassDescriptor_getColorAttachmentClearColor(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor,
    unsigned int index);

extern void fvr_renderPassDescriptor_setDepthAttachmentTexture(
    fvr_renderPassDescriptor_t *const renderPassDescriptor,
    const fvr_texture_t *const texture);
extern const fvr_texture_t *fvr_renderPassDescriptor_getDepthAttachmentTexture(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setDepthAttachmentMipmapLevel(
    fvr_renderPassDescriptor_t *const renderPassDescriptor, int mipmapLevel);
extern int fvr_renderPassDescriptor_getDepthAttachmentMipmapLevel(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setDepthAttachmentLoadAction(
    fvr_renderPassDescriptor_t *const renderPassDescriptor,
    fvr_loadAction_t loadAction);
extern fvr_loadAction_t fvr_renderPassDescriptor_getDepthAttachmentLoadAction(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setDepthAttachmentStoreAction(
    fvr_renderPassDescriptor_t *const renderPassDescriptor,
    fvr_storeAction_t storeAction);
extern fvr_storeAction_t fvr_renderPassDescriptor_getDepthAttachmentStoreAction(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setDepthAttachmentClearDepth(
    fvr_renderPassDescriptor_t *const renderPassDescriptor, float clearDepth);
extern float fvr_renderPassDescriptor_getDepthAttachmentClearDepth(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setStencilAttachmentTexture(
    fvr_renderPassDescriptor_t *const renderPassDescriptor,
    const fvr_texture_t *const texture);
extern const fvr_texture_t *
fvr_renderPassDescriptor_getStencilAttachmentTexture(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setStencilAttachmentMipmapLevel(
    fvr_renderPassDescriptor_t *const renderPassDescriptor, int mipmapLevel);
extern int fvr_renderPassDescriptor_getStencilAttachmentMipmapLevel(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setStencilAttachmentLoadAction(
    fvr_renderPassDescriptor_t *const renderPassDescriptor,
    fvr_loadAction_t loadAction);
extern fvr_loadAction_t fvr_renderPassDescriptor_getStencilAttachmentLoadAction(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setStencilAttachmentStoreAction(
    fvr_renderPassDescriptor_t *const renderPassDescriptor,
    fvr_storeAction_t storeAction);
extern fvr_storeAction_t
fvr_renderPassDescriptor_getStencilAttachmentStoreAction(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_setStencilAttachmentClearStencil(
    fvr_renderPassDescriptor_t *const renderPassDescriptor,
    unsigned int clearStencil);
extern unsigned int fvr_renderPassDescriptor_getStencilAttachmentClearStencil(
    const fvr_renderPassDescriptor_t *const renderPassDescriptor);

extern void fvr_renderPassDescriptor_destroy(
    fvr_renderPassDescriptor_t *const renderPassDescriptor);
