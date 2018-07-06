#include <catch.hpp>
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/fold_reactor.hpp"
#include "darcel/reactors/operators.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_fold_constant", "[fold_reactor]") {
  auto left = make_fold_parameter<int>();
  auto right = make_fold_parameter<int>();
  auto add_reactor = add(static_pointer_cast<reactor<int>>(left),
    static_pointer_cast<reactor<int>>(right));
  auto r = fold(add_reactor, left, right, 5);
  assert_exception<reactor_unavailable_exception>(*r, 0,
    base_reactor::update::COMPLETE_EMPTY);
}

TEST_CASE("test_fold_two_values", "[fold_reactor]") {
  auto left = make_fold_parameter<int>();
  auto right = make_fold_parameter<int>();
  auto add_reactor = add(static_pointer_cast<reactor<int>>(left),
    static_pointer_cast<reactor<int>>(right));
  auto r = fold(add_reactor, left, right, chain(5, 10));
  assert_exception<reactor_unavailable_exception>(*r, 0,
    base_reactor::update::NONE);
  assert_value(*r, 1, base_reactor::update::COMPLETE_EVAL, 15);
}
