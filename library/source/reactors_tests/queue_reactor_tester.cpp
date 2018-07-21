#include <catch.hpp>
#include "darcel/reactors/queue_reactor.hpp"
#include "darcel/reactors_tests/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_queue_immediate_complete", "[QueueReactor]") {
  Trigger t;
  auto reactor = make_queue_reactor<int>(t);
  assert_exception<ReactorUnavailableException>(*reactor, 0,
    BaseReactor::Update::NONE);
  reactor->set_complete();
  assert_exception<ReactorUnavailableException>(*reactor, 1,
    BaseReactor::Update::COMPLETE_EMPTY);
}

TEST_CASE("test_queue_complete_with_throw", "[QueueReactor]") {
  Trigger t;
  auto reactor = make_queue_reactor<int>(t);
  assert_exception<ReactorUnavailableException>(*reactor, 0,
    BaseReactor::Update::NONE);
  reactor->set_complete(dummy_exception());
  assert_exception<dummy_exception>(*reactor, 1,
    BaseReactor::Update::COMPLETE_EVAL);
}

TEST_CASE("test_queue_single_value", "[QueueReactor]") {
  Trigger t;
  auto reactor = make_queue_reactor<int>(t);
  assert_exception<ReactorUnavailableException>(*reactor, 0,
    BaseReactor::Update::NONE);
  reactor->push(123);
  assert_value(*reactor, 1, BaseReactor::Update::EVAL, 123);
  reactor->set_complete();
  assert_value(*reactor, 2, BaseReactor::Update::COMPLETE_EVAL, 123);
}

TEST_CASE("test_queue_single_value_and_throw", "[QueueReactor]") {
  Trigger t;
  auto reactor = make_queue_reactor<int>(t);
  assert_exception<ReactorUnavailableException>(*reactor, 0,
    BaseReactor::Update::NONE);
  reactor->push(123);
  assert_value(*reactor, 1, BaseReactor::Update::EVAL, 123);
  reactor->set_complete(dummy_exception());
  assert_exception<dummy_exception>(*reactor, 2,
    BaseReactor::Update::COMPLETE_EVAL);
}
