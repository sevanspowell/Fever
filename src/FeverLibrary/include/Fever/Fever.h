/*===-- Fever/Fever.h - Fever Graphics Library API ------------------*- C -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * This file contains the API for the Fever Graphics Library
 *
 * How do I draw?
 * To draw something, you need to use the 'fvr_submit' call to submit a
 * RenderPass object, a RenderPipelineState object and a DrawCommand object.
 *
 * RenderPass
 * The RenderPass object is used to specify which color attachments, depth
 * attachment and stencil attachment you wish to store the results of a draw
 * into.
 *
 * RenderPipelineState
 * The RenderCommand object ... (as below?)
 *
 * DrawCommand
 * The DrawCommand object ...
 *
 * Buffers
 * Buffers come in three types: Vertex, Index and Constant buffers. Vertex
 * buffers are used for uploading vertex data to the GPU, index buffers are used
 * for describing in what order to access elements of the vertex buffer, and
 * constant buffers are used for uploading uniforms (essentially arbitrary data)
 * to the shader.
 *
 * Argument Tables
 * All 'at' arguments specify the position to bind that resource in one of the
 * argument tables. There are two argument tables; one for textures (the
 * 'Texture Argument Table') and one for buffers (the 'Buffer Argument Table').
 *
 * Render Pipeline State
 * The render pipeline state object holds all the state associated with a draw
 * call. You are required to describe the pixel format of each color attachment
 * you plan to use in a render pass. There are a fixed number of color
 * attachments given by FVR_MAX_COLOR_ATTACHMENTS. You are also required to
 * describe the depth and stencil attachment pixel formats.
 *
 * Setting all the pipeline state up in advance is cumbersome, and these
 * properties cannot be changed dynamically, but imposing this limitation allows
 * the implementations of this backend to optimize the rendering process.
 *
 * The following functions may be declared as macros.
//===----------------------------------------------------------------------===*/
#define FVR_MAX_COLOR_ATTACHMENTS 8

typedef void FvrBuffer;
typedef void FvrShader;
typedef void FvrTexture;
typedef void FvrRenderPipelineState;
typedef void FvrDrawCommand;

/** GPU Buffer types */
enum FvrBufferType {
    FvrBufferTypeVertex,
    FvrBufferTypeIndex,
    FvrBufferTypeConstant
};

/** Shader types */
enum FvrShaderType { FvrShaderTypeVertex, FvrShaderTypeFragment };

/** Texture types */
enum FvrTextureType {
    FvrTextureType1D,
    FvrTextureType2D,
    FvrTextureType3D,
    FvrTextureTypeCubeMap
};
enum FvrTextureUsage {
    FvrTextureUsageShaderRead,
    FvrTextureUsageShaderWrite,
    FvrTextureUsageRenderTarget
};
enum FvrTexturePixelFormat {};
enum FvrWindingOrder {
    FvrWindingOrderClockwise,
    FvrWindingOrderCounterClockwise
};
enum FvrCullMode {
    FvrCullModeNone,
    FvrCullModeFrontFacing,
    FvrCullModeBackFacing
};
enum FvrPrimitiveType {
    FvrPrimitiveTypeTriangle,
    FvrPrimitiveTypeLine,
};

FvrBuffer *const fvr_buffer_create();
void fvr_buffer_setData(FvrBuffer *const buffer, const void *const data,
                        size_t dataSize);
void fvr_buffer_setType(FvrBuffer *const buffer, FvrBufferType type);
void fvr_buffer_destroy(FvrBuffer *const buffer);

FvrShader *const fvr_shader_create();
void fvr_shader_setData(FvrShader *const shader, const void *const data,
                        size_t dataSize);
void fvr_shader_setType(FvrShader *const shader, FvrShaderType type);
void fvr_shader_destroy(FvrShader *const shader);

typedef struct FvrRegion { unsigned int originX; } FvrRegion;

FvrTexture *const fvr_texture_create();
void fvr_texture_setType(FvrTexture *const texture, FvrTextureType type);
void fvr_texture_setWidth(FvrTexture *const texture, unsigned int width);
void fvr_texture_setHeight(FvrTexture *const texture, unsigned int height);
void fvr_texture_setDepth(FvrTexture *const texture, unsigned int depth);
void fvr_texture_setPixelFormat(FvrTexture *const texture,
                                FvrTexturePixelFormat pixelFormat);
/* void fvr_texture_setData2D(FvrTexture *const texture, const void *const data,
 */
/*                            size_t dataSize); */
void fvr_texture_destroy(FvrTexture *const texture);

FvrRenderPipelineState *const fvr_renderPipelineState_create();
void fvr_renderPipelineState_setVertexBuffer(
    FvrRenderPipelineState *const renderPipelineState,
    const FvrBuffer *const buffer, size_t offset, uint32_t at);
