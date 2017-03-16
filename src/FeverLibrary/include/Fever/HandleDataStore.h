/*===-- Fever/Handle.h - Handle data container --------------------*- C++ -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * \brief Container for data referred to by handle.
 *
 * This code was only slightly modified from this source:
 * http://bitsquid.blogspot.com.au/2014/08/building-data-oriented-entity-system.html?m=1
 * (Niklas Frykholm).
 *
 *===----------------------------------------------------------------------===*/
#pragma once

#include <queue>
#include <vector>

#include <Fever/Handle.h>

namespace fv {
/**
 * Manages data that is referred to by handles.
 *
 * - Re-uses expired handle IDs.
 * - Prevents duplication of handle IDs.
 */
template <typename T> class HandleDataStore {
  public:
    /**
     * Handle id's are a weak reference. This method checks to see if the id is
     * valid.
     *
     * Index slots are re-used and a generation field is used to keep track of
     * which index slots are valid. This methods checks that the generation
     * field matches the current known generation value for that index. If they
     * don't match, the Handle is no longer valid.
     *
     * \param  handle Handle to check for validity.
     * \return        True if handle id is valid, false otherwise.
     */
    bool isValid(Handle handle) const;

    /**
     * Add a new object to the data store.
     *
     * \param  object Reference to object to add.
     * \return        Handle, created handle.
     */
    Handle add(const T &object);

    /**
     * Remove the object referred to by the given handle from the data store.
     *
     * Release the Handle's id so it can be re-used and invalidate any current
     * references to the Handle.
     *
     * \param handle Handle to destroy.
     */
    void remove(Handle handle);

    /**
     * Get pointer to element with given handle or nullptr if handle invalid.
     */
    const T *get(Handle handle) const;

    /**
     * Get pointer to element with given handle or nullptr if handle invalid.
     */
    T *get(Handle handle);

  private:
    /**
     * Construct a Handle from an index and a generation value.
     *
     * \pre    index is smaller than the maximum value possible given the number
     * of bits assigned to it.
     * \pre    generation is smaller than the maximum value possible given the
     * number of bits assigned to it.
     *
     * \param  index       unsigned int, index value to give Handle.
     * \param  generation  unsigned int, generation value to give Handle.
     * \return             Handle, constructed from index and value.
     */
    Handle makeHandle(unsigned int index, unsigned int generation) const;

    std::queue<uint32_t> freeIndices;
    std::vector<uint8_t> generations;
    std::vector<T> objects;
};
}

#include <Fever/HandleDataStore.hpp>
