#include <catch.hpp>
#include "darcel/reactors/reactor_translator.hpp"
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/syntax/syntax_builders.hpp"

using namespace darcel;

TEST_CASE("test_translating_literal", "[reactor_translator]") {
  scope s;
  auto node = bind_variable(s, "main", make_literal(123));
  reactor_translator rt(s);
  REQUIRE_NOTHROW(rt.translate(*node));
  trigger t;
  auto result = rt.get_main(t);
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_identity_function", "[reactor_translator]") {
  scope s;
  auto f = bind_function(s, "f", {{"x", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto node = bind_variable(s, "main", call(s, "f", make_literal(321)));
  reactor_translator rt(s);
  REQUIRE_NOTHROW(rt.translate(*f));
  REQUIRE_NOTHROW(rt.translate(*node));
  trigger t;
  auto result = rt.get_main(t);
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_function_variable", "[reactor_translator]") {
  scope s;
  auto f = bind_function(s, "f", {{"x", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto g = bind_variable(s, "g", find_term("f", s));
  auto node = bind_variable(s, "main", call(s, "g", make_literal(1)));
  reactor_translator rt(s);
  REQUIRE_NOTHROW(rt.translate(*f));
  REQUIRE_NOTHROW(rt.translate(*g));
  REQUIRE_NOTHROW(rt.translate(*node));
  trigger t;
  auto result = rt.get_main(t);
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_identity_generic", "[reactor_translator]") {
  scope s;
  auto f = bind_function(s, "f", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto node = bind_variable(s, "main", call(s, "f", make_literal(true)));
  reactor_translator rt(s);
  REQUIRE_NOTHROW(rt.translate(*f));
  REQUIRE_NOTHROW(rt.translate(*node));
  trigger t;
  auto result = rt.get_main(t);
  auto l = std::dynamic_pointer_cast<constant_reactor<bool>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_generic_series", "[reactor_translator]") {
  scope s;
  auto f = bind_function(s, "f", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto g = bind_function(s, "g", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return call(s, "f", find_term("x", s));
    });
  auto node = bind_variable(s, "main", call(s, "g", make_literal(314)));
  reactor_translator rt(s);
  REQUIRE_NOTHROW(rt.translate(*f));
  REQUIRE_NOTHROW(rt.translate(*g));
  REQUIRE_NOTHROW(rt.translate(*node));
  trigger t;
  auto result = rt.get_main(t);
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_function_parameter", "[reactor_translator]") {
  auto s = make_builtin_scope();
  auto f = bind_function(*s, "f", {{"x", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto g = bind_function(*s, "g",
    {{"f", make_function_data_type({{"x", integer_data_type::get_instance()}},
      integer_data_type::get_instance())}},
    [&] (auto& s) {
      return call(s, "f", make_literal(314));
    });
  auto node = bind_variable(*s, "main", call(*s, "g", find_term("f", *s)));
  reactor_translator rt(*s);
  REQUIRE_NOTHROW(rt.translate(*f));
  REQUIRE_NOTHROW(rt.translate(*g));
  REQUIRE_NOTHROW(rt.translate(*node));
  trigger t;
  auto result = rt.get_main(t);
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}

TEST_CASE("test_translating_generic_function_parameter",
    "[reactor_translator]") {
  auto s = make_builtin_scope();
  auto f = bind_function(*s, "f", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto g = bind_function(*s, "g",
    {{"h", make_function_data_type({{"y", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0))},
     {"z", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return call(s, "h", find_term("z", s));
    });
  auto node = bind_variable(*s, "main",
    call(*s, "g", find_term("f", *s), make_literal(911)));
  reactor_translator rt(*s);
  REQUIRE_NOTHROW(rt.translate(*f));
  REQUIRE_NOTHROW(rt.translate(*g));
  REQUIRE_NOTHROW(rt.translate(*node));
  trigger t;
  auto result = rt.get_main(t);
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}
