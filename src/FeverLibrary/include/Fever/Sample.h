//===-- Fever/Sample.h - Sample class definition ----------------*- C++ -*-===//
//
//                     The Fever Graphics Library
//
// This file is distributed under the MIT License. See LICENSE.txt for details.
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Sample class, which is the
/// base class for all of the sample instructions.
///
/// X represents the input into the Renderer...
///
/// More detail...
///
//===----------------------------------------------------------------------===//
/// \name Functions that return numbers
/// @{
/// Gets the number 'three' from the module.
int fvr_sample_getThree();

/// Gets the number 'four' from the module.
///
/// Does not make any state changes to the module.
///
/// Input parameter exists for legacy reasons and is not used.
///
/// Typical usage:
/// \code
///   int four = getFour();
/// \endcode
///
/// \param myFour Function puts the number 'four' into this parameter.
///
/// \returns number 'four'.
int fvr_sample_getFour(int *const myFour);
/// @}
