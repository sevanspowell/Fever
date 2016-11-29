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
typedef void fvrbuffer_t;
typedef void fvrshader_t;
typedef void fvrtexture_t;
typedef void fvrpipelinestatedescriptor_t;
typedef void fvrregion_t;
typedef void fvrsamplerstate_t;
typedef void fvrrenderattachment_t;
typedef void fvrrenderpass_t;

typedef enum fvrbuffertype_t {
    FVR_BUFFER_TYPE_VERTEX,
    FVR_BUFFER_TYPE_INDEX,
    FVR_BUFFER_TYPE_CONSTANT
} fvrbuffertype_t;

typedef enum fvrshadertype_t {
    FVR_SHADER_TYPE_VERTEX,
    FVR_SHADER_TYPE_FRAGMENT
} fvrshadertype_t;

typedef enum fvrwindingorder_t {
    FVR_WINDING_ORDER_CLOCKWISE,
    FVR_WINDING_ORDER_COUNTER_CLOCKWISE
} fvrwindingorder_t;

typedef enum fvrcullmode_t {
    FVR_CULL_MODE_NONE,
    FVR_CULL_MODE_FRONT_FACING,
    FVR_CULL_MODE_BACK_FACING
} fvrcullmode_t;

typedef enum fvrprimitivetype_t {
    FVR_PRIMITIVE_TYPE_TRIANGLE,
    FVR_PRIMITIVE_TYPE_LINE
} fvrprimitivetype_t;

typedef enum fvrsampleraddressmode_t {
    FVR_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    FVR_SAMPLER_ADDRESS_MODE_REPEAT,
    FVR_SAMPLER_ADDRESS_MODE_MIRROR_REPEAT,
    FVR_SAMPLER_ADDRESS_MODE_CLAMP_TO_ZERO
} fvrsampleraddressmode_t;

typedef enum fvrsamplerminmagfilter_t {
    FVR_SAMPLER_MIN_MAG_FILTER_NEAREST,
    FVR_SAMPLER_MIN_MAG_FILTER_LINEAR
} fvrsamplerminmagfilter_t;

typedef enum fvrrenderattachmenttype_t {
    FVR_RENDER_ATTACHMENT_TYPE_COLOR,
    FVR_RENDER_ATTACHMENT_TYPE_DEPTH,
    FVR_RENDER_ATTACHMENT_TYPE_STENCIL
} fvrrenderttachmenttype_t;

/** Depth texture comparison function. A fragment that passes the test is
 * stored, a fragment that fails the test is discarded */
typedef enum fvrcomparefunction_t {
    /** Newer value never passes */
    FVR_COMPARE_FUNCTION_NEVER,
    /** Newer value passes if it is less than existing*/
    FVR_COMPARE_FUNCTION_LESS,
    /** Newer value passes if it is equal to existing */
    FVR_COMPARE_FUNCTION_EQUAL,
    /** Newer value passes if it is less than or equal to existing */
    FVR_COMPARE_FUNCTION_LESS_EQUAL,
    /** Newer value passes if it is greater than existing */
    FVR_COMPARE_FUNCTION_GREATER,
    /** Newer value passes if it is not equal to the existing */
    FVR_COMPARE_FUNCTION_NOT_EQUAL,
    /** Newer value passes if it is greater than or equal to existing */
    FVR_COMPARE_FUNCTION_GREATER_EQUAL,
    /** Newer value always passes the comparison test */
    FVR_COMPARE_FUNCTION_ALWAYS,
} fvrcomparefunction_t;

typedef enum fvrpixelformat_t {} fvrpixelformat_t;

typedef enum fvrloadaction_t {
    FVR_LOAD_ACTION_DONT_CARE,
    FVR_LOAD_ACTION_LOAD,
    FVR_LOAD_ACTION_CLEAR
} fvrloadaction_t;

typedef enum fvrstoreaction_t {
    FVR_STORE_ACTION_DONT_CARE,
    FVR_STORE_ACTION_STORE,
    FVR_STORE_ACTION_MULTISAMPLE_RESOLVE,
    FVR_STORE_ACTION_STORE_AND_MULTISAMPLE_RESOLVE,
    FVR_STORE_ACTION_UNKNOWN
} fvrstoreaction_t;

typedef enum fvrtexturetype_t {
    FVR_TEXTURE_TYPE_1D,
    FVR_TEXTURE_TYPE_2D,
    FVR_TEXTURE_TYPE_3D,
    FVR_TEXTURE_TYPE_CUBEMAP,
} fvrtexturetype_t;

typedef enum fvrindextype_t { FVR_INDEX_TYPE_UINT16 } fvrindextype_t;

typedef struct fvrcolor_t {
    float red;
    float green;
    float blue;
    float alpha;
} fvrcolor_t;

typedef struct fvrregion_t {
    float originx;
    float originy;
    float originz;
    float width;
    float height;
    float depth;
} fvrregion_t;

typedef struct fvrdrawitem_t {
    fvrrenderpass_t *renderpass;
    fvrpipelinestate_t *pipelinestate;
    fvrdrawcommand_t *drawcommand;
} fvrdrawitem_t;

/** All fields are compulsory */
typedef struct fvrtexturereplacerequest_t {
    fvrregion_t region;
    unsigned int mipmaplevel;
    const void *const data;
    size_t datasize;
};

/* Colors */
extern fvrcolor_t fvr_color_getcolor(float red, float green, float blue,
                                     float alpha);

/* Regions */
extern fvrregion_t fvr_region_get2dregion(float originX, float originY,
                                          float width, float height);

/* Buffers */
extern fvrbuffer_t *const fvr_buffer_create(fvrbuffertype_t buffertype);
extern void fvr_buffer_setdata(fvrbuffer_t *const buffer,
                               const void *const data, size_t datasize);
