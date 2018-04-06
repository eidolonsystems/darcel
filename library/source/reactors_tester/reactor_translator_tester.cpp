#include <catch.hpp>
#include "darcel/reactors/reactor_translator.hpp"
#include "darcel/semantics/builtin_scope.hpp"
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

TEST_CASE("test_translation_function_variable", "[reactor_translator]") {
  scope s;
  auto f = bind_function("f", {{"x", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    }, s);
  auto g = bind_variable("g", find_term("f", s), s);
  auto node = bind_variable("main",
    call("g", make_literal_expression(1), s), s);
  trigger t;
  reactor_translator rt(t);
  rt.translate(*f);
  rt.translate(*g);
  rt.translate(*node);
  auto result = rt.get_main();
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_identity_generic", "[reactor_translator]") {
  scope s;
  auto f = bind_function("f", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return make_variable_expression("x", s);
    }, s);
  auto node = bind_variable("main",
    call("f", make_literal_expression(true), s), s);
  trigger t;
  reactor_translator rt(t);
  rt.translate(*f);
  rt.translate(*node);
  auto result = rt.get_main();
  auto l = std::dynamic_pointer_cast<constant_reactor<bool>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_generic_series", "[reactor_translator]") {
  scope s;
  auto f = bind_function("f", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return make_variable_expression("x", s);
    }, s);
  auto g = bind_function("g", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return call("f", make_variable_expression("x", s), s);
    }, s);
  auto node = bind_variable("main",
    call("g", make_literal_expression(314), s), s);
  trigger t;
  reactor_translator rt(t);
  rt.translate(*f);
  rt.translate(*g);
  rt.translate(*node);
  auto result = rt.get_main();
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_function_parameter", "[reactor_translator]") {
  auto s = make_builtin_scope();
  auto f = bind_function("f", {{"x", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    }, *s);
  auto g = bind_function("g",
    {{"f", make_function_data_type({{"x", integer_data_type::get_instance()}},
      integer_data_type::get_instance())}},
    [&] (auto& s) {
      return call("f", make_literal_expression(314), s);
    }, *s);
  auto node = bind_variable("main", call("g", find_term("f", *s), *s), *s);
  trigger t;
  reactor_translator rt(t);
  rt.translate(*f);
  rt.translate(*g);
  rt.translate(*node);
  auto result = rt.get_main();
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}
