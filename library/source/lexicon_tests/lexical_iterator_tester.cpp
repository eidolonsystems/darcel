#include <catch.hpp>
#include "darcel/lexicon/lexical_iterator.hpp"

using namespace darcel;

TEST_CASE("test_lexical_iterator", "[LexicalIterator]") {
  auto s = "hello world";
  LexicalIterator c(s, std::strlen(s));
  REQUIRE(c.get_size_remaining() == 11);
  REQUIRE(*c == 'h');
  ++c;
  REQUIRE(c.get_size_remaining() == 10);
  REQUIRE(*c == 'e');
}

TEST_CASE("test_subtract_iterator", "[LexicalIterator]") {
  auto s = "hello world";
  LexicalIterator c1(s, std::strlen(s));
  LexicalIterator c2 = c1 + 6;
  REQUIRE(c2.get_size_remaining() == 5);
  REQUIRE(*c2 == 'w');
  REQUIRE(c2 - c1 == 6);
}
