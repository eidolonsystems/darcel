#include <catch.hpp>
#include "darcel/reactors/perpetual_reactor.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_perpetual", "[PerpetualReactor]") {
  auto r = make_perpetual();
  REQUIRE(r->commit(0) == BaseReactor::Update::EVAL);
  REQUIRE(r->commit(0) == BaseReactor::Update::EVAL);
  REQUIRE(r->commit(1) == BaseReactor::Update::EVAL);
  REQUIRE(r->commit(1) == BaseReactor::Update::EVAL);
  REQUIRE(r->commit(2) == BaseReactor::Update::EVAL);
  REQUIRE(r->commit(100) == BaseReactor::Update::EVAL);
}
