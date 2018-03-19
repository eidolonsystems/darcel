#include <catch.hpp>
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/function_reactor.hpp"
#include "darcel/reactors_tester/assertions.hpp"

using namespace darcel;
using namespace darcel::tests;

namespace {
  int no_parameter_function() {
    return 512;
  }

  int no_parameter_function_throw() {
    throw dummy_exception();
  }

  std::optional<int> no_parameter_function_empty() {
    return std::nullopt;
  }

  int square(int x) {
    return x * x;
  }
}

TEST_CASE("test_function_no_parameters", "[function_reactor]") {
  auto reactor = make_function_reactor(&no_parameter_function);
  assert_value(*reactor, 0, base_reactor::update::COMPLETE_WITH_EVAL, 512);
  assert_value(*reactor, 1, base_reactor::update::NONE, 512);
}

TEST_CASE("test_function_no_parameters_throw", "[function_reactor]") {
  auto reactor = make_function_reactor(&no_parameter_function_throw);
  assert_exception<dummy_exception>(*reactor, 0,
    base_reactor::update::COMPLETE_WITH_EVAL);
  assert_exception<dummy_exception>(*reactor, 1, base_reactor::update::NONE);
}

TEST_CASE("test_function_no_parameters_empty", "[function_reactor]") {
  auto reactor = make_function_reactor(&no_parameter_function_empty);
  assert_exception<reactor_unavailable_exception>(*reactor, 0,
    base_reactor::update::COMPLETE);
  assert_exception<reactor_unavailable_exception>(*reactor, 1,
    base_reactor::update::NONE);
}

TEST_CASE("test_function_one_constant_parameter", "[function_reactor]") {
  auto reactor = make_function_reactor(&square, make_constant_reactor(5));
  assert_value(*reactor, 0, base_reactor::update::COMPLETE_WITH_EVAL, 25);
  assert_value(*reactor, 1, base_reactor::update::NONE, 25);
}
