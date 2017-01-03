/*===-- Fever/fever_constants.h - Fever Graphics Library Constants --*- C -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 *
 //===---------------------------------------------------------------------===*/
#define FVR_MAX_COLOR_ATTACHMENTS 8

typedef enum fvr_bufferType_t {
    FVR_BUFFER_TYPE_VERTEX,  /** Vertex buffer */
    FVR_BUFFER_TYPE_INDEX,   /** Index buffer */
    FVR_BUFFER_TYPE_CONSTANT /** Constant buffer */
} fvr_bufferType_t;

typedef enum fvr_vertexStepFunction_t {
    FVR_VERTEX_STEP_FUNCTION_CONSTANT,   /** Attribute data is fetched once. */
    FVR_VERTEX_STEP_FUNCTION_PER_VERTEX, /** Attribute data is fetched for every
                                            vertex */
    FVR_VERTEX_STEP_FUNCTION_PER_INSTANCE /** Attribute data is fetched for a
                                             number of instances that is
                                             determined by the step rate */
} fvr_vertexStepFunction_t;

typedef enum fvr_shaderType_t {
    FVR_SHADER_TYPE_VERTEX,  /** Vertex shader */
    FVR_SHADER_TYPE_FRAGMENT /** Fragment shader */
} fvr_shaderType_t;

typedef enum fvr_textureType_t {
    FVR_TEXTURE_TYPE_1D,
    FVR_TEXTURE_TYPE_2D,
    FVR_TEXTURE_TYPE_3D,
    FVR_TEXTURE_TYPE_CUBEMAP,
} fvr_textureType_t;

typedef enum fvr_textureFormat_t {
    FVR_TEXTURE_FORMAT_RGBA8_UNORM
} fvr_textureFormat_t;

/** Can be OR'd together */
typedef enum fvr_textureUsage_t {
    FVR_TEXTURE_USAGE_RENDER_TARGET = 0x1,
    FVR_TEXTURE_USAGE_SHADER_READ   = 0x2,
    FVR_TEXTURE_USAGE_SHADER_WRITE  = 0x4,
} fvr_textureUsage_t;

typedef enum fvr_loadAction_t {
    FVR_LOAD_ACTION_DONT_CARE,
    FVR_LOAD_ACTION_LOAD,
    FVR_LOAD_ACTION_CLEAR
} fvr_loadAction_t;

typedef enum fvr_storeAction_t {
    FVR_STORE_ACTION_DONT_CARE,
    FVR_STORE_ACTION_STORE
} fvr_storeAction_t;
