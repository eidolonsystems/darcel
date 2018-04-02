#include <catch.hpp>
#include "darcel/reactors/none_reactor.hpp"
#include "darcel/reactors/update_reactor.hpp"
#include "darcel/reactors_tester/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_update_none", "[update_reactor]") {
  auto reactor = get_updates(none<int>());
  assert_value(*reactor, 0, base_reactor::update::COMPLETE_WITH_EVAL,
    base_reactor::update::COMPLETE);
}
