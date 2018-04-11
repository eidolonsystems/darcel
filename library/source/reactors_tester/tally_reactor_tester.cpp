#include <catch.hpp>
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/count_reactor.hpp"
#include "darcel/reactors/none_reactor.hpp"
#include "darcel/reactors/tally_reactor.hpp"
#include "darcel/reactors_tester/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_tally_none", "[tally_reactor]") {
  auto r = tally(none<int>());
  assert_value(*r, 0, base_reactor::update::COMPLETE_EVAL, 0);
}

TEST_CASE("test_tally_constant", "[tally_reactor]") {
  auto r = tally(5);
  assert_value(*r, 0, base_reactor::update::COMPLETE_EVAL, 1);
}

TEST_CASE("test_tally_two_values", "[tally_reactor]") {
  auto r = tally(chain(true, false));
  assert_value(*r, 0, base_reactor::update::EVAL, 1);
  assert_value(*r, 1, base_reactor::update::COMPLETE_EVAL, 2);
}

TEST_CASE("test_tally_ten_values", "[tally_reactor]") {
  auto r = tally(count(1, 10));
  assert_value(*r, 0, base_reactor::update::EVAL, 1);
  assert_value(*r, 1, base_reactor::update::EVAL, 2);
  assert_value(*r, 2, base_reactor::update::EVAL, 3);
  assert_value(*r, 3, base_reactor::update::EVAL, 4);
  assert_value(*r, 4, base_reactor::update::EVAL, 5);
  assert_value(*r, 5, base_reactor::update::EVAL, 6);
  assert_value(*r, 6, base_reactor::update::EVAL, 7);
  assert_value(*r, 7, base_reactor::update::EVAL, 8);
  assert_value(*r, 8, base_reactor::update::EVAL, 9);
  assert_value(*r, 9, base_reactor::update::COMPLETE_EVAL, 10);
}
