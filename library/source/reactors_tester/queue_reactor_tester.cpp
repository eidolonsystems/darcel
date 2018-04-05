#include <catch.hpp>
#include "darcel/reactors/queue_reactor.hpp"
#include "darcel/reactors_tester/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_queue_immediate_complete", "[queue_reactor]") {
  trigger t;
  auto reactor = make_queue_reactor<int>(t);
  assert_exception<reactor_unavailable_exception>(*reactor, 0,
    base_reactor::update::NONE);
  reactor->set_complete();
  assert_exception<reactor_unavailable_exception>(*reactor, 1,
    base_reactor::update::COMPLETE_EMPTY);
}

TEST_CASE("test_queue_complete_with_throw", "[queue_reactor]") {
  trigger t;
  auto reactor = make_queue_reactor<int>(t);
  assert_exception<reactor_unavailable_exception>(*reactor, 0,
    base_reactor::update::NONE);
  reactor->set_complete(dummy_exception());
  assert_exception<dummy_exception>(*reactor, 1,
    base_reactor::update::COMPLETE_EVAL);
}

TEST_CASE("test_queue_single_value", "[queue_reactor]") {
  trigger t;
  auto reactor = make_queue_reactor<int>(t);
  assert_exception<reactor_unavailable_exception>(*reactor, 0,
    base_reactor::update::NONE);
  reactor->push(123);
  assert_value(*reactor, 1, base_reactor::update::EVAL, 123);
  reactor->set_complete();
  assert_value(*reactor, 2, base_reactor::update::COMPLETE_EVAL, 123);
}

TEST_CASE("test_queue_single_value_and_throw", "[queue_reactor]") {
  trigger t;
  auto reactor = make_queue_reactor<int>(t);
  assert_exception<reactor_unavailable_exception>(*reactor, 0,
    base_reactor::update::NONE);
  reactor->push(123);
  assert_value(*reactor, 1, base_reactor::update::EVAL, 123);
  reactor->set_complete(dummy_exception());
  assert_exception<dummy_exception>(*reactor, 2,
    base_reactor::update::COMPLETE_EVAL);
}
