#include <catch.hpp>
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/syntax_tests/syntax_tests.hpp"
#include "darcel/type_checks/type_checker.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_bind_variable_type_checker", "[type_checker]") {
  scope s;
  auto binding = bind_variable(s, "x", make_literal(123));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*binding));
  REQUIRE(*checker.get_types().get_type(*binding->get_variable()) ==
    integer_data_type());
}

TEST_CASE("test_bind_function_type_checker", "[type_checker]") {
  scope s;
  auto binding = bind_function(s, "f",
    {{"x", integer_data_type::get_instance()}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*binding));
}

TEST_CASE("test_call_type_checker", "[type_checker]") {
  scope s;
  auto binding = bind_function(s, "f",
    {{"x", integer_data_type::get_instance()}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  auto expr1 = call(s, "f", make_literal(5));
  auto expr2 = call(s, "f", make_literal(false));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*binding));
  REQUIRE_NOTHROW(checker.check(*expr1));
  REQUIRE_THROWS(checker.check(*expr2));
}

TEST_CASE("test_call_single_generic_type_checker", "[type_checker]") {
  scope s;
  auto binding = bind_function(s, "f",
    {{"x", std::make_shared<generic_data_type>(location::global(), "`T", 0)}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  auto expr1 = call(s, "f", make_literal(5));
  auto expr2 = call(s, "f", make_literal(false));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*binding));
  REQUIRE_NOTHROW(checker.check(*expr1));
  REQUIRE_NOTHROW(checker.check(*expr2));
}

TEST_CASE("test_call_two_equal_generics_type_checker", "[type_checker]") {
  scope s;
  auto binding = bind_function(s, "f",
    {{"x", std::make_shared<generic_data_type>(location::global(), "`T", 0)},
     {"y", std::make_shared<generic_data_type>(location::global(), "`T", 0)}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  auto expr1 = call(s, "f", make_literal(5), make_literal(10));
  auto expr2 = call(s, "f", make_literal(false), make_literal(10));
  auto expr3 = call(s, "f", make_literal(false));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*binding));
  REQUIRE_NOTHROW(checker.check(*expr1));
  REQUIRE_THROWS(checker.check(*expr2));
  REQUIRE_THROWS(checker.check(*expr3));
}

TEST_CASE("test_call_two_distinct_generics_type_checker", "[type_checker]") {
  scope s;
  auto binding = bind_function(s, "f",
    {{"x", std::make_shared<generic_data_type>(location::global(), "`T", 0)},
     {"y", std::make_shared<generic_data_type>(location::global(), "`U", 1)}},
    [&] (scope& s) {
      return find_term("x", s);
    });
  auto expr1 = call(s, "f", make_literal(5), make_literal(10));
  auto expr2 = call(s, "f", make_literal(false), make_literal(10));
  auto expr3 = call(s, "f", make_literal(false));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*binding));
  REQUIRE_NOTHROW(checker.check(*expr1));
  REQUIRE_NOTHROW(checker.check(*expr2));
  REQUIRE_THROWS(checker.check(*expr3));
}

TEST_CASE("test_nested_generics_type_checker", "[type_checker]") {
  scope s;
  auto f = bind_function(s, "f", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto g = bind_function(s, "g", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return call(s, "f", find_term("x", s));
    });
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*f));
  REQUIRE_NOTHROW(checker.check(*g));
}

TEST_CASE("test_function_variable_type_checker", "[type_checker]") {
  scope s;
  auto f = bind_function(s, "f",
    [&] (auto& s) {
      return make_literal(123);
    });
  auto g = bind_variable(s, "g", find_term("f", s));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*f));
  REQUIRE_NOTHROW(checker.check(*g));
  REQUIRE(*checker.get_types().get_type(*g->get_variable()) ==
    callable_data_type(f->get_function()));
}

