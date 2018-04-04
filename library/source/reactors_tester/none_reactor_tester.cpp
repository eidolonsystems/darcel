#include <catch.hpp>
#include "darcel/reactors/none_reactor.hpp"
#include "darcel/reactors_tester/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_none_int", "[none_reactor]") {
  auto r = none<int>();
  REQUIRE(r->commit(0) == base_reactor::update::COMPLETE_EMPTY);
  REQUIRE_THROWS_AS(r->eval(), reactor_unavailable_exception);
}
