#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/punctuation.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_punctuation_stream", "[Punctuation]") {
  SECTION("Colon") {
    Punctuation p(Punctuation::Mark::COLON);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ":");
  }
  SECTION("Path") {
    Punctuation p(Punctuation::Mark::PATH);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "::");
  }
  SECTION("Comma") {
    Punctuation p(Punctuation::Mark::COMMA);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ",");
  }
  SECTION("Dot") {
    Punctuation p(Punctuation::Mark::DOT);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ".");
  }
  SECTION("Bar") {
    Punctuation p(Punctuation::Mark::BAR);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "|");
  }
  SECTION("Backtick") {
    Punctuation p(Punctuation::Mark::BACKTICK);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "`");
  }
  SECTION("Arrow") {
    Punctuation p(Punctuation::Mark::ARROW);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "->");
  }
}

TEST_CASE("test_punctuation_equality", "[Punctuation]") {
  Punctuation p1(Punctuation::Mark::COLON);
  Punctuation p2(Punctuation::Mark::BAR);
  REQUIRE(p1 == p1);
  REQUIRE(p1 != p2);
  REQUIRE(p1 == Punctuation::Mark::COLON);
  REQUIRE(p2 != Punctuation::Mark::COLON);
}

TEST_CASE("test_parse_punctuation", "[Punctuation]") {
  SECTION("Valid Punctuation") {
    REQUIRE(parse_punctuation(":") == Punctuation::Mark::COLON);
    REQUIRE(parse_punctuation("::") == Punctuation::Mark::PATH);
    REQUIRE(parse_punctuation(",") == Punctuation::Mark::COMMA);
    REQUIRE(parse_punctuation(".") == Punctuation::Mark::DOT);
    REQUIRE(parse_punctuation("|") == Punctuation::Mark::BAR);
    REQUIRE(parse_punctuation("`") == Punctuation::Mark::BACKTICK);
    REQUIRE(parse_punctuation("->") == Punctuation::Mark::ARROW);
  }
  SECTION("Delimiters") {
    REQUIRE(parse_punctuation(":5") == Punctuation::Mark::COLON);
    REQUIRE(parse_punctuation(":+") == Punctuation::Mark::COLON);
    REQUIRE(parse_punctuation(":::") == Punctuation::Mark::PATH);
    REQUIRE(parse_punctuation(":,") == Punctuation::Mark::COLON);
    REQUIRE(parse_punctuation(":a") == Punctuation::Mark::COLON);
  }
  SECTION("Invalid Punctuation") {
    REQUIRE(parse_punctuation("abc") == std::nullopt);
  }
}