TEST_CASE("test_passing_overloaded_function_type_checker", "[type_checker]") {
  SECTION("Concrete overloaded function.") {
    scope s;
    auto t1 = make_function_data_type({}, bool_data_type::get_instance());
    auto f = bind_function(s, "f", {{"x", t1}},
      [&] (auto& s) {
        return make_literal(123);
      });
    auto g = bind_function(s, "g",
      [&] (auto& s) {
        return make_literal(true);
      });
    auto c = call(s, "f", find_term("g", s));
    type_checker checker(s);
    REQUIRE_NOTHROW(checker.check(*f));
    REQUIRE_NOTHROW(checker.check(*g));
    REQUIRE_NOTHROW(checker.check(*c));
    auto r1 = checker.get_types().get_type(*c->get_parameters()[0]);
    REQUIRE(*r1 == *t1);
  }
  SECTION("Generic overloaded function.") {
    scope s;
    auto t1 = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    auto f = bind_function(s, "f", {{"x", t1}},
      [&] (auto& s) {
        return make_literal(123);
      });
    auto g = bind_function(s, "g",
      [&] (auto& s) {
        return make_literal(true);
      });
    auto c = call(s, "f", find_term("g", s));
    type_checker checker(s);
    REQUIRE_NOTHROW(checker.check(*f));
    REQUIRE_NOTHROW(checker.check(*g));
    REQUIRE_NOTHROW(checker.check(*c));
    auto r1 = checker.get_types().get_type(*c->get_parameters()[0]);
    REQUIRE(*r1 != *t1);
    REQUIRE(*r1 == *make_function_data_type(
      {}, bool_data_type::get_instance()));
  }
}

