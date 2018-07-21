#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/bracket.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bracket_stream", "[Bracket]") {
  SECTION("Open bracket") {
    Bracket b(Bracket::Type::ROUND_OPEN);
    stringstream ss;
    ss << b;
    REQUIRE(ss.str() == "(");
  }
  SECTION("Close bracket") {
    Bracket b(Bracket::Type::ROUND_CLOSE);
    stringstream ss;
    ss << b;
    REQUIRE(ss.str() == ")");
  }
}

TEST_CASE("test_bracket_equality", "[Bracket]") {
  Bracket b1(Bracket::Type::ROUND_OPEN);
  Bracket b2(Bracket::Type::ROUND_CLOSE);
  REQUIRE(b1 == b1);
  REQUIRE(b1 != b2);
  REQUIRE(b1 == Bracket::Type::ROUND_OPEN);
  REQUIRE(b2 != Bracket::Type::ROUND_OPEN);
}

TEST_CASE("test_parse_bracket", "[Bracket]") {
  SECTION("Valid Bracket") {
    REQUIRE(parse_bracket("(") == Bracket::Type::ROUND_OPEN);
    REQUIRE(parse_bracket(")") == Bracket::Type::ROUND_CLOSE);
  }
  SECTION("Delimiters") {
    REQUIRE(parse_bracket("(5") == Bracket::Type::ROUND_OPEN);
    REQUIRE(parse_bracket("(+") == Bracket::Type::ROUND_OPEN);
    REQUIRE(parse_bracket("((") == Bracket::Type::ROUND_OPEN);
    REQUIRE(parse_bracket("),") == Bracket::Type::ROUND_CLOSE);
    REQUIRE(parse_bracket(")a") == Bracket::Type::ROUND_CLOSE);
  }
  SECTION("Invalid Bracket") {
    REQUIRE(parse_bracket("a(") == std::nullopt);
  }
}
