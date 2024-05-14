#include <gtest/gtest.h>

// Demonstrate some basic assertions.

TEST(BasicTest, StrNE) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
}

TEST(BasicTest, intEQ) {
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}