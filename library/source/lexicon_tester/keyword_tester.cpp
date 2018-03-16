#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/keyword.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_keyword_stream", "[keyword]") {
  SECTION("define") {
    keyword k(keyword::word::DEFINE);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "def");
  }
  SECTION("let") {
    keyword k(keyword::word::LET);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "let");
  }
  SECTION("if") {
    keyword k(keyword::word::IF);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "if");
  }
  SECTION("else") {
    keyword k(keyword::word::ELSE);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "else");
  }
  SECTION("else if") {
    keyword k(keyword::word::ELSE_IF);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "else if");
  }
  SECTION("end") {
    keyword k(keyword::word::END);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "end");
  }
  SECTION("return") {
    keyword k(keyword::word::RETURN);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "return");
  }
  SECTION("ignore") {
    keyword k(keyword::word::IGNORE);
    stringstream ss;
    ss << k;
    REQUIRE(ss.str() == "_");
  }
}

TEST_CASE("test_keyword_equality", "[keyword]") {
  keyword k1(keyword::word::DEFINE);
  keyword k2(keyword::word::LET);
  REQUIRE(k1 == k1);
  REQUIRE(k1 != k2);
  REQUIRE(k1 == keyword::word::DEFINE);
  REQUIRE(k2 != keyword::word::DEFINE);
}

TEST_CASE("test_parse_keyword", "[keyword]") {
  SECTION("Valid Keywords") {
    REQUIRE(parse_keyword("def") == keyword::word::DEFINE);
    REQUIRE(parse_keyword("let") == keyword::word::LET);
    REQUIRE(parse_keyword("if") == keyword::word::IF);
    REQUIRE(parse_keyword("else") == keyword::word::ELSE);
    REQUIRE(parse_keyword("if") == keyword::word::IF);
    REQUIRE(parse_keyword("else if") == keyword::word::ELSE_IF);
    REQUIRE(parse_keyword("end") == keyword::word::END);
    REQUIRE(parse_keyword("return") == keyword::word::RETURN);
    REQUIRE(parse_keyword("_") == keyword::word::IGNORE);
  }
  SECTION("Delimiters") {
    REQUIRE(parse_keyword("def+") == keyword::word::DEFINE);
    REQUIRE(parse_keyword("def.") == keyword::word::DEFINE);
    REQUIRE(parse_keyword("def5") == nullopt);
    REQUIRE(parse_keyword("defs") == std::nullopt);
    REQUIRE(parse_keyword("_s") == std::nullopt);
    REQUIRE(parse_keyword("s_") == std::nullopt);
  }
  SECTION("Invalid Keywords") {
    REQUIRE(parse_keyword("abc") == std::nullopt);
    REQUIRE(parse_keyword("123") == std::nullopt);
  }
  SECTION("if/else/else if") {
    REQUIRE(parse_keyword("else") == keyword::word::ELSE);
    REQUIRE(parse_keyword("else if") == keyword::word::ELSE_IF);
    REQUIRE(parse_keyword("else else") == keyword::word::ELSE);
    auto s = "else i";
    REQUIRE(parse_keyword(lexical_iterator(s, std::strlen(s))) == std::nullopt);
    s = "else if";
    REQUIRE(parse_keyword(lexical_iterator(s, std::strlen(s))) == std::nullopt);
    s = "else ib";
    REQUIRE(parse_keyword(lexical_iterator(s, std::strlen(s))) ==
      keyword::word::ELSE);
    s = "else i b";
    REQUIRE(parse_keyword(lexical_iterator(s, std::strlen(s))) ==
      keyword::word::ELSE);
    s = "else ifb";
    REQUIRE(parse_keyword(lexical_iterator(s, std::strlen(s))) ==
      keyword::word::ELSE);
    REQUIRE(parse_keyword("else                \n\n\n\n               if") ==
      keyword::word::ELSE_IF);
  }
}
