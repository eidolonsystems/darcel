#include <catch.hpp>
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/count_reactor.hpp"
#include "darcel/reactors/none_reactor.hpp"
#include "darcel/reactors/tally_reactor.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_tally_none", "[TallyReactor]") {
  auto r = tally(none<int>());
  assert_value(*r, 0, BaseReactor::Update::COMPLETE_EVAL, 0);
}

TEST_CASE("test_tally_constant", "[TallyReactor]") {
  auto r = tally(5);
  assert_value(*r, 0, BaseReactor::Update::COMPLETE_EVAL, 1);
}

TEST_CASE("test_tally_two_values", "[TallyReactor]") {
  auto r = tally(chain(true, false));
  assert_value(*r, 0, BaseReactor::Update::EVAL, 1);
  assert_value(*r, 1, BaseReactor::Update::COMPLETE_EVAL, 2);
}

TEST_CASE("test_tally_ten_values", "[TallyReactor]") {
  auto r = tally(count(1, 10));
  assert_value(*r, 0, BaseReactor::Update::EVAL, 1);
  assert_value(*r, 1, BaseReactor::Update::EVAL, 2);
  assert_value(*r, 2, BaseReactor::Update::EVAL, 3);
  assert_value(*r, 3, BaseReactor::Update::EVAL, 4);
  assert_value(*r, 4, BaseReactor::Update::EVAL, 5);
  assert_value(*r, 5, BaseReactor::Update::EVAL, 6);
  assert_value(*r, 6, BaseReactor::Update::EVAL, 7);
  assert_value(*r, 7, BaseReactor::Update::EVAL, 8);
  assert_value(*r, 8, BaseReactor::Update::EVAL, 9);
  assert_value(*r, 9, BaseReactor::Update::COMPLETE_EVAL, 10);
}
