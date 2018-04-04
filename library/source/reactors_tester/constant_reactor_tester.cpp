#include <string>
#include <catch.hpp>
#include "darcel/reactors/constant_reactor.hpp"

using namespace darcel;
using namespace std::literals;

TEST_CASE("test_constant_int", "[constant_reactor]") {
  auto constant = make_constant_reactor(123);
  REQUIRE(constant->commit(0) == base_reactor::update::COMPLETE_EVAL);
  REQUIRE(constant->eval() == 123);
  REQUIRE(constant->commit(1) == base_reactor::update::NONE);
}

TEST_CASE("test_constant_decimal", "[constant_reactor]") {
  auto constant = make_constant_reactor(3.14);
  REQUIRE(constant->commit(100) == base_reactor::update::COMPLETE_EVAL);
  REQUIRE(constant->eval() == 3.14);
  REQUIRE(constant->commit(101) == base_reactor::update::NONE);
}

TEST_CASE("test_constant_string", "[constant_reactor]") {
  auto constant = make_constant_reactor("hello world"s);
  REQUIRE(constant->commit(123) == base_reactor::update::COMPLETE_EVAL);
  REQUIRE(constant->eval() == "hello world"s);
  REQUIRE(constant->commit(124) == base_reactor::update::NONE);
}

TEST_CASE("test_constant_lift", "[constant_reactor]") {
  auto literal = lift(5);
  REQUIRE(literal->commit(0) == base_reactor::update::COMPLETE_EVAL);
  REQUIRE(literal->eval() == 5);
  REQUIRE(literal->commit(1) == base_reactor::update::NONE);
  auto reactor = lift(make_constant_reactor(10));
  REQUIRE(reactor->commit(0) == base_reactor::update::COMPLETE_EVAL);
  REQUIRE(reactor->eval() == 10);
  REQUIRE(reactor->commit(1) == base_reactor::update::NONE);
}
