#include <catch.hpp>
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/syntax_tests/syntax_tests.hpp"
#include "darcel/type_checks/type_checker.hpp"

using namespace darcel;
using namespace darcel::tests;

namespace {
  auto register_function(Scope& s, type_map& t, std::string name,
      std::vector<FunctionDataType::Parameter> parameters,
      const ExpressionBuilder& body) {
    auto f = bind_function(s, std::move(name), parameters,
      [&] (auto& s) {
        for(auto& parameter : parameters) {
          t.add(*s.find<Variable>(parameter.m_name), parameter.m_type);
        }
        return body(s);
      });
    auto definition = std::make_shared<FunctionDefinition>(Location::global(),
      f->get_function(), std::make_shared<FunctionDataType>(
      std::move(parameters), t.get_type(f->get_expression())));
    s.add(definition);
    auto callable = t.get_type(*f->get_function());
    if(callable == nullptr) {
      t.add(*f->get_function(),
        std::make_shared<CallableDataType>(f->get_function()));
    }
    t.add(definition);
    return f;
  }
}

TEST_CASE("test_bind_variable_type_checker", "[type_checker]") {
  Scope s;
  auto binding = bind_variable(s, "x", make_literal(123));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*binding));
  REQUIRE(*checker.get_types().get_type(*binding->get_variable()) ==
    IntegerDataType());
}

TEST_CASE("test_bind_function_type_checker", "[type_checker]") {
  Scope s;
  auto binding = bind_function(s, "f",
    {{"x", IntegerDataType::get_instance()}},
    [&] (Scope& s) {
      return find_term("x", s);
    });
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*binding));
}

