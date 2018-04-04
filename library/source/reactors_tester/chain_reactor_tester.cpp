#include <catch.hpp>
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/none_reactor.hpp"
#include "darcel/reactors_tester/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_constant_chain", "[chain_reactor]") {
  auto r = chain(100, 200);
  assert_value(*r, 0, base_reactor::update::EVAL, 100);
  assert_value(*r, 1, base_reactor::update::COMPLETE_EVAL, 200);
}

TEST_CASE("test_single_chain", "[chain_reactor]") {
  auto r = chain(911, none<int>());
  assert_value(*r, 0, base_reactor::update::EVAL, 911);
  assert_value(*r, 1, base_reactor::update::COMPLETE_EVAL, 911);
}

TEST_CASE("test_chain_immediate_transition", "[chain_reactor]") {
  auto r = chain(none<int>(), 911);
  assert_value(*r, 0, base_reactor::update::COMPLETE_EVAL, 911);
  assert_value(*r, 1, base_reactor::update::COMPLETE_EVAL, 911);
}
