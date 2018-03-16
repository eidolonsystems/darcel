#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/keyword.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_keyword_stream", "[keyword]") {
  SECTION("let") {
    keyword k(keyword::word::LET);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "let");
  }
}

TEST_CASE("test_keyword_equality", "[keyword]") {
  keyword k1(keyword::word::LET);
  REQUIRE(k1 == k1);
}

TEST_CASE("test_parse_keyword", "[keyword]") {
  SECTION("Valid Keywords") {
    REQUIRE(parse_keyword("let") == keyword::word::LET);
  }
  SECTION("Delimiters") {
    REQUIRE(parse_keyword("let+") == keyword::word::LET);
    REQUIRE(parse_keyword("let.") == keyword::word::LET);
    REQUIRE(parse_keyword("let5") == nullopt);
    REQUIRE(parse_keyword("lets") == std::nullopt);
  }
  SECTION("Invalid Keywords") {
    REQUIRE(parse_keyword("abc") == std::nullopt);
    REQUIRE(parse_keyword("123") == std::nullopt);
  }
}
