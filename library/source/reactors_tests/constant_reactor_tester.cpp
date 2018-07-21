#include <string>
#include <catch.hpp>
#include "darcel/reactors/constant_reactor.hpp"

using namespace darcel;
using namespace std::literals;

TEST_CASE("test_constant_int", "[ConstantReactor]") {
  auto constant = make_constant_reactor(123);
  REQUIRE(constant->commit(0) == BaseReactor::Update::COMPLETE_EVAL);
  REQUIRE(constant->eval() == 123);
}

TEST_CASE("test_constant_decimal", "[ConstantReactor]") {
  auto constant = make_constant_reactor(3.14);
  REQUIRE(constant->commit(100) == BaseReactor::Update::COMPLETE_EVAL);
  REQUIRE(constant->eval() == 3.14);
}

TEST_CASE("test_constant_string", "[ConstantReactor]") {
  auto constant = make_constant_reactor("hello world"s);
  REQUIRE(constant->commit(123) == BaseReactor::Update::COMPLETE_EVAL);
  REQUIRE(constant->eval() == "hello world"s);
}

TEST_CASE("test_constant_lift", "[ConstantReactor]") {
  auto literal = lift(5);
  REQUIRE(literal->commit(0) == BaseReactor::Update::COMPLETE_EVAL);
  REQUIRE(literal->eval() == 5);
  auto reactor = lift(make_constant_reactor(10));
  REQUIRE(reactor->commit(0) == BaseReactor::Update::COMPLETE_EVAL);
  REQUIRE(reactor->eval() == 10);
}