void fvr_renderPipelineState_setWindingOrder(
    FvrRenderPipelineState *const renderPipelineState,
    FvrWindingOrder windingOrder);
void fvr_renderPipelineState_setCullMode(
    FvrRenderPipelineState *const renderPipelineState, FvrCullMode cullMode);
void fvr_renderPipelineState_setViewport(uint32_t originX, uint32_t originY,
                                         uint32_t width, uint32_t height);
void fvr_renderPipelineState_setTexture(
    FvrRenderPipelineState *const renderPipelineState,
    const FvrTexture *const texture, uint32_t at);
void fvr_renderPipelineState_setSamplerState(
    FvrRenderPipelineSate *const renderPipelineState,
    const FvrSamplerState *const samplerState);
void fvr_renderPipelineState_setVertexShader(
    FvrRenderPipelineState *const renderPipelineState,
    const FvrShader *const vertexShader);
void fvr_renderPipelineState_setFragmentShader(
    FvrRenderPipelineState *const renderPipelineState,
    const FvrShader *const fragmentShader);
void fvr_renderPipelineState_setPixelFormatColorAttachment(
    FvrRenderPipelineState *const renderPipelineState, uint32_t colorAttachment,
    FvrPixelFormat pixelFormat);
void fvr_renderPipelineState_setPixelFormatDepthAttachment(
    FvrRenderPipelineState *const renderPipelineState,
    FvrPixelFormat pixelFormat);
void fvr_renderPipelineState_setPixelFormatStencilAttachment(
    FvrRenderPipelineState *const renderPipelineState,
    FvrPixelFormat pixelFormat);
void fvr_renderPipelineState_destroy(
    FvrRenderPipelineState *const renderPipelineState);

FvrDrawCommand *const fvr_drawCommand_create();
void fvr_drawCommand_setDrawCallIndexedPrimitives(
    FvrDrawCommand *const drawCommand, FvrPrimitiveType primitiveType,
    size_t num, FvrIndexType indexType, const FvrBuffer *const indexBuffer,
    size_t indexBufferOffset);
void fvr_drawCommand_destroy(FvrDrawCommand *const drawCommand);

typedef struct FvrColor {
    FvrColor() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}

    float r;
    float g;
    float b;
    float a;
} FvrColor;

enum FvrAttachmentType {
    FvrAttachmentTypeColor,
    FvrAttachmentTypeDepth,
    FvrAttachmentTypeStencil,
};

typedef void FvrAttachmentDescriptor;
extern FvrAttachmentDescriptor *
fvr_attachmentDescriptor_create(FvrAttachmentType attachmentType);
fvr_attachmentDescriptor_setTexture(FvrTexture)
extern void
fvr_attachmentDescriptor_destroy(FvrAttachmentDescriptor *attachmentDescriptor);

class FvrRenderPassAttachmentDescriptor {};

class FvrRenderPassColorAttachmentDescriptor
    : public FvrRenderPassAttachmentDescriptor {
  public:
    FvrRenderPassColorAttachmentDescriptor() : clearColor(FvrColor()) {}

    FvrColor clearColor;
};

class FvrRenderPassDepthAttachmentDescriptor
    : public FvrRenderPassAttachmentDescriptor {
  public:
    FvrRenderPassDepthAttachmentDescriptor() : clearDepth(1.0f) {}

    float clearDepth;
};

class FvrRenderPassStencilAttachmentDescriptor
    : public FvrRenderPassAttachmentDescriptor {
  public:
    FvrRenderPassStencilAttachmentDescriptor() : clearStencil(0) {}

    uint32_t clearStencil;
};

FvrRenderPass *const fvr_renderPass_create();
/* Use this instead? */
/* FvrRenderPass *const */
/* fvr_renderPass_create(const FvrColorAttachment *const colorAttachments, */
/*                       int numColorAttachments, */
/*                       const FvrDepthAttachment *const depthAttachment, */
/*                       const FvrStencilAttachment *const stencilAttachment);
 */
void fvr_renderPass_colorAttachment_setClearColor(
    FvrRenderPass *const renderPass, uint32_t colorAttachment,
    FvrColor clearColor);
void fvr_renderPass_depthAttachment_setClearDepth(
    FvrRenderPass *const renderPass, float clearDepth);
void fvr_renderPass_stencilAttachment_setClearStencil(
    FvrRenderPass *const renderPass, uint32_t clearStencil);
void fvr_renderPass_destroy(FvrRenderPass *const renderPass);

/* Debug api
#ifdef DEBUG
#include <FeverDebugInternal.h>
#endif
*/
