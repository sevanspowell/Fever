//===-- Sample.c - Sample class implementation --------------------*- C -*-===//
//
//                     The Fever Graphics Library
//
// This file is distributed under the MIT License. See LICENSE.txt for details.
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of the Sample class, which is the
/// base class for all of the sample instructions.
///
/// This implementation was created after careful study of the bgfx library,
/// credit to the author ... for ...
///
//===----------------------------------------------------------------------===//
#include <GL/glew.h>

int fvr_sample_getThree() {
    glewInit();
    return 3;
}

// Use arithmetic to return four.
int fvr_sample_getFour(int *const myFour) { return (5 - 1); }
