#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/keyword.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_keyword_stream", "[Keyword]") {
  SECTION("enum") {
    Keyword k(Keyword::Word::ENUM);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "enum");
  }
  SECTION("let") {
    Keyword k(Keyword::Word::LET);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "let");
  }
}

TEST_CASE("test_keyword_equality", "[Keyword]") {
  Keyword k1(Keyword::Word::ENUM);
  Keyword k2(Keyword::Word::LET);
  REQUIRE(k1 == k1);
  REQUIRE(k2 == k2);
  REQUIRE(k1 != k2);
  REQUIRE(k2 != k1);
}

TEST_CASE("test_parse_keyword", "[Keyword]") {
  SECTION("Valid Keywords") {
    REQUIRE(parse_keyword("enum") == Keyword::Word::ENUM);
    REQUIRE(parse_keyword("let") == Keyword::Word::LET);
  }
  SECTION("Delimiters") {
    REQUIRE(parse_keyword("let+") == Keyword::Word::LET);
    REQUIRE(parse_keyword("let.") == Keyword::Word::LET);
    REQUIRE(parse_keyword("let5") == nullopt);
    REQUIRE(parse_keyword("lets") == std::nullopt);
  }
  SECTION("Invalid Keywords") {
    REQUIRE(parse_keyword("abc") == std::nullopt);
    REQUIRE(parse_keyword("123") == std::nullopt);
  }
}
