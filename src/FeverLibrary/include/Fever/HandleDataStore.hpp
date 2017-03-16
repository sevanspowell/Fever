/**
 * This code was only slightly modified from this source:
 * http://bitsquid.blogspot.com.au/2014/08/building-data-oriented-handle-system.html?m=1
 * (Niklas Frykholm).
 */
#include <cassert>

#include <Fever/HandleDataStore.h>

namespace fv {
template <typename T> bool HandleDataStore<T>::isValid(Handle handle) const {
    bool result = false;

    // Only try to check Entities that could possibly
    // have an entry in out generation array.
    if (handle.getIndex() < generations.size()) {
        // Handle is only valid if the Handle's generation
        // matches our recorded valid generation.
        result = generations[handle.getIndex()] == handle.getGeneration();
    }

    return result;
}

template <typename T> Handle HandleDataStore<T>::add(const T &object) {
    uint32_t index     = 0;
    uint8_t generation = 0;

    // If the queue doesn't have any free indices, create new index.
    if (freeIndices.empty()) {
        generations.push_back(0);
        objects.push_back(object);
        index = generations.size() - 1;
    }
    // If the queue has filled up, re-use index from queue.
    else {
        index = freeIndices.front();
        freeIndices.pop();
        generation     = generations[index];
        objects[index] = object;
    }

    return makeHandle(index, generation);
}

template <typename T> void HandleDataStore<T>::remove(Handle handle) {
    // Only try to destroy valid Entities.
    if (isValid(handle)) {
        const uint32_t index = handle.getIndex();

        // Free index to be re-used
        freeIndices.push(index);
        // Increment the generation value for that index,
        // invalidating any previous references to that Handle.
        generations[index]++;
    }
}

template <typename T> const T *HandleDataStore<T>::get(Handle handle) const {
    if (!isValid(handle)) {
        return nullptr;
    } else {
        const uint32_t index = handle.getIndex();

        return &objects[index];
    }
}

template <typename T> T *HandleDataStore<T>::get(Handle handle) {
    if (!isValid(handle)) {
        return nullptr;
    } else {
        const uint32_t index = handle.getIndex();

        return &objects[index];
    }
}

template <typename T>
Handle HandleDataStore<T>::makeHandle(unsigned int index,
                                      unsigned int generation) const {
    // Ensure index is not above maximum amount of Entities.
    assert(index < (Handle::HANDLE_INDEX_MASK + 1) &&
           "Tried to create an Handle with too high an index.");
    // Ensure generation is not about maximum generation.
    assert(generation < (Handle::HANDLE_GENERATION_MASK + 1) &&
           "Tried to create an Handle with too high a generation.");

    Handle handle;
    handle.id = (generation << Handle::HANDLE_INDEX_BITS) + index;

    return handle;
}
}
