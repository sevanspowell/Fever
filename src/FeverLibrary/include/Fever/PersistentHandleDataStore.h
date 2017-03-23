/*===-- Fever/Handle.h - Persistent Handle data container ---------*- C++ -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * \brief Same as HandleDataStore but also stores the handles internally and
 * returns pointers to those handles.
 *
 *===----------------------------------------------------------------------===*/
#pragma once

#include <Fever/HandleDataStore.h>

namespace fv {
/**
 * Duplicates the interface of the HandleDataStore but store handles internally
 * and returns pointers to those handles rather than returning the handles by
 * value.
 */
template <typename T> class PersistentHandleDataStore {
  public:
    PersistentHandleDataStore(uint32_t maxNumHandles) {
        handles.reserve(maxNumHandles);
    }

    /**
     * \copydoc HandleDataStore::isValid
     */
    bool isValid(Handle handle) const;

    /**
     * Add an object to the data store.
     *
     * Handle guaranteed to exist in same spot in memory until handle and object
     * removed using 'remove'.
     *
     * \returns Pointer to a handle to the object or nullptr if adding failed.
     */
    const Handle *add(const T &object);

    /**
     * \copydoc HandleDataStore::remove
     */
    void remove(Handle handle);

    /**
     * \copydoc HandleDataStore::get(Handle handle) const
     */
    const T *get(Handle handle) const;

    /**
     * \copydoc HandleDataStore::get(Handle handle)
     */
    T *get(Handle handle);

  private:
    HandleDataStore<T> dataStore;

    std::vector<Handle> handles;
    std::queue<size_t> freeIndices;
};
}

#include <Fever/PersistentHandleDataStore.hpp>
