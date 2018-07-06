#include <catch.hpp>
#include "darcel/reactors/count_reactor.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_count_none", "[count_reactor]") {
  auto reactor = count(10, 9);
  assert_exception<reactor_unavailable_exception>(*reactor, 0,
    base_reactor::update::COMPLETE_EMPTY);
}

TEST_CASE("test_count_single", "[count_reactor]") {
  auto reactor = count(1, 1);
  assert_value(*reactor, 0, base_reactor::update::COMPLETE_EVAL, 1);
}

TEST_CASE("test_count_double", "[count_reactor]") {
  auto reactor = count(10, 11);
  assert_value(*reactor, 0, base_reactor::update::EVAL, 10);
  assert_value(*reactor, 1, base_reactor::update::COMPLETE_EVAL, 11);
}

TEST_CASE("test_count_triple", "[count_reactor]") {
  auto reactor = count(10, 12);
  assert_value(*reactor, 0, base_reactor::update::EVAL, 10);
  assert_value(*reactor, 1, base_reactor::update::EVAL, 11);
  assert_value(*reactor, 2, base_reactor::update::COMPLETE_EVAL, 12);
}
