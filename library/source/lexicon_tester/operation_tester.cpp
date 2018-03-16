#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/operation.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_operation_stream", "[operation]") {
  SECTION("Plus") {
    operation p(operation::symbol::PLUS);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "+");
  }
  SECTION("Minus") {
    operation p(operation::symbol::MINUS);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "-");
  }
  SECTION("Times") {
    operation p(operation::symbol::TIMES);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "*");
  }
  SECTION("Divide") {
    operation p(operation::symbol::DIVIDE);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "/");
  }
  SECTION("Less") {
    operation p(operation::symbol::LESS);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "<");
  }
  SECTION("Less or equal") {
    operation p(operation::symbol::LESS_OR_EQUAL);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "<=");
  }
  SECTION("Equal") {
    operation p(operation::symbol::EQUAL);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "==");
  }
  SECTION("Greater or equal") {
    operation p(operation::symbol::GREATER_OR_EQUAL);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ">=");
  }
  SECTION("Greater") {
    operation p(operation::symbol::GREATER);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ">");
  }
  SECTION("And") {
    operation p(operation::symbol::AND);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "and");
  }
  SECTION("Or") {
    operation p(operation::symbol::OR);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "or");
  }
  SECTION("Not") {
    operation p(operation::symbol::NOT);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "not");
  }
}

TEST_CASE("test_operation_equality", "[operation]") {
  operation o1(operation::symbol::PLUS);
  operation o2(operation::symbol::MINUS);
  REQUIRE(o1 == o1);
  REQUIRE(o1 != o2);
  REQUIRE(o1 == operation::symbol::PLUS);
  REQUIRE(o2 != operation::symbol::PLUS);
}

TEST_CASE("test_parse_operation", "[operation]") {
  SECTION("Valid Operations") {
    REQUIRE(parse_operation("+") == operation::symbol::PLUS);
    REQUIRE(parse_operation("-") == operation::symbol::MINUS);
    REQUIRE(parse_operation("*") == operation::symbol::TIMES);
    REQUIRE(parse_operation("/") == operation::symbol::DIVIDE);
    REQUIRE(parse_operation("<") == operation::symbol::LESS);
    REQUIRE(parse_operation("<=") == operation::symbol::LESS_OR_EQUAL);
    REQUIRE(parse_operation("==") == operation::symbol::EQUAL);
    REQUIRE(parse_operation(">=") == operation::symbol::GREATER_OR_EQUAL);
    REQUIRE(parse_operation(">") == operation::symbol::GREATER);
    REQUIRE(parse_operation("and") == operation::symbol::AND);
    REQUIRE(parse_operation("or") == operation::symbol::OR);
    REQUIRE(parse_operation("not") == operation::symbol::NOT);
  }
  SECTION("Delimiters") {
    REQUIRE(parse_operation("+5") == operation::symbol::PLUS);
    REQUIRE(parse_operation("+(") == operation::symbol::PLUS);
    REQUIRE(parse_operation("+-") == operation::symbol::PLUS);
    REQUIRE(parse_operation("++") == operation::symbol::PLUS);
    REQUIRE(parse_operation("+s") == operation::symbol::PLUS);
  }
  SECTION("Invalid Operations") {
    REQUIRE(parse_operation("a+") == std::nullopt);
    REQUIRE(parse_operation("??") == std::nullopt);
  }
}
