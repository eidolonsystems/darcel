#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/operation.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_operation_stream", "[Operation]") {
  SECTION("Plus") {
    Operation p(Operation::Symbol::PLUS);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "+");
  }
  SECTION("Minus") {
    Operation p(Operation::Symbol::MINUS);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "-");
  }
  SECTION("Times") {
    Operation p(Operation::Symbol::TIMES);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "*");
  }
  SECTION("Divide") {
    Operation p(Operation::Symbol::DIVIDE);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "/");
  }
  SECTION("Less") {
    Operation p(Operation::Symbol::LESS);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "<");
  }
  SECTION("Less or equal") {
    Operation p(Operation::Symbol::LESS_OR_EQUAL);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "<=");
  }
  SECTION("Equal") {
    Operation p(Operation::Symbol::EQUAL);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "==");
  }
  SECTION("Greater or equal") {
    Operation p(Operation::Symbol::GREATER_OR_EQUAL);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ">=");
  }
  SECTION("Greater") {
    Operation p(Operation::Symbol::GREATER);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == ">");
  }
  SECTION("And") {
    Operation p(Operation::Symbol::AND);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "and");
  }
  SECTION("Or") {
    Operation p(Operation::Symbol::OR);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "or");
  }
  SECTION("Not") {
    Operation p(Operation::Symbol::NOT);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "not");
  }
  SECTION("Assign") {
    Operation p(Operation::Symbol::ASSIGN);
    stringstream ss;
    ss << p;
    REQUIRE(ss.str() == "=");
  }
}

TEST_CASE("test_operation_equality", "[Operation]") {
  Operation o1(Operation::Symbol::PLUS);
  Operation o2(Operation::Symbol::MINUS);
  REQUIRE(o1 == o1);
  REQUIRE(o1 != o2);
  REQUIRE(o1 == Operation::Symbol::PLUS);
  REQUIRE(o2 != Operation::Symbol::PLUS);
}

TEST_CASE("test_parse_operation", "[Operation]") {
  SECTION("Valid Operations") {
    REQUIRE(parse_operation("+") == Operation::Symbol::PLUS);
    REQUIRE(parse_operation("-") == Operation::Symbol::MINUS);
    REQUIRE(parse_operation("*") == Operation::Symbol::TIMES);
    REQUIRE(parse_operation("/") == Operation::Symbol::DIVIDE);
    REQUIRE(parse_operation("<") == Operation::Symbol::LESS);
    REQUIRE(parse_operation("<=") == Operation::Symbol::LESS_OR_EQUAL);
    REQUIRE(parse_operation("==") == Operation::Symbol::EQUAL);
    REQUIRE(parse_operation(">=") == Operation::Symbol::GREATER_OR_EQUAL);
    REQUIRE(parse_operation(">") == Operation::Symbol::GREATER);
    REQUIRE(parse_operation("and") == Operation::Symbol::AND);
    REQUIRE(parse_operation("or") == Operation::Symbol::OR);
    REQUIRE(parse_operation("not") == Operation::Symbol::NOT);
    REQUIRE(parse_operation("=") == Operation::Symbol::ASSIGN);
  }
  SECTION("Delimiters") {
    REQUIRE(parse_operation("+5") == Operation::Symbol::PLUS);
    REQUIRE(parse_operation("+(") == Operation::Symbol::PLUS);
    REQUIRE(parse_operation("+-") == Operation::Symbol::PLUS);
    REQUIRE(parse_operation("++") == Operation::Symbol::PLUS);
    REQUIRE(parse_operation("+s") == Operation::Symbol::PLUS);
  }
  SECTION("Invalid Operations") {
    REQUIRE(parse_operation("a+") == std::nullopt);
    REQUIRE(parse_operation("??") == std::nullopt);
  }
}
