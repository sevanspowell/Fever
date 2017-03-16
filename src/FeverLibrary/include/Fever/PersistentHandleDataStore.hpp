namespace fv {
template <typename T>
bool PersistentHandleDataStore<T>::isValid(Handle handle) const {
    // Valid if handle exists in our handle storage and is a valid handle
    return (
        (std::find(handles.begin(), handles.end(), handle) != handles.end()) &&
        (dataStore.isValid(handle)));
}

template <typename T>
const Handle *PersistentHandleDataStore<T>::add(const T &object) {
    Handle *handlePtr = nullptr;

    // No 'holes' to fill
    if (freeIndices.empty()) {
        // Haven't exceeded maxNumHandles
        if (handles.size() < handles.capacity()) {
            handles.push_back(dataStore.add(object));
            handlePtr = &handles[handles.size() - 1];
        }
    }
    // Else fill free indices
    else {
        size_t index = freeIndices.front();
        freeIndices.pop();
        handles[index] = dataStore.add(object);
        handlePtr      = &handles[index];
    }

    return handlePtr;
}

template <typename T> void PersistentHandleDataStore<T>::remove(Handle handle) {
    dataStore.remove(handle);

    std::vector<Handle>::const_iterator it =
        std::find(handles.begin(), handles.end(), handle);
    if (it != handles.end()) {
        size_t index = it - handles.begin();

        freeIndices.push(index);
    }
}

template <typename T>
const T *PersistentHandleDataStore<T>::get(Handle handle) const {
    return dataStore.get(handle);
}

template <typename T> T *PersistentHandleDataStore<T>::get(Handle handle) {
    return dataStore.get(handle);
}
}
