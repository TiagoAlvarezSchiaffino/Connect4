#include "../../../gtest/googletest/googletest/include/gtest/gtest.h"
#include "hash_table.hpp"

// Tests for TranspositionTable
TEST(TranspositionTableTest, PutAndGet) {
TranspositionTable transTable;

// Test put and get
transTable.put(123, 42, false);
EXPECT_EQ(transTable.get(123), 42);

// Test collisions and replacement
transTable.put(123, 99, false);
EXPECT_EQ(transTable.get(123), 99);

// Test key not found
EXPECT_EQ(transTable.get(456), 0);
}

TEST(TranspositionTableTest, CollisionRate) {
TranspositionTable transTable;

// Insert some entries to generate collisions
for (int i = 0; i < 200; ++i) {
transTable.put(i, i % 10, false);
}

// Test collision rate
EXPECT_GT(transTable.getCollisionRate(), 0.0);
}

TEST(TranspositionTableTest, Reset) {
TranspositionTable transTable;

// Insert some entries
transTable.put(123, 42, false);
transTable.put(456, 99, false);

// Reset the table
transTable.reset();

// Ensure previous entries have been removed
EXPECT_EQ(transTable.get(123), 0);
EXPECT_EQ(transTable.get(456), 0);

// Ensure collision rate has been reset
EXPECT_EQ(transTable.getCollisionRate(), 0.0);
}

// You can add more tests as needed

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
