#include <gtest/gtest.h>

TEST(Test, One) { EXPECT_EQ(1, 1); }

#include "TestHandle.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
