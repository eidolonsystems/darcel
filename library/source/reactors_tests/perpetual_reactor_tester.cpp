#include <catch.hpp>
#include "darcel/reactors/perpetual_reactor.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_perpetual", "[perpetual_reactor]") {
  auto r = make_perpetual();
  REQUIRE(r->commit(0) == base_reactor::update::EVAL);
  REQUIRE(r->commit(0) == base_reactor::update::EVAL);
  REQUIRE(r->commit(1) == base_reactor::update::EVAL);
  REQUIRE(r->commit(1) == base_reactor::update::EVAL);
  REQUIRE(r->commit(2) == base_reactor::update::EVAL);
  REQUIRE(r->commit(100) == base_reactor::update::EVAL);
}
