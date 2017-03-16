/*===-- Fever/Handle.h - Opaque handle object ---------------------*- C++ -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * \brief Opaque handle object.
 *
 * This code was only slightly modified from this source:
 * http://bitsquid.blogspot.com.au/2014/08/building-data-oriented-entity-system.html?m=1
 * (Niklas Frykholm).
 *
 *===----------------------------------------------------------------------===*/
#pragma once

#include <cstdint>

namespace fv {
/**
 * An handle is merely a unique identifier.
 *
 * This unique identifier is made up of two parts:
 *     - an index (lower, less significant X bits)
 *     - a generation (higher, more significant Y bits)
 * Each time an index is re-used it's generation is increased,
 * this allows us to check if an Handle id is valid or not.
 */
class Handle {
  public:
    /**
     * Get the index part of the Handle id.
     *
     * @return     unsigned int, index part of Handle id.
     */
    unsigned int getIndex() const;
    /**
     * Get the generation part of the Handle id.
     *
     * @return     unsigned int, generation part of Handle id.
     */
    unsigned int getGeneration() const;

    /** Number of bits making up index */
    static const uint8_t HANDLE_INDEX_BITS = 24;
    /** Number of bits making up generation */
    static const uint8_t HANDLE_GENERATION_BITS = 8;

    /**
     * Index mask.
     *
     * If HANDLE_INDEX_BITS = 22:
     *  (1 << 22) will leave a 1 in the 23rd bit and 0s elsewhere.
     *  Subtracting 1 from this will leave 1s in all 22 bits and 0s elsewhere,
     *  forming a mask for the index bits.
     */
    static const uint32_t HANDLE_INDEX_MASK = (1 << HANDLE_INDEX_BITS) - 1;
    /** Generation mask */
    static const uint32_t HANDLE_GENERATION_MASK = ~HANDLE_INDEX_MASK;

    /** Unique identifier */
    uint32_t id;
};
}
