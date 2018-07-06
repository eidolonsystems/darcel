#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/bracket.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bracket_stream", "[bracket]") {
  SECTION("Open bracket") {
    bracket b(bracket::type::OPEN_ROUND_BRACKET);
    stringstream ss;
    ss << b;
    REQUIRE(ss.str() == "(");
  }
  SECTION("Close bracket") {
    bracket b(bracket::type::CLOSE_ROUND_BRACKET);
    stringstream ss;
    ss << b;
    REQUIRE(ss.str() == ")");
  }
}

TEST_CASE("test_bracket_equality", "[bracket]") {
  bracket b1(bracket::type::OPEN_ROUND_BRACKET);
  bracket b2(bracket::type::CLOSE_ROUND_BRACKET);
  REQUIRE(b1 == b1);
  REQUIRE(b1 != b2);
  REQUIRE(b1 == bracket::type::OPEN_ROUND_BRACKET);
  REQUIRE(b2 != bracket::type::OPEN_ROUND_BRACKET);
}

TEST_CASE("test_parse_bracket", "[bracket]") {
  SECTION("Valid Bracket") {
    REQUIRE(parse_bracket("(") == bracket::type::OPEN_ROUND_BRACKET);
    REQUIRE(parse_bracket(")") == bracket::type::CLOSE_ROUND_BRACKET);
  }
  SECTION("Delimiters") {
    REQUIRE(parse_bracket("(5") == bracket::type::OPEN_ROUND_BRACKET);
    REQUIRE(parse_bracket("(+") == bracket::type::OPEN_ROUND_BRACKET);
    REQUIRE(parse_bracket("((") == bracket::type::OPEN_ROUND_BRACKET);
    REQUIRE(parse_bracket("),") == bracket::type::CLOSE_ROUND_BRACKET);
    REQUIRE(parse_bracket(")a") == bracket::type::CLOSE_ROUND_BRACKET);
  }
  SECTION("Invalid Bracket") {
    REQUIRE(parse_bracket("a(") == std::nullopt);
  }
}
