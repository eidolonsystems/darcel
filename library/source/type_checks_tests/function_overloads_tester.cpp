#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/type_checks/function_overloads.hpp"

using namespace darcel;

TEST_CASE("test_single_parameter_function_overload", "[function_overloads]") {
  SECTION("No matching overload.") {
    scope top_scope;
    auto f = std::make_shared<function>(location::global(), "f");
    top_scope.add(f);
    auto definition = std::make_shared<function_definition>(
      location::global(), f,
      make_function_data_type({{"a", integer_data_type::get_instance()}},
      integer_data_type::get_instance()));
    top_scope.add(definition);
    auto overload = find_overload(*f,
      {{"a", bool_data_type::get_instance()}}, top_scope);
    REQUIRE(overload == nullptr);
  }
}

TEST_CASE("test_generic_function_parameter_overload", "[function_overloads]") {
  SECTION("Valid substitution.") {
    scope top_scope;
    auto generic_type = make_function_data_type(
      {{"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0));
    auto instantiated_type = make_function_data_type(
      {{"x", integer_data_type::get_instance()}},
      integer_data_type::get_instance());
    auto h = std::make_shared<function>(location::global(), "h");
    top_scope.add(h);
    auto h_definition = std::make_shared<function_definition>(
      location::global(), h, make_function_data_type({{"f", generic_type}},
      integer_data_type::get_instance()));
    top_scope.add(h_definition);
    auto overload = find_overload(*h, {{"f", instantiated_type}}, top_scope);
    REQUIRE(overload == h_definition);
  }
  SECTION("Invalid substitution.") {
    scope top_scope;
    auto generic_type = make_function_data_type(
      {{"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0));
    auto instantiated_type = make_function_data_type(
      {{"x", integer_data_type::get_instance()}},
      bool_data_type::get_instance());
    auto h = std::make_shared<function>(location::global(), "h");
    top_scope.add(h);
    auto h_definition = std::make_shared<function_definition>(
      location::global(), h, make_function_data_type({{"f", generic_type}},
      integer_data_type::get_instance()));
    top_scope.add(h_definition);
    auto overload = find_overload(*h, *instantiated_type, top_scope);
    REQUIRE(overload == nullptr);
  }
}

TEST_CASE("test_two_generic_function_parameters_overload",
    "[function_overloads]") {
  SECTION("Valid substitution.") {
    scope top_scope;
    auto generic_type = make_function_data_type(
      {{"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0));
    auto instantiated_type = make_function_data_type(
      {{"x", integer_data_type::get_instance()}},
      integer_data_type::get_instance());
    auto h = std::make_shared<function>(location::global(), "h");
    top_scope.add(h);
    auto h_definition = std::make_shared<function_definition>(
      location::global(), h, make_function_data_type(
      {{"f", generic_type}, {"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0)));
    top_scope.add(h_definition);
    auto overload = find_overload(*h,
      {{"f", generic_type}, {"x", integer_data_type::get_instance()}},
      top_scope);
    REQUIRE(overload == h_definition);
  }
}

TEST_CASE("test_overloaded_function_parameter", "[function_overloads]") {
  auto top_scope = make_builtin_scope();
  auto f = std::make_shared<function>(location::global(), "f");
  top_scope->add(f);
  auto t1 = make_function_data_type(
    {{"f", make_function_data_type({}, bool_data_type::get_instance())}},
    bool_data_type::get_instance());
  auto d1 = std::make_shared<function_definition>(location::global(), f, t1);
  top_scope->add(d1);
  auto g = std::make_shared<function>(location::global(), "g");
  top_scope->add(g);
  auto t2 = make_function_data_type({}, bool_data_type::get_instance());
  auto d2 = std::make_shared<function_definition>(location::global(), g, t2);
  top_scope->add(d2);
  auto t3 = make_function_data_type({}, integer_data_type::get_instance());
  auto d3 = std::make_shared<function_definition>(location::global(), g, t3);
  top_scope->add(d3);
  auto callable = std::make_shared<callable_data_type>(g);
  auto overload = find_overload(*f, {{"f", callable}}, *top_scope);
  REQUIRE(overload == d1);
}
