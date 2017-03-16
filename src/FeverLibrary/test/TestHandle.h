#include <cstdio>

#include <Fever/Handle.h>
#include <Fever/HandleDataStore.h>
#include <Fever/PersistentHandleDataStore.h>

TEST(Handle, GetIndex0) {
    fv::Handle handle;
    handle.id = 0;

    EXPECT_EQ(0, handle.getIndex());
}

TEST(Handle, GetGeneration0) {
    fv::Handle handle;
    handle.id = 0;

    EXPECT_EQ(0, handle.getGeneration());
}

TEST(Handle, GetIndex1) {
    fv::Handle handle;
    handle.id = 1;

    EXPECT_EQ(1, handle.getIndex());
    EXPECT_EQ(0, handle.getGeneration());
}

TEST(Handle, GetGeneration1) {
    fv::Handle handle;
    handle.id = (1 << fv::Handle::HANDLE_INDEX_BITS);

    EXPECT_EQ(1, handle.getGeneration());
    EXPECT_EQ(0, handle.getIndex());
}

TEST(Handle, GetComposite) {
    fv::Handle handle;
    handle.id = (1 << fv::Handle::HANDLE_INDEX_BITS) + 3;

    EXPECT_EQ(3, handle.getIndex());
    EXPECT_EQ(1, handle.getGeneration());
}

TEST(Handle, Equality) {
    fv::Handle handle1;
    handle1.id = (1 << fv::Handle::HANDLE_INDEX_BITS) + 3;

    fv::Handle handle2;
    handle2.id = (2 << fv::Handle::HANDLE_INDEX_BITS) + 3;

    EXPECT_TRUE(handle1 == handle1);
    EXPECT_TRUE(handle2 == handle2);
    EXPECT_FALSE(handle1 == handle2);
}

// Test that 'isValid' returns false with an invalid handle
TEST(HandleDataStore, InvalidHandle) {
    fv::HandleDataStore<uint32_t> dataStore;
    fv::Handle handle;

    handle.id = 0;

    EXPECT_EQ(false, dataStore.isValid(handle));
}

// Test that 'isValid' returns true with an valid handle
TEST(HandleDataStore, ValidHandle) {
    fv::HandleDataStore<uint32_t> dataStore;
    fv::Handle handle = dataStore.add(2);

    EXPECT_EQ(true, dataStore.isValid(handle));
}

// Test that getting data from the data store works (const version)
TEST(HandleDataStore, GetImmutable) {
    fv::HandleDataStore<uint32_t> dataStore;
    uint32_t a        = 2;
    fv::Handle handle = dataStore.add(a);

    EXPECT_EQ(true, dataStore.isValid(handle));
    const uint32_t *object = dataStore.get(handle);
    EXPECT_FALSE(object == nullptr);
    EXPECT_EQ(a, *object);
}

// Test that getting data from the data store works (mutable version)
TEST(HandleDataStore, GetMutable) {
    fv::HandleDataStore<uint32_t> dataStore;
    uint32_t a        = 2;
    fv::Handle handle = dataStore.add(a);

    EXPECT_EQ(true, dataStore.isValid(handle));
    uint32_t *object = dataStore.get(handle);
    EXPECT_FALSE(object == nullptr);
    EXPECT_EQ(a, *object);
}

// Test that removing a handle from the data store invalidates it
TEST(HandleDataStore, InvalidHandleDestroyedEntity) {
    fv::HandleDataStore<uint32_t> dataStore;
    uint32_t a        = 2;
    fv::Handle handle = dataStore.add(a);

    EXPECT_EQ(true, dataStore.isValid(handle));

    dataStore.remove(handle);
    EXPECT_EQ(false, dataStore.isValid(handle));
    EXPECT_EQ(nullptr, dataStore.get(handle));
}

// Test creating a large number of handles
TEST(HandleDataStore, CreateLargeNumHandles) {
    fv::HandleDataStore<uint32_t> dataStore;
    fv::Handle handles[2048];

    for (uint32_t i = 0; i < 2048; ++i) {
        handles[i] = dataStore.add(i);
    }

    for (uint32_t i = 0; i < 2048; ++i) {
        EXPECT_TRUE(dataStore.isValid(handles[i]));
    }

    for (uint32_t i = 0; i < 2048; ++i) {
        dataStore.remove(handles[i]);
    }

    for (uint32_t i = 0; i < 2048; ++i) {
        EXPECT_FALSE(dataStore.isValid(handles[i]));
    }
}

