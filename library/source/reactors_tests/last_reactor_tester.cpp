#include <catch.hpp>
#include "darcel/reactors/last_reactor.hpp"
#include "darcel/reactors/none_reactor.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_last_constant", "[last_reactor]") {
  auto reactor = last(123);
  assert_value(*reactor, 0, BaseReactor::Update::COMPLETE_EVAL, 123);
}

TEST_CASE("test_last_none", "[last_reactor]") {
  auto reactor = last(none<int>());
  assert_exception<ReactorUnavailableException>(*reactor, 0,
    BaseReactor::Update::COMPLETE_EMPTY);
}