TEST_CASE("test_checking_generic_function_parameters", "[type_checker]") {
  scope s;
  auto f = bind_function(s, "f", {{"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto g = bind_function(s, "g",
    {{"h", make_function_data_type({{"y", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0))},
     {"z", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return call(s, "h", find_term("z", s));
    });
  auto node = bind_variable(s, "main",
    call(s, "g", find_term("f", s), make_literal(911)));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*f));
  REQUIRE_NOTHROW(checker.check(*g));
  REQUIRE_NOTHROW(checker.check(*node));
  auto& overloaded_argument = static_cast<const call_expression&>(
    node->get_expression()).get_parameters()[0];
  REQUIRE(*checker.get_types().get_type(*overloaded_argument) ==
    *make_function_data_type({{"x", integer_data_type::get_instance()}},
    integer_data_type::get_instance()));
  REQUIRE(*checker.get_types().get_type(*node->get_variable()) ==
    integer_data_type());
}

TEST_CASE("test_checking_generic_overloaded_function_parameters",
    "[type_checker]") {
  scope s;
  auto f = bind_function(s, "f",
    {{"f", make_function_data_type({{"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0))},
     {"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return call(s, "f", find_term("x", s));
    });
  auto hInt = bind_function(s, "h", {{"x", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto hBool = bind_function(s, "h", {{"x", bool_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto node = bind_variable(s, "main",
    call(s, "f", find_term("h", s), make_literal(true)));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*f));
  REQUIRE_NOTHROW(checker.check(*hInt));
  REQUIRE_NOTHROW(checker.check(*hBool));
  REQUIRE_NOTHROW(checker.check(*node));
  REQUIRE(*checker.get_types().get_type(*node->get_variable()) ==
    bool_data_type());
}

TEST_CASE("test_checking_generic_return_type", "[type_checker]") {
  scope s;
  auto f = bind_function(s, "f",
    {{"f", make_function_data_type({}, make_generic_data_type("`T", 0))}},
    [&] (auto& s) {
      return call(s, "f");
    });
  auto g = bind_function(s, "g",
    [&] (auto& s) {
      return make_literal(123);
    });
  auto node = bind_variable(s, "main", call(s, "f", find_term("g", s)));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*f));
  REQUIRE_NOTHROW(checker.check(*g));
  REQUIRE_NOTHROW(checker.check(*node));
}

TEST_CASE("test_parameter_inference", "[type_checker]") {
  scope s;
  type_map m;
  auto f = bind_function(s, "f", {{"x", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto f_definition = std::make_shared<function_definition>(location::global(),
    f->get_function(),
    make_function_data_type({{"x", integer_data_type::get_instance()}},
    std::make_shared<integer_data_type>()));
  s.add(f_definition);
  m.add(*f->get_function(),
    std::make_shared<callable_data_type>(f->get_function()));
  m.add(f_definition);
  auto g = bind_function(s, "g", {{"x", bool_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto g_definition = std::make_shared<function_definition>(location::global(),
    g->get_function(),
    make_function_data_type({{"x", bool_data_type::get_instance()}},
    std::make_shared<bool_data_type>()));
  s.add(g_definition);
  m.add(*g->get_function(),
    std::make_shared<callable_data_type>(g->get_function()));
  m.add(g_definition);
  auto chain = bind_function(s, "chain",
      {{"x", integer_data_type::get_instance()},
       {"y", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto chain_definition = std::make_shared<function_definition>(
    location::global(), chain->get_function(), make_function_data_type(
    {{"x", std::make_shared<generic_data_type>(location::global(), "`T", 0)},
     {"y", std::make_shared<generic_data_type>(location::global(), "`U", 1)}},
    std::make_shared<integer_data_type>()));
  s.add(chain_definition);
  m.add(*chain->get_function(),
    std::make_shared<callable_data_type>(chain->get_function()));
  m.add(chain_definition);
  auto x = std::make_shared<variable>(location::global(), "x");
  s.add(x);
  auto y = std::make_shared<variable>(location::global(), "y");
  s.add(y);
  SECTION("Infer single consistent type.") {
    auto e = call(s, "f", find_term("x", s));
    auto inferred_types = infer_types(*e, m, s);
    REQUIRE(*inferred_types.get_type(*x) == integer_data_type());
  }
  SECTION("Infer single inconsistent type.") {
    auto e = call(s, "chain", call(s, "f", find_term("x", s)),
      call(s, "g", find_term("x", s)));
    auto inferred_types = infer_types(*e, m, s);
    REQUIRE(inferred_types.get_type(*x) == nullptr);
  }
  SECTION("Infer two consistent types.") {
    auto e = call(s, "chain", call(s, "f", find_term("x", s)),
      call(s, "g", find_term("y", s)));
    auto inferred_types = infer_types(*e, m, s);
    REQUIRE(*inferred_types.get_type(*x) == integer_data_type());
    REQUIRE(*inferred_types.get_type(*y) == bool_data_type());
  }
}

TEST_CASE("test_generic_parameter_inference", "[type_checker]") {
  scope s;
  type_map m;
  auto f = bind_function(s, "f",
    {{"f", make_function_data_type(
      {{"x", std::make_shared<generic_data_type>(location::global(), "`T", 0)}},
      std::make_shared<generic_data_type>(location::global(), "`T", 0))}},
    [&] (auto& s) {
      return make_literal(true);
    });
  auto f_definition = std::make_shared<function_definition>(location::global(),
    f->get_function(),
    make_function_data_type({{"f", f->get_parameters()[0].m_type}},
    std::make_shared<bool_data_type>()));
  s.add(f_definition);
  m.add(*f->get_function(),
    std::make_shared<callable_data_type>(f->get_function()));
  m.add(f_definition);
  auto x = std::make_shared<variable>(location::global(), "x");
  s.add(x);
  auto expected_type = make_function_data_type(
    {{"x", integer_data_type::get_instance()}},
    integer_data_type::get_instance());
  m.add(*x, expected_type);
  auto e = call(s, "f", find_term("x", s));
  auto inferred_types = infer_types(*e, m, s);
  REQUIRE(*inferred_types.get_type(*x) == *expected_type);
}

TEST_CASE("test_nested_generic_parameter_inference", "[type_checker]") {
  scope s;
  type_map m;
  auto f = bind_function(s, "f",
    {{"a", std::make_shared<generic_data_type>(location::global(), "`T", 0)},
     {"b", std::make_shared<generic_data_type>(location::global(), "`T", 0)}},
    [&] (auto& s) {
      return find_term("a", s);
    });
  auto f_definition = std::make_shared<function_definition>(location::global(),
    f->get_function(), make_function_data_type(
    {{"a", f->get_parameters()[0].m_type},
     {"b", f->get_parameters()[1].m_type}}, f->get_parameters()[0].m_type));
  s.add(f_definition);
  m.add(*f->get_function(),
    std::make_shared<callable_data_type>(f->get_function()));
  m.add(f_definition);
  auto g = bind_function(s, "g", {{"c", integer_data_type::get_instance()}},
    [&] (auto& s) {
      return make_literal(123);
    });
  auto g_definition = std::make_shared<function_definition>(location::global(),
    g->get_function(), make_function_data_type(
    {{"a", g->get_parameters()[0].m_type}}, g->get_parameters()[0].m_type));
  s.add(g_definition);
  m.add(*g->get_function(),
    std::make_shared<callable_data_type>(g->get_function()));
  m.add(g_definition);
  auto x = std::make_shared<variable>(location::global(), "x");
  s.add(x);
  auto y = std::make_shared<variable>(location::global(), "y");
  s.add(y);
  auto e = call(s, "f", call(s, "g", find_term("x", s)), find_term("y", s));
  auto inferred_types = infer_types(*e, m, s);
  REQUIRE(*inferred_types.get_type(*x) == integer_data_type());
  REQUIRE(*inferred_types.get_type(*y) == integer_data_type());
}
