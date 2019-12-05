#include "gtest/gtest.h"
#include <exception>
#include "../src/aint/aint.hpp"

TEST(AInt, Argument_less_construction) {
  aint a;
  ASSERT_TRUE(a.zero());
  ASSERT_EQ(a.size(), 0u);
}
TEST(AInt, Unsigned_construction) {
  aint a = aint(1u);
  ASSERT_EQ(a.size(), 1u);
  ASSERT_EQ(a.get_blocks()[0], 1u);

  aint b = aint(0u);
  ASSERT_EQ(b.size(), 0u);
  ASSERT_TRUE(b.zero());
}
TEST(AInt, String_construction) {
  aint a = aint(const_cast<char*>(""));
  ASSERT_TRUE(a.zero());
  ASSERT_EQ(a.size(), 0u);

  aint b = aint(const_cast<char*>("0"));
  ASSERT_TRUE(b.zero());
  ASSERT_EQ(b.size(), 0u);

  aint c = aint(const_cast<char*>("1"));
  ASSERT_EQ(c.size(), 1u);
  ASSERT_EQ(c.get_blocks()[0], 1u);

  aint d =
          const_cast<char*>("000000000000000000000000000000001000000000000000000000000000000001000000000000000000000000000000");
  ASSERT_EQ(d.size(), 3);
  ASSERT_EQ(d.get_blocks()[0], 0u);
  ASSERT_EQ(d.get_blocks()[1], 1u);
  ASSERT_EQ(d.get_blocks()[2], 2u);

  ASSERT_THROW(aint(const_cast<char*>("0a")), std::invalid_argument);
}
TEST(AInt, Copy) {
  aint a = aint(2u);
  aint b = aint(a);
  ASSERT_EQ(a.get_blocks()[0], 2u);
  ASSERT_EQ(b.size(), 1u);
  ASSERT_EQ(b.get_blocks()[0], 2u);
}
TEST(AInt, Move) {
  aint a = aint(2u);
  aint b = aint(std::move(a));
  ASSERT_EQ(b.size(), 1u);
  ASSERT_EQ(b.get_blocks()[0], 2u);
  ASSERT_TRUE(a.zero());
}

TEST(AInt, Memory_management) {
  aint a = 1u;
  ASSERT_EQ(a.size(), 1u);
  ASSERT_EQ(a.capacity(), 1u);
  a.reserve(5);
  ASSERT_EQ(a.size(), 1u);
  ASSERT_EQ(a.capacity(), 5u);
  a.resize(0);
  ASSERT_EQ(a.size(), 0u);
  a = 1u;
  a.reserve(5);
  a.shrink_to_fit();
  ASSERT_EQ(a.capacity(), 1);
  aint b = 2u;
  a.swap(b);
  ASSERT_EQ(a.get_blocks()[0], 2u);
  ASSERT_EQ(b.get_blocks()[0], 1u);
}

TEST(AInt, Comparison_one_block) {
  aint a = 1u;
  aint b = 2u;
  aint c = 1u;
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
  aint a = const_cast<char*>("1000000000000000000000000000000000000000000000000000000000000001");
  aint b = const_cast<char*>("0100000000000000000000000000000000000000000000000000000000000001");
  aint c = const_cast<char*>("1000000000000000000000000000000000000000000000000000000000000001");
  aint d = const_cast<char*>("10000000000000000000000000000000000000000000000000000000000000011");
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
  aint a = 3u;
  aint b = 1u;
  aint c = a + b;
  ASSERT_EQ(c, 4u);
  a = const_cast<char*>("11111111111111111111111111111111");
  b = 1u;
  c = a + b;
  ASSERT_EQ(c, aint(const_cast<char*>("000000000000000000000000000000001")));
}
TEST(AInt, Operation_subtract) {
  aint a = 3u;
  aint b = 1u;
  aint c = a - b;
  ASSERT_EQ(c, 2u);
  a = const_cast<char*>("000000000000000000000000000000001");
  b = 1u;
  c = a - b;
  ASSERT_EQ(c, aint(const_cast<char*>("11111111111111111111111111111111")));
  c = a - a;
  ASSERT_EQ(c, 0u);
}
TEST(AInt, Operation_multiply) {
  aint a = 1u;
  aint b = 5u;
  aint c = a * b;
  ASSERT_EQ(c, 5u);
  c = b * a;
  ASSERT_EQ(c, 5u);
  a = 2u;
  c = a * b;
  ASSERT_EQ(c, 10u);
  c = b * a;
  ASSERT_EQ(c, 10u);
  a = const_cast<char*>("011000000000000000000000000000001");
  b = const_cast<char*>("01");
  c = a * b;
  ASSERT_EQ(c, aint(const_cast<char*>("0011000000000000000000000000000001")));

  a = const_cast<char*>("100000000000000000000000000000001");
  b = const_cast<char*>("011000000000000000000000000000001");
  c = a * b;
  ASSERT_EQ(c,
          aint(const_cast<char*>("01100000000000000000000000000000111000000000000000000000000000001")));
}
TEST(AInt, Operation_divide) {
  aint a, b, c;
  a = const_cast<char*>("01");
  b = const_cast<char*>("0011000000000000000000000000000001");
  c = a / b;
  ASSERT_TRUE(c.zero());
  a = const_cast<char*>("0011000000000000000000000000000001");
  b = const_cast<char*>("01");
  c = a / b;
  ASSERT_EQ(c, aint(const_cast<char*>("011000000000000000000000000000001")));
  a = const_cast<char*>("01100000000000000000000000000000111000000000000000000000000000001");
  b = const_cast<char*>("100000000000000000000000000000001");
  c = a / b;
  ASSERT_EQ(c, aint(const_cast<char*>("011000000000000000000000000000001")));
}
TEST(AInt, Operation_modulo) {
  aint a, b, c;
  a = const_cast<char*>("01");
  b = const_cast<char*>("0011000000000000000000000000000001");
  c = a % b;
  ASSERT_EQ(c, a);
  a = const_cast<char*>("1011000000000000000000000000000001");
  b = const_cast<char*>("01");
  c = a % b;
  ASSERT_EQ(c, aint(const_cast<char*>("1")));
  a = const_cast<char*>("01100000000000000000000000000100111000000000000000000000000000001");
  b = const_cast<char*>("100000000000000000000000000000001");
  c = a % b;
  ASSERT_EQ(c, aint(const_cast<char*>("000000000000000000000000000001")));
}
TEST(AInt, Operation_left_bitshift) {
  aint a = const_cast<char*>("1110000000000000000000000000000011");
  aint b = a << 2;
  ASSERT_EQ(b, aint(const_cast<char*>("001110000000000000000000000000000011")));
  b = a << 34;
  ASSERT_EQ(b,
          aint(const_cast<char*>("00000000000000000000000000000000001110000000000000000000000000000011")));
}
TEST(AInt, Operation_right_bitshift) {
  aint a = const_cast<char*>("11100000000000000000000000000000111");
  aint b = a >> 2;
  ASSERT_EQ(b, aint(const_cast<char*>("100000000000000000000000000000111")));
  b = a >> 34;
  ASSERT_EQ(b, 1u);
}

