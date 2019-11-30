#include <catch2/catch.hpp>
#include <exception>
#include "../src/aint/aint.hpp"

TEST_CASE("Construction / assignation") {
  SECTION("Argument-less") {
    aint a;
    CHECK(a.zero());
    CHECK(a.size() == 0);
  }SECTION("Unsigned integer argument") {
    aint a = aint(1);
    CHECK(a.size() == 1);
    size_t block_count = 0;
    for (const auto &block : a) {
      CHECK(block == 1);
      block_count++;
    }
    CHECK(block_count == 1);

    aint b = aint((unsigned) 0);
    CHECK(b.size() == 0);
    CHECK(b.zero());
  }SECTION("String argument") {
    aint a = aint(const_cast<char *>(""));
    CHECK(a.zero());
    CHECK(a.size() == 0);

    aint b = aint(const_cast<char *>("0"));
    CHECK(b.zero());
    CHECK(b.size() == 0);

    aint c = aint(const_cast<char *>("1"));
    CHECK(c.size() == 1);
    size_t block_count = 0;
    for (const auto &block : c) {
      CHECK(block == 1);
      block_count++;
    }
    CHECK(block_count == 1);

    aint d =
        aint(const_cast<char *>("000000000000000000000000000000001000000000000000000000000000000001000000000000000000000000000000"));
    CHECK(d.size() == 3);
    block_count = 0;
    for (const auto &block : d) {
      CHECK(block == block_count);
      block_count++;
    }
    CHECK(block_count == 3);

    CHECK_THROWS_AS(aint(const_cast<char *>("0a")), std::invalid_argument);
  }SECTION("Copy") {
    aint a = aint(2);
    aint b = aint(a);
    CHECK(a.size() == 1);
    for (const auto &block : a) {
      CHECK(block == 2);
    }
    CHECK(b.size() == 1);
    for (const auto &block : b) {
      CHECK(block == 2);
    }
  }SECTION("Move") {
    aint a = aint(2);
    aint b = aint(std::move(a));
    CHECK(b.size() == 1);
    for (const auto &block : b) {
      CHECK(block == 2);
    }
    CHECK(a.zero());
  }
}

TEST_CASE("Memory management") {
  aint a = 1;
  CHECK(a.size() == 1);
  CHECK(a.capacity() == 1);
  a.reserve(5);
  CHECK(a.size() == 1);
  CHECK(a.capacity() == 5);
  a.resize(0);
  CHECK(a.size() == 0);
  a = 1;
  a.reserve(5);
  a.shrink_to_fit();
  CHECK(a.capacity() == 1);
  aint b = 2;
  a.swap(b);
  for (const auto &block : a) {
    CHECK(block == 2);
  }
  for (const auto &block : b) {
    CHECK(block == 1);
  }
}

TEST_CASE("Comparison") {
  SECTION("One block") {
    aint a = 1;
    aint b = 2;
    aint c = 1;
    CHECK(a < b);
    CHECK(b > a);
    CHECK(a <= c);
    CHECK(a >= c);
    CHECK(a <= b);
    CHECK(b >= a);
    CHECK(a == c);
    CHECK(a != b);
  }SECTION("Multi-block") {
    aint a = const_cast<char *>("1000000000000000000000000000000000000000000000000000000000000001");
    aint b = const_cast<char *>("0100000000000000000000000000000000000000000000000000000000000001");
    aint c = const_cast<char *>("1000000000000000000000000000000000000000000000000000000000000001");
    aint d = const_cast<char *>("10000000000000000000000000000000000000000000000000000000000000011");
    CHECK(a < b);
    CHECK(b > a);
    CHECK(a <= c);
    CHECK(a >= c);
    CHECK(a <= b);
    CHECK(b >= a);
    CHECK(a == c);
    CHECK(a != b);
    CHECK(a < d);
    CHECK(d > a);
    CHECK(a <= d);
    CHECK(d >= a);
    CHECK(a != d);
  }
}

TEST_CASE("Accumulative operation") {
  SECTION("+=") {

  }SECTION("-=") {

  }SECTION("*=") {

  }SECTION("/=") {

  }SECTION("%=") {

  }SECTION("<<=") {

  }SECTION(">>=") {

  }
}