// Test that 'isValid' returns false with an invalid handle
TEST(PersistentHandleDataStore, InvalidHandle) {
    fv::PersistentHandleDataStore<uint32_t> dataStore(64);
    fv::Handle handle;

    handle.id = 0;

    EXPECT_EQ(false, dataStore.isValid(handle));
}

// Test that 'isValid' returns true with an valid handle
TEST(PersistentHandleDataStore, ValidHandle) {
    fv::PersistentHandleDataStore<uint32_t> dataStore(64);
    const fv::Handle *handle = dataStore.add(2);
    EXPECT_TRUE(handle != nullptr);

    EXPECT_EQ(true, dataStore.isValid(*handle));
}

// Test that getting data from the data store works (const version)
TEST(PersistentHandleDataStore, GetImmutable) {
    fv::PersistentHandleDataStore<uint32_t> dataStore(64);
    uint32_t a               = 2;
    const fv::Handle *handle = dataStore.add(a);
    EXPECT_TRUE(handle != nullptr);

    EXPECT_EQ(true, dataStore.isValid(*handle));
    const uint32_t *object = dataStore.get(*handle);
    EXPECT_FALSE(object == nullptr);
    EXPECT_EQ(a, *object);
}

// Test that getting data from the data store works (mutable version)
TEST(PersistentHandleDataStore, GetMutable) {
    fv::PersistentHandleDataStore<uint32_t> dataStore(64);
    uint32_t a               = 2;
    const fv::Handle *handle = dataStore.add(a);
    EXPECT_TRUE(handle != nullptr);

    EXPECT_EQ(true, dataStore.isValid(*handle));
    uint32_t *object = dataStore.get(*handle);
    EXPECT_FALSE(object == nullptr);
    EXPECT_EQ(a, *object);
}

// Test that removing a handle from the data store invalidates it
TEST(PersistentHandleDataStore, InvalidHandleDestroyedEntity) {
    fv::PersistentHandleDataStore<uint32_t> dataStore(64);
    uint32_t a               = 2;
    const fv::Handle *handle = dataStore.add(a);
    EXPECT_TRUE(handle != nullptr);

    EXPECT_EQ(true, dataStore.isValid(*handle));

    dataStore.remove(*handle);
    EXPECT_EQ(false, dataStore.isValid(*handle));
    EXPECT_EQ(nullptr, dataStore.get(*handle));
}

// Test creating a large number of handles
TEST(PersistentHandleDataStore, CreateLargeNumHandles) {
    fv::PersistentHandleDataStore<uint32_t> dataStore(2048);
    const fv::Handle *handles[2048];

    for (uint32_t i = 0; i < 2048; ++i) {
        handles[i] = dataStore.add(i);
    }

    for (uint32_t i = 0; i < 2048; ++i) {
        EXPECT_TRUE(dataStore.isValid(*handles[i]));
    }

    // Copy all handle values
    fv::Handle handleCopies[2048];
    for (uint32_t i = 0; i < 2048; ++i) {
        handleCopies[i] = *handles[i];
    }

    for (uint32_t i = 0; i < 2048; ++i) {
        dataStore.remove(*handles[i]);
    }

    // Check that all handle values are now invalid
    for (uint32_t i = 0; i < 2048; ++i) {
        EXPECT_FALSE(dataStore.isValid(handleCopies[i]));
    }
}

// Test creating  one too many handles
TEST(PersistentHandleDataStore, CreateTooManyHandles) {
    fv::PersistentHandleDataStore<uint32_t> dataStore(64);

    // Add and validate 64 data items
    const fv::Handle *handles[64];

    for (uint32_t i = 0; i < 64; ++i) {
        handles[i] = dataStore.add(i);
    }

    for (uint32_t i = 0; i < 64; ++i) {
        EXPECT_TRUE(dataStore.isValid(*handles[i]));
    }

    // Expect adding one more to fail
    EXPECT_EQ(nullptr, dataStore.add(0));
}