extern void fvr_buffer_destroy(fvrbuffer_t *const buffer);

/* Shaders */
extern fvrshader_t *const fvr_shader_create(fvrshadertype_t shadertype);
extern void fvr_shader_setdata(fvrshadertype_t *const shader,
                               const void *const data, size_t datasize);
extern void fvr_shader_destroy(fvrshader_t *const shader);

/* Textures */
extern fvrtexture_t *const fvr_texture_create(fvrtexturetype_t texturetype);
extern void fvr_texture_setwidth(fvrtexture_t *const texture,
                                 unsigned int width);
extern void fvr_texture_setheight(fvrtexture_t *const texture,
                                  unsigned int height);
extern void fvr_texture_setpixelformat(fvrtexture_t *const texture,
                                       fvrpixelformat_t pixelformat);
extern void fvr_texture_replacedata(fvrtexturereplacerequest_t replacerequest);
extern void fvr_texture_destroy(fvrtexture_t *const texture);

/* Pipeline states */
extern fvrpipelinestate_t *const fvr_pipelinestate_create(void);
extern void
fvr_pipelinestate_setvertexbuffer(fvrpipelinestate_t *const pipelinestate,
                                  const fvrbuffer_t buffer, size_t offset,
                                  unsigned int at);
extern void
fvr_pipelinestate_setwindingorder(fvrpipelinestate_t *const pipelinestate,
                                  fvrwindingorder_t windingorder);
extern void
fvr_pipelinestate_setcullmode(fvrpipelinestate_t *const pipelinestate,
                              fvrcullmode_t cullmode);
extern void fvr_pipelinestate_setviewportregion(fvrregion_t region);
extern void
fvr_pipelinestate_settexture(fvrpipelinestate_t *const pipelinestate,
                             const fvrtexture_t *const texture,
                             unsigned int at);
extern void
fvr_pipelinestate_setsamplerstate(fvrpipelinestate_t *const pipelinestate,
                                  const fvrsamplerstate_t *const samplerstate);
extern void
fvr_pipelinestate_setvertexshader(fvrpipelinestate_t *const pipelinestate,
                                  const fvrshader_t *const vertexshader);
extern void
fvr_pipelinestate_setfragmentshader(fvrpipelinestate_t *const pipelinestate,
                                    const fvrshader_t *const fragmentshader);
extern void fvr_pipelinestate_setpixelformatcolorattachment(
    fvrpipelinestate_t *const pipelinestate, unsigned int colorattachmentindex,
    fvrpixelformat_t pixelformat);
extern void fvr_pipelinestate_setpixelformatdepthattachment(
    fvrpipelinestate_t *const pipelinestate, fvrpixelformat_t pixelformat);
extern void fvr_pipelinestate_setpixelformatstencilattachment(
    fvrpipelinestate_t *const pipelinestate, fvrpixelformat_t pixelformat);
extern void fvr_pipelinestate_destroy(fvrpipelinestate_t *const pipelinestate);

/* Render attachments */
extern fvrrenderattachment_t *const
fvr_renderattachment_create(fvrenderattachmenttype_t renderattachmenttype);
extern void
fvr_renderattachment_settexture(fvrrenderattachment_t *const renderattachment,
                                const fvrtexture_t *const texture);
extern void fvr_renderattachment_setmipmaplevel(
    fvrrenderattachment_t *const renderattachment, unsigned int mipmaplevel);
extern void fvr_renderattachment_setloadaction(
    fvrrenderattachment_t *const renderattachment, fvrloadaction_t loadaction);
extern void fvr_renderattachment_setstoreaction(
    fvrrenderattachment_t *const renderattachment,
    fvrstoreaction_t storeaction);
extern void fvr_renderattachment_setclearcolor(
    fvrrenderattachment_t *const renderattachment, fvrcolor_t clearcolor);
extern void fvr_renderattachment_setcleardepth(
    fvrrenderattachment_t *const renderattachment, float cleardepth);
extern void fvr_renderattachment_setclearstencil(
    fvrrenderattachment_t *const renderattachment, unsigned int clearstencil);
extern void
fvr_renderattachment_destroy(fvrrenderattachment_t *const renderattachment);

/* Render passes */
extern fvrrenderpass_t *fvr_renderpass_create();
extern void fvr_renderpass_setcolorattachment(
    fvrrenderpass_t *const renderpass, unsigned int colorattachmentindex,
    const fvrrenderattachment_t *const colorattachment);
extern void
fvr_renderpass_setdepthattachment(fvrrenderpass_t *const renderpass,
                                  const fvrrenderattachment_t depthattachment);
extern void fvr_renderpass_setstencilattachment(
    fvrrenderpass_t *const renderpass,
    const fvrrenderattachment_t stencilattachment);
extern void fvr_renderpass_destroy(fvrrenderpass_t *const renderpass);

/* Draw commands */
extern fvrdrawitem_t *fvr_drawitem_create();
extern void fvr_drawitem_setdrawcommandindexedprimitives(
    fvrdrawitem_t *const drawitem, fvrprimitivetype_t primitivetype,
    unsigned int numprimitives, fvrbuffer_t indexbuffer,
    fvrindextype_t indextype, size_t offset);
extern void
fvr_drawitem_setpipelinestate(fvrdrawitem_t *const drawitem,
                              const fvrpipelinestate_t *const pipelinestate);
extern void fvr_drawitem_destroy(fvrdrawitem_t *const drawitem);

/* Fever functions */
extern bool fvr_init();
extern void fvr_shutdown();
extern void fvr_submit(const fvrrenderpass_t *const renderpass,
                       const fvrdrawitem_t drawitems[]);
