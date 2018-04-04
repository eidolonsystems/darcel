#include <catch.hpp>
#include "darcel/reactors/last_reactor.hpp"
#include "darcel/reactors_tester/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_last_constant", "[last_reactor]") {
  auto reactor = last(123);
  assert_value(*reactor, 0, base_reactor::update::COMPLETE_EVAL, 123);
}
