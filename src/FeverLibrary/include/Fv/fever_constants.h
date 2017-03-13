/*===-- Fever/fever_constants.h - Fever Graphics Library Constants --*- C -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * \brief Collection of enumerations and constants used in the Fever Graphics
 * Library public API.
 *
 * Heavily based off the Vulkan (https://www.khronos.org/vulkan/) and Metal
 * (https://developer.apple.com/metal/) APIs.
 *
 *===---------------------------------------------------------------------===*/
typedef enum FvBufferType {
    FV_BUFFER_TYPE_VERTEX,
    FV_BUFFER_TYPE_INDEX,
} FvBufferType;

typedef enum FvShaderStage {
    FV_SHADER_STAGE_VERTEX,
    FV_SHADER_STAGE_FRAGMENT,
    FV_SHADER_STAGE_COMPUTE,
    FV_SHADER_STAGE_GEOMETRY,
} FvShaderStage;

typedef enum FvPixelFormat {
    FV_PIXEL_FORMAT_INVALID,
    FV_PIXEL_FORMAT_RGBA8UNORM,
    FV_PIXEL_FORMAT_RGBA16FLOAT,
    FV_PIXEL_FORMAT_DEPTH32FLOAT_STENCIL8,
    FV_PIXEL_FORMAT_BGRA8UNORM
} FvPixelFormat;

typedef enum FvTextureUsage {
    FV_TEXTURE_USAGE_UNKNOWN,
    FV_TEXTURE_USAGE_RENDER_TARGET,
    FV_TEXTURE_USAGE_SHADER_READ,
    FV_TEXTURE_USAGE_SHADER_WRITE
} FvTextureUsage;

typedef enum FvCompareFunc {
    /** Newer value always passes */
    FV_COMPARE_FUNC_NEVER,
    /** New value passes if it is less than existing */
    FV_COMPARE_FUNC_LESS,
    /** New value passes if it is equal to existing */
    FV_COMPARE_FUNC_EQUAL,
    /** New value passes if it is less than or equal to existing */
    FV_COMPARE_FUNC_LESS_EQUAL,
    /** New value passes if it is not equal to existing */
    FV_COMPARE_FUNC_NOT_EQUAL,
    /** New value passes if it is greater than existing */
    FV_COMPARE_FUNC_GREATER,
    /** New value passes if it is greater than or equal to existing */
    FV_COMPARE_FUNC_GREATER_EQUAL,
    /** New value always passes */
    FV_COMPARE_FUNC_ALWAYS
} FvCompareFunc;

typedef enum FvStencilOp {
    /** Keep the current stencil value */
    FV_STENCIL_OP_KEEP,
    /** Zero the current stencil value */
    FV_STENCIL_OP_ZERO,
    /** Sets the stencil value to the stencil reference value */
    FV_STENCIL_OP_REPLACE,
    /** Increments the current stencil value and clamps it to the maximum value
       of an unsigned */
    FV_STENCIL_OP_INCREMENT_CLAMP,
    /** Decrements the current stencil value and clamps it to the minimum value
        of an unsigned */
    FV_STENCIL_OP_DECREMENT_CLAMP,
    /** Bitwise invert the current value */
    FV_STENCIL_OP_INVERT,
    /** Increment the current stencil value and wrap to 0 when maximum value
       would have been exceeded */
    FV_STENCIL_OP_INCREMENT_WRAP,
    /** Decrement the current stencil value and wrap to the maximum value when
       value would go below 0 */
    FV_STENCIL_OP_DECREMENT_WRAP
} FvStencilOp;

typedef enum FvLoadOp {
    /** The contents of an attachment will be undefined after loading */
    FV_LOAD_OP_DONT_CARE,
    /** The existing contents of an attachment are preserved after loading */
    FV_LOAD_OP_LOAD,
    /** The contents of an attachment will be cleared to a given value after
       loading */
    FV_LOAD_OP_CLEAR
} FvLoadOp;

typedef enum FvStoreOp {
    /** After rendering, attachment is left in undefined state */
    FV_STORE_OP_DONT_CARE,
    /** Results of render pass are stored in attachment */
    FV_STORE_OP_STORE
} FvStoreOp;

typedef enum FvBlendFactor {
    FV_BLEND_FACTOR_ZERO,
    FV_BLEND_FACTOR_ONE,
    FV_BLEND_FACTOR_SOURCE_COLOR,
    FV_BLEND_FACTOR_ONE_MINUS_SOURCE_COLOR,
    FV_BLEND_FACTOR_SOURCE_ALPHA,
    FV_BLEND_FACTOR_ONE_MINUS_SOURCE_ALPHA,
    FV_BLEND_FACTOR_DST_COLOR,
    FV_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    FV_BLEND_FACTOR_DST_ALPHA,
    FV_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    FV_BLEND_FACTOR_SOURCE_ALPHA_SATURATED,
    FV_BLEND_FACTOR_BLEND_COLOR,
    FV_BLEND_FACTOR_ONE_MINUS_BLEND_COLOR,
    FV_BLEND_FACTOR_BLEND_ALPHA,
    FV_BLEND_FACTOR_ONE_MINUS_BLEND_ALPHA,
    FV_BLEND_FACTOR_SOURCE1_COLOR,
    FV_BLEND_FACTOR_ONE_MINUS_SOURCE1_COLOR,
    FV_BLEND_FACTOR_SOURCE1_ALPHA,
    FV_BLEND_FACTOR_ONE_MINUS_SOURCE1_ALPHA
} FvBlendFactor;

typedef enum FvBlendOp {
    /** Add parts of both src and dst pixel values */
    FV_BLEND_OP_ADD,
    /** Subtract part of dst pixel value from src */
    FV_BLEND_OP_SUBTRACT,
    /** Subtract part of src pixel value from dst */
    FV_BLEND_OP_REVERSE_SUBTRACT,
    /** Choose the minimum of the src and dst pixel values */
    FV_BLEND_OP_MIN,
    /** Choose the maximum of the src and dst pixel values */
    FV_BLEND_OP_MAX
} FvBlendOp;

typedef enum FvColorComponentFlags {
    FV_COLOR_COMPONENT_R = 1 << 0,
    FV_COLOR_COMPONENT_G = 1 << 1,
    FV_COLOR_COMPONENT_B = 1 << 2,
    FV_COLOR_COMPONENT_A = 1 << 3
} FvColorComponentFlags;

