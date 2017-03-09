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
} FvShaderStage;

typedef enum FvTextureFormat {
    FV_TEXTURE_FORMAT_INVALID,
    FV_TEXTURE_FORMAT_RGBA8UNORM,
    FV_TEXTURE_FORMAT_RGBA16FLOAT,
    FV_TEXTURE_FORMAT_DEPTH32FLOAT_STENCIL8,
    FV_TEXTURE_FORMAT_BGRA8UNORM
} FvTextureFormat;

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
