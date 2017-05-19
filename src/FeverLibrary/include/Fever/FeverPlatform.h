/*===-- Fever/FeverPlatform.h - Used to determine platform ----------*- C -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * \brief Determine compilation platform.
 *
 * Adapted from Daniele Bartolini's platform
 * https://github.com/taylor001/crown/blob/master/src/core/platform.h:
 *
 * Modified by Samuel Evans-Powell.
 *
 *===----------------------------------------------------------------------===*/
#pragma once

#define FV_COMPILER_CLANG 0
#define FV_COMPILER_GCC 0
#define FV_COMPILER_MSVC 0

#define FV_PLATFORM_LINUX 0
#define FV_PLATFORM_MACOS 0
#define FV_PLATFORM_WINDOWS 0

// https://sourceforge.net/p/predef/wiki/Compilers/
#if defined(_MSC_VER)
#undef FV_COMPILER_MSVC
#define FV_COMPILER_MSVC 1
#elif defined(__clang__)
// clang defines __GNUC__
#undef FV_COMPILER_CLANG
#define FV_COMPILER_CLANG 1
#undef FV_COMPILER_GCC
#define FV_COMPILER_GCC 1
#elif defined(__GNUC__)
#undef FV_COMPILER_GCC
#define FV_COMPILER_GCC 1
#else
#error "FV_COMPILER_* is not defined!"
#endif

// https://sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined(_WIN32) || defined(_WIN64)
#undef FV_PLATFORM_WINDOWS
#define FV_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#undef FV_PLATFORM_LINUX
#define FV_PLATFORM_LINUX 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#undef FV_PLATFORM_MACOS
#define FV_PLATFORM_MACOS 1
#else
#error "FV_PLATFORM_* is not defined!"
#endif

#define FV_PLATFORM_POSIX (FV_PLATFORM_LINUX || FV_PLATFORM_MACOS)

#if FV_PLATFORM_LINUX
#define FV_PLATFORM_NAME "linux"
#elif FV_PLATFORM_MACOS
#define FV_PLATFORM_NAME "osx"
#elif FV_PLATFORM_WINDOWS
#define FV_PLATFORM_NAME "windows"
#endif

// Force inlining
#if FV_COMPILER_MSVC
#define FV_FORCE_INLINE __forceinline
#elif FV_COMPILER_GCC
#define FV_FORCE_INLINE __attribute__((always_inline))
#endif

