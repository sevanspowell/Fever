/**
 * This code was only slightly modified from this source:
 * http://bitsquid.blogspot.com.au/2014/08/building-data-oriented-entity-system.html?m=1
 * (Niklas Frykholm).
 *
 */
#include <Fever/Handle.h>

namespace fv {
unsigned int Handle::getIndex() const {
    // Bitwise AND id with index mask to get index bits only
    return (id & HANDLE_INDEX_MASK);
}

unsigned int Handle::getGeneration() const {
    // Bitwise AND id with gen mask to get gen bits only
    return (id & HANDLE_GENERATION_MASK) >> HANDLE_INDEX_BITS;
}
}