TEST_CASE("test_call_type_checker", "[type_checker]") {
  Scope s;
  auto binding = bind_function(s, "f",
    {{"x", IntegerDataType::get_instance()}},
    [&] (Scope& s) {
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
  Scope s;
  auto binding = bind_function(s, "f",
    {{"x", std::make_shared<GenericDataType>(Location::global(), "`T", 0)}},
    [&] (Scope& s) {
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
  Scope s;
  auto binding = bind_function(s, "f",
    {{"x", std::make_shared<GenericDataType>(Location::global(), "`T", 0)},
     {"y", std::make_shared<GenericDataType>(Location::global(), "`T", 0)}},
    [&] (Scope& s) {
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
  Scope s;
  auto binding = bind_function(s, "f",
    {{"x", std::make_shared<GenericDataType>(Location::global(), "`T", 0)},
     {"y", std::make_shared<GenericDataType>(Location::global(), "`U", 1)}},
    [&] (Scope& s) {
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
  Scope s;
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
  Scope s;
  auto f = bind_function(s, "f",
    [&] (auto& s) {
      return make_literal(123);
    });
  auto g = bind_variable(s, "g", find_term("f", s));
  type_checker checker(s);
  REQUIRE_NOTHROW(checker.check(*f));
  REQUIRE_NOTHROW(checker.check(*g));
  REQUIRE(*checker.get_types().get_type(*g->get_variable()) ==
    CallableDataType(f->get_function()));
}

TEST_CASE("test_passing_overloaded_function_type_checker", "[type_checker]") {
  SECTION("Concrete overloaded function.") {
    Scope s;
    auto t1 = make_function_data_type({}, BoolDataType::get_instance());
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
    auto r1 = checker.get_types().get_type(*c->get_arguments()[0]);
    REQUIRE(*r1 == *t1);
  }
  SECTION("Generic overloaded function.") {
    Scope s;
    auto t1 = std::make_shared<GenericDataType>(Location::global(), "`T", 0);
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
    auto r1 = checker.get_types().get_type(*c->get_arguments()[0]);
    REQUIRE(*r1 != *t1);
    REQUIRE(*r1 == *make_function_data_type(
      {}, BoolDataType::get_instance()));
  }
}

TEST_CASE("test_checking_generic_function_parameters", "[type_checker]") {
  Scope s;
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
  auto& overloaded_argument = static_cast<const CallExpression&>(
    node->get_expression()).get_arguments()[0];
  REQUIRE(*checker.get_types().get_type(*overloaded_argument) ==
    *make_function_data_type({{"x", IntegerDataType::get_instance()}},
    IntegerDataType::get_instance()));
  REQUIRE(*checker.get_types().get_type(*node->get_variable()) ==
    IntegerDataType());
}

TEST_CASE("test_checking_generic_overloaded_function_parameters",
    "[type_checker]") {
  Scope s;
  auto f = bind_function(s, "f",
    {{"f", make_function_data_type({{"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0))},
     {"x", make_generic_data_type("`T", 0)}},
    [&] (auto& s) {
      return call(s, "f", find_term("x", s));
    });
  auto hInt = bind_function(s, "h", {{"x", IntegerDataType::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto hBool = bind_function(s, "h", {{"x", BoolDataType::get_instance()}},
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
    BoolDataType());
}

TEST_CASE("test_checking_generic_return_type", "[type_checker]") {
  Scope s;
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

TEST_CASE("test_expression_candidates", "[type_checker]") {
  Scope s;
  type_map m;
  auto f1 = register_function(s, m, "f",
    {{"x", IntegerDataType::get_instance()}},
    [&] (auto& s) {
      return make_literal(123);
    });
  auto f2 = register_function(s, m, "f",
    {{"x", BoolDataType::get_instance()}},
    [&] (auto& s) {
      return make_literal(true);
    });
  SECTION("Single literal candidate.") {
    auto candidates = determine_expression_types(*make_literal(123), m, s);
    REQUIRE(candidates.size() == 1);
    REQUIRE(*candidates.front() == IntegerDataType());
  }
  SECTION("Overloaded function candidates.") {
    auto candidates = determine_expression_types(
      FunctionExpression(Location::global(), f1->get_function()), m, s);
    REQUIRE(candidates.size() == 3);
    REQUIRE(contains(candidates, CallableDataType(f1->get_function())));
    REQUIRE(contains(candidates,
      *make_function_data_type({{"x", IntegerDataType::get_instance()}},
      IntegerDataType::get_instance())));
    REQUIRE(contains(candidates,
      *make_function_data_type({{"x", BoolDataType::get_instance()}},
      BoolDataType::get_instance())));
  }
  SECTION("Call expression.") {
    auto candidates = determine_expression_types(
      *call(s, "f", make_literal(123)), m, s);
    REQUIRE(candidates.size() == 2);
    REQUIRE(contains(candidates, IntegerDataType()));
    REQUIRE(contains(candidates, BoolDataType()));
  }
}

TEST_CASE("test_parameter_inference", "[type_checker]") {
  Scope s;
  type_map m;
  auto f = register_function(s, m, "f",
    {{"x", IntegerDataType::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto g = register_function(s, m, "g", {{"x", BoolDataType::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto chain = register_function(s, m, "chain",
      {{"x", IntegerDataType::get_instance()},
       {"y", BoolDataType::get_instance()}},
    [&] (auto& s) {
      return find_term("x", s);
    });
  auto x = std::make_shared<Variable>(Location::global(), "x");
  s.add(x);
  auto y = std::make_shared<Variable>(Location::global(), "y");
  s.add(y);
  SECTION("Infer single consistent type.") {
    auto e = call(s, "f", find_term("x", s));
    auto inferred_types = infer_types(*e, m, s);
    REQUIRE(*inferred_types.get_type(*x) == IntegerDataType());
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
    REQUIRE(*inferred_types.get_type(*x) == IntegerDataType());
    REQUIRE(*inferred_types.get_type(*y) == BoolDataType());
  }
}

TEST_CASE("test_generic_function_parameter_inference", "[type_checker]") {
  Scope s;
  type_map m;
  auto f = register_function(s, m, "f",
      {{"f", make_function_data_type(
      {{"x", std::make_shared<GenericDataType>(Location::global(), "`T", 0)}},
      std::make_shared<GenericDataType>(Location::global(), "`T", 0))}},
    [&] (auto& s) {
      return make_literal(true);
    });
  auto x = std::make_shared<Variable>(Location::global(), "x");
  s.add(x);
  auto expected_type = make_function_data_type(
    {{"x", IntegerDataType::get_instance()}},
    IntegerDataType::get_instance());
  m.add(*x, expected_type);
  auto e = call(s, "f", find_term("x", s));
  auto inferred_types = infer_types(*e, m, s);
  REQUIRE(*inferred_types.get_type(*x) == *expected_type);
}

TEST_CASE("test_nested_generic_parameter_inference", "[type_checker]") {
  Scope s;
  type_map m;
  auto f = register_function(s, m, "f",
    {{"a", std::make_shared<GenericDataType>(Location::global(), "`T", 0)}},
    [&] (auto& s) {
      return find_term("a", s);
    });
  auto g = register_function(s, m, "g",
    {{"b", IntegerDataType::get_instance()}},
    [&] (auto& s) {
      return make_literal(123);
    });
  auto x = std::make_shared<Variable>(Location::global(), "x");
  s.add(x);
  auto e = call(s, "f", call(s, "g", find_term("x", s)));
  auto inferred_types = infer_types(*e, m, s);
  REQUIRE(*inferred_types.get_type(*x) == IntegerDataType());
}

TEST_CASE("test_generic_parameter_inference", "[type_checker]") {
  Scope s;
  type_map m;
  auto f = register_function(s, m, "f",
    {{"a", std::make_shared<GenericDataType>(Location::global(), "`T", 0)},
     {"b", std::make_shared<GenericDataType>(Location::global(), "`T", 0)}},
    [&] (auto& s) {
      return find_term("a", s);
    });
  auto x = std::make_shared<Variable>(Location::global(), "x");
  s.add(x);
  auto y = std::make_shared<Variable>(Location::global(), "y");
  s.add(y);
  SECTION("Test inferring a single variable.") {
    auto e = call(s, "f", find_term("x", s), make_literal(1));
    auto inferred_types = infer_types(*e, m, s);
    REQUIRE(*inferred_types.get_type(*x) == IntegerDataType());
  }
  SECTION("Swap the location of the single variable.") {
    auto e = call(s, "f", make_literal(1), find_term("x", s));
    auto inferred_types = infer_types(*e, m, s);
    REQUIRE(*inferred_types.get_type(*x) == IntegerDataType());
  }
  SECTION("Test inferring a chain of expressions.") {
    auto e = call(s, "f", find_term("x", s),
      call(s, "f", find_term("y", s), make_literal(1)));
    auto inferred_types = infer_types(*e, m, s);
    REQUIRE(*inferred_types.get_type(*x) == IntegerDataType());
    REQUIRE(*inferred_types.get_type(*y) == IntegerDataType());
  }
}
