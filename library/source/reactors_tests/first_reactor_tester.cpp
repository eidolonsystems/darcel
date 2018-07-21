#include <catch.hpp>
#include "darcel/reactors/first_reactor.hpp"
#include "darcel/reactors/none_reactor.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_first_constant", "[first_reactor]") {
  auto reactor = first(123);
  assert_value(*reactor, 0, BaseReactor::Update::COMPLETE_EVAL, 123);
}

TEST_CASE("test_first_none", "[first_reactor]") {
  auto reactor = first(none<int>());
  assert_exception<ReactorUnavailableException>(*reactor, 0,
    BaseReactor::Update::COMPLETE_EMPTY);
}
