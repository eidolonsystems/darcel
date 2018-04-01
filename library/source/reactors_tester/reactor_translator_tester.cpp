#include <catch.hpp>
#include "darcel/reactors/reactor_translator.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/syntax/syntax_builders.hpp"

using namespace darcel;

TEST_CASE("test_translating_literal", "[reactor_translator]") {
  scope s;
  auto node = bind_variable("main", make_literal_expression(123), s);
  trigger t;
  reactor_translator rt(t);
  rt.translate(*node);
  auto result = rt.get_main();
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_identity_function", "[reactor_translator]") {
  scope s;
  auto f = bind_function("f", {{"x", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return make_variable_expression("x", s);
    }, s);
  auto node = bind_variable("main",
    call("f", make_literal_expression(321), s), s);
  trigger t;
  reactor_translator rt(t);
  rt.translate(*f);
  rt.translate(*node);
  auto result = rt.get_main();
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}
