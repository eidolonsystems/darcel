#include <catch.hpp>
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/type_checks/type_checker.hpp"

using namespace darcel;

TEST_CASE("test_bind_variable_type_checker", "[type_checker]") {
  scope top_scope;
  auto binding = bind_variable(top_scope, "x", make_literal(123));
  type_checker checker(top_scope);
  REQUIRE_NOTHROW(checker.check(*binding));
  REQUIRE(*checker.get_type(*binding->get_variable()) ==
    integer_data_type());
}

TEST_CASE("test_bind_function_type_checker", "[type_checker]") {
  scope top_scope;
  auto binding = bind_function(top_scope, "f",
    {{"x", integer_data_type::get_instance()}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  type_checker checker(top_scope);
  REQUIRE_NOTHROW(checker.check(*binding));
}

TEST_CASE("test_call_type_checker", "[type_checker]") {
  scope top_scope;
  type_checker checker(top_scope);
  auto binding = bind_function(top_scope, "f",
    {{"x", integer_data_type::get_instance()}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  REQUIRE_NOTHROW(checker.check(*binding));
  auto expr1 = call(top_scope, "f", make_literal(5));
  REQUIRE_NOTHROW(checker.check(*expr1));
  auto expr2 = call(top_scope, "f", make_literal(false));
  REQUIRE_THROWS(checker.check(*expr2));
}

TEST_CASE("test_call_single_generic_type_checker", "[type_checker]") {
  scope top_scope;
  type_checker checker(top_scope);
  auto binding = bind_function(top_scope, "f",
    {{"x", std::make_shared<generic_data_type>(location::global(), "`T", 0)}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  REQUIRE_NOTHROW(checker.check(*binding));
  auto expr1 = call(top_scope, "f", make_literal(5));
  REQUIRE_NOTHROW(checker.check(*expr1));
  auto expr2 = call(top_scope, "f", make_literal(false));
  REQUIRE_NOTHROW(checker.check(*expr2));
}

TEST_CASE("test_call_two_equal_generics_type_checker", "[type_checker]") {
  scope top_scope;
  type_checker checker(top_scope);
  auto binding = bind_function(top_scope, "f",
    {{"x", std::make_shared<generic_data_type>(location::global(), "`T", 0)},
     {"y", std::make_shared<generic_data_type>(location::global(), "`T", 0)}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  REQUIRE_NOTHROW(checker.check(*binding));
  auto expr1 = call(top_scope, "f", make_literal(5), make_literal(10));
  REQUIRE_NOTHROW(checker.check(*expr1));
  auto expr2 = call(top_scope, "f", make_literal(false), make_literal(10));
  REQUIRE_THROWS(checker.check(*expr2));
  auto expr3 = call(top_scope, "f", make_literal(false));
  REQUIRE_THROWS(checker.check(*expr3));
}

TEST_CASE("test_call_two_distinct_generics_type_checker", "[type_checker]") {
  scope top_scope;
  type_checker checker(top_scope);
  auto binding = bind_function(top_scope, "f",
    {{"x", std::make_shared<generic_data_type>(location::global(), "`T", 0)},
     {"y", std::make_shared<generic_data_type>(location::global(), "`U", 1)}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  REQUIRE_NOTHROW(checker.check(*binding));
  auto expr1 = call(top_scope, "f", make_literal(5), make_literal(10));
  REQUIRE_NOTHROW(checker.check(*expr1));
  auto expr2 = call(top_scope, "f", make_literal(false), make_literal(10));
  REQUIRE_NOTHROW(checker.check(*expr2));
  auto expr3 = call(top_scope, "f", make_literal(false));
  REQUIRE_THROWS(checker.check(*expr3));
}

TEST_CASE("test_nested_generics_type_checker", "[type_checker]") {
  scope top_scope;
  type_checker checker(top_scope);
  auto f = bind_function(top_scope, "f",
    {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto g = bind_function(top_scope, "g",
    {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return call(s, "f", find_term("x", s));
    });
  REQUIRE_NOTHROW(checker.check(*f));
  REQUIRE_NOTHROW(checker.check(*g));
}

TEST_CASE("test_function_variable_type_checker", "[type_checker]") {
  scope top_scope;
  type_checker checker(top_scope);
  auto f = bind_function(top_scope, "f",
    [&] (auto& s) {
      return make_literal(123);
    });
  REQUIRE_NOTHROW(checker.check(*f));
  auto g = bind_variable(top_scope, "g", find_term("f", top_scope));
  REQUIRE_NOTHROW(checker.check(*g));
  REQUIRE(*checker.get_type(*g->get_variable()) ==
    callable_data_type(f->get_function()));
}
