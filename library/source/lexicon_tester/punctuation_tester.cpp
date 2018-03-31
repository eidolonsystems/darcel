#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/punctuation.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_punctuation_stream", "[punctuation]") {
  SECTION("Colon") {
    punctuation p(punctuation::mark::COLON);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ":");
  }
  SECTION("Path") {
    punctuation p(punctuation::mark::PATH);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "::");
  }
  SECTION("Comma") {
    punctuation p(punctuation::mark::COMMA);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ",");
  }
  SECTION("Dot") {
    punctuation p(punctuation::mark::DOT);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ".");
  }
  SECTION("Bar") {
    punctuation p(punctuation::mark::BAR);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "|");
  }
  SECTION("Backtick") {
    punctuation p(punctuation::mark::BACKTICK);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "`");
  }
}

TEST_CASE("test_punctuation_equality", "[punctuation]") {
  punctuation p1(punctuation::mark::COLON);
  punctuation p2(punctuation::mark::BAR);
  REQUIRE(p1 == p1);
  REQUIRE(p1 != p2);
  REQUIRE(p1 == punctuation::mark::COLON);
  REQUIRE(p2 != punctuation::mark::COLON);
}

TEST_CASE("test_parse_punctuation", "[punctuation]") {
  SECTION("Valid Punctuation") {
    REQUIRE(parse_punctuation(":") == punctuation::mark::COLON);
    REQUIRE(parse_punctuation("::") == punctuation::mark::PATH);
    REQUIRE(parse_punctuation(",") == punctuation::mark::COMMA);
    REQUIRE(parse_punctuation(".") == punctuation::mark::DOT);
    REQUIRE(parse_punctuation("|") == punctuation::mark::BAR);
    REQUIRE(parse_punctuation("`") == punctuation::mark::BACKTICK);
  }
  SECTION("Delimiters") {
    REQUIRE(parse_punctuation(":5") == punctuation::mark::COLON);
    REQUIRE(parse_punctuation(":+") == punctuation::mark::COLON);
    REQUIRE(parse_punctuation(":::") == punctuation::mark::PATH);
    REQUIRE(parse_punctuation(":,") == punctuation::mark::COLON);
    REQUIRE(parse_punctuation(":a") == punctuation::mark::COLON);
  }
  SECTION("Invalid Punctuation") {
    REQUIRE(parse_punctuation("abc") == std::nullopt);
  }
}
