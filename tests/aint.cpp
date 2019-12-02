#include "gtest/gtest.h"
#include <exception>
#include "../src/aint/aint.hpp"

TEST(AInt, Argument_less_construction) {
  aint a;
  ASSERT_TRUE(a.zero());
  ASSERT_EQ(a.size(), 0);
}
TEST(AInt, Unsigned_construction) {
  aint a = aint(1);
  ASSERT_EQ(a.size(), 1);
  ASSERT_EQ(a.get_blocks()[0], 1);

  aint b = aint(static_cast<unsigned>(0));
  ASSERT_EQ(b.size(), 0);
  ASSERT_TRUE(b.zero());
}
TEST(AInt, String_construction) {
  aint a = aint(const_cast<char *>(""));
  ASSERT_TRUE(a.zero());
  ASSERT_EQ(a.size(), 0);

  aint b = aint(const_cast<char *>("0"));
  ASSERT_TRUE(b.zero());
  ASSERT_EQ(b.size(), 0);

  aint c = aint(const_cast<char *>("1"));
  ASSERT_EQ(c.size(), 1);
  ASSERT_EQ(c.get_blocks()[0], 1);

  aint d =
      const_cast<char *>("000000000000000000000000000000001000000000000000000000000000000001000000000000000000000000000000");
  ASSERT_EQ(d.size(), 3);
  ASSERT_EQ(d.get_blocks()[0], 0);
  ASSERT_EQ(d.get_blocks()[1], 1);
  ASSERT_EQ(d.get_blocks()[2], 2);

  ASSERT_THROW(aint(const_cast<char *>("0a")), std::invalid_argument);
}
TEST(AInt, Copy) {
  aint a = aint(2);
  aint b = aint(a);
  ASSERT_EQ(a.get_blocks()[0], 2);
  ASSERT_EQ(b.size(), 1);
  ASSERT_EQ(b.get_blocks()[0], 2);
}
TEST(AInt, Move) {
  aint a = aint(2);
  aint b = aint(std::move(a));
  ASSERT_EQ(b.size(), 1);
  ASSERT_EQ(b.get_blocks()[0], 2);
  ASSERT_TRUE(a.zero());
}

TEST(AInt, Memory_management) {
  aint a = 1;
  ASSERT_EQ(a.size(), 1);
  ASSERT_EQ(a.capacity(), 1);
  a.reserve(5);
  ASSERT_EQ(a.size(), 1);
  ASSERT_EQ(a.capacity(), 5);
  a.resize(0);
  ASSERT_EQ(a.size(), 0);
  a = 1;
  a.reserve(5);
  a.shrink_to_fit();
  ASSERT_EQ(a.capacity(), 1);
  aint b = 2;
  a.swap(b);
  ASSERT_EQ(a.get_blocks()[0], 2);
  ASSERT_EQ(b.get_blocks()[0], 1);
}

TEST(AInt, Comparison_one_block) {
  aint a = 1;
  aint b = 2;
  aint c = 1;
  ASSERT_LT(a, b);
  ASSERT_GT(b, a);
  ASSERT_LE(a, c);
  ASSERT_GE(a, c);
  ASSERT_LE(a, b);
  ASSERT_GE(b, a);
  ASSERT_EQ(a, c);
  ASSERT_NE(a, b);
}
TEST(AInt, Comparison_multi_block) {
  aint a = const_cast<char *>("1000000000000000000000000000000000000000000000000000000000000001");
  aint b = const_cast<char *>("0100000000000000000000000000000000000000000000000000000000000001");
  aint c = const_cast<char *>("1000000000000000000000000000000000000000000000000000000000000001");
  aint d = const_cast<char *>("10000000000000000000000000000000000000000000000000000000000000011");
  ASSERT_LT(a, b);
  ASSERT_GT(b, a);
  ASSERT_LE(a, c);
  ASSERT_GE(a, c);
  ASSERT_LE(a, b);
  ASSERT_GE(b, a);
  ASSERT_EQ(a, c);
  ASSERT_NE(a, b);
  ASSERT_LT(a, d);
  ASSERT_GT(d, a);
  ASSERT_LE(a, d);
  ASSERT_GE(d, a);
  ASSERT_NE(a, d);
}

TEST(AInt, Operation_add) {
  aint a = const_cast<char *>("11");
  aint b = const_cast<char *>("1");
  aint c = a + b;
  ASSERT_EQ(c, aint(const_cast<char *>("001")));
  a = const_cast<char *>("11111111111111111111111111111111");
  b = const_cast<char *>("1");
  c = a + b;
  ASSERT_EQ(c, aint(const_cast<char *>("000000000000000000000000000000001")));
}
TEST(AInt, Operation_subtract) {
  aint a = const_cast<char *>("11");
  aint b = const_cast<char *>("1");
  aint c = a - b;
  ASSERT_EQ(c, aint(const_cast<char *>("01")));
  a = const_cast<char *>("000000000000000000000000000000001");
  b = const_cast<char *>("1");
  c = a - b;
  ASSERT_EQ(c, aint(const_cast<char *>("11111111111111111111111111111111")));
}
TEST(AInt, Operation_multiply) {
}
TEST(AInt, Operation_divide) {
}
TEST(AInt, Operation_modulo) {
}
TEST(AInt, Operation_left_bitshift) {
  aint a = const_cast<char *>("1110000000000000000000000000000011");
  aint b = a << 2;
  ASSERT_EQ(b, aint(const_cast<char *>("001110000000000000000000000000000011")));
  b = a << 34;
  ASSERT_EQ(b, aint(const_cast<char *>("00000000000000000000000000000000001110000000000000000000000000000011")));
}
TEST(AInt, Operation_right_bitshift) {
  aint a = const_cast<char *>("11100000000000000000000000000000111");
  aint b = a >> 2;
  ASSERT_EQ(b, aint(const_cast<char *>("100000000000000000000000000000111")));
  b = a >> 34;
  ASSERT_EQ(b, aint(const_cast<char *>("1")));
}

