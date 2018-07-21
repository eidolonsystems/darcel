#include <catch.hpp>
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/fold_reactor.hpp"
#include "darcel/reactors/operators.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_fold_constant", "[FoldReactor]") {
  auto left = make_fold_parameter<int>();
  auto right = make_fold_parameter<int>();
  auto add_reactor = add(static_pointer_cast<Reactor<int>>(left),
    static_pointer_cast<Reactor<int>>(right));
  auto r = fold(add_reactor, left, right, 5);
  assert_exception<ReactorUnavailableException>(*r, 0,
    BaseReactor::Update::COMPLETE_EMPTY);
}

TEST_CASE("test_fold_two_values", "[FoldReactor]") {
  auto left = make_fold_parameter<int>();
  auto right = make_fold_parameter<int>();
  auto add_reactor = add(static_pointer_cast<Reactor<int>>(left),
    static_pointer_cast<Reactor<int>>(right));
  auto r = fold(add_reactor, left, right, chain(5, 10));
  assert_exception<ReactorUnavailableException>(*r, 0,
    BaseReactor::Update::NONE);
  assert_value(*r, 1, BaseReactor::Update::COMPLETE_EVAL, 15);
}
