#include <catch.hpp>
#include "darcel/reactors/none_reactor.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_none_int", "[NoneReactor]") {
  auto r = none<int>();
  REQUIRE(r->commit(0) == BaseReactor::Update::COMPLETE_EMPTY);
  REQUIRE_THROWS_AS(r->eval(), ReactorUnavailableException);
}
