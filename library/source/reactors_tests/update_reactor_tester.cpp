#include <catch.hpp>
#include "darcel/reactors/none_reactor.hpp"
#include "darcel/reactors/update_reactor.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_update_none", "[UpdateReactor]") {
  auto reactor = get_updates(none<int>());
  assert_value(*reactor, 0, BaseReactor::Update::COMPLETE_EVAL,
    BaseReactor::Update::COMPLETE_EMPTY);
}
