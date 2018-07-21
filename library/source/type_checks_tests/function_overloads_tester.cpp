#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/type_checks/function_overloads.hpp"

using namespace darcel;

TEST_CASE("test_single_parameter_function_overload", "[function_overloads]") {
  SECTION("No matching overload.") {
    scope top_scope;
    auto f = std::make_shared<function>(Location::global(), "f");
    top_scope.add(f);
    auto definition = std::make_shared<function_definition>(
      Location::global(), f,
      make_function_data_type({{"a", IntegerDataType::get_instance()}},
      IntegerDataType::get_instance()));
    top_scope.add(definition);
    auto overload = find_overload(*f,
      {{"a", BoolDataType::get_instance()}}, top_scope);
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
      {{"x", IntegerDataType::get_instance()}},
      IntegerDataType::get_instance());
    auto h = std::make_shared<function>(Location::global(), "h");
    top_scope.add(h);
    auto h_definition = std::make_shared<function_definition>(
      Location::global(), h, make_function_data_type({{"f", generic_type}},
      IntegerDataType::get_instance()));
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
      {{"x", IntegerDataType::get_instance()}},
      BoolDataType::get_instance());
    auto h = std::make_shared<function>(Location::global(), "h");
    top_scope.add(h);
    auto h_definition = std::make_shared<function_definition>(
      Location::global(), h, make_function_data_type({{"f", generic_type}},
      IntegerDataType::get_instance()));
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
      {{"x", IntegerDataType::get_instance()}},
      IntegerDataType::get_instance());
    auto h = std::make_shared<function>(Location::global(), "h");
    top_scope.add(h);
    auto h_definition = std::make_shared<function_definition>(
      Location::global(), h, make_function_data_type(
      {{"f", generic_type}, {"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0)));
    top_scope.add(h_definition);
    auto overload = find_overload(*h,
      {{"f", generic_type}, {"x", IntegerDataType::get_instance()}},
      top_scope);
    REQUIRE(overload == h_definition);
  }
}

TEST_CASE("test_overloaded_function_parameter", "[function_overloads]") {
  auto top_scope = make_builtin_scope();
  auto f = std::make_shared<function>(Location::global(), "f");
  top_scope->add(f);
  auto t1 = make_function_data_type(
    {{"f", make_function_data_type({}, BoolDataType::get_instance())}},
    BoolDataType::get_instance());
  auto d1 = std::make_shared<function_definition>(Location::global(), f, t1);
  top_scope->add(d1);
  auto g = std::make_shared<function>(Location::global(), "g");
  top_scope->add(g);
  auto t2 = make_function_data_type({}, BoolDataType::get_instance());
  auto d2 = std::make_shared<function_definition>(Location::global(), g, t2);
  top_scope->add(d2);
  auto t3 = make_function_data_type({}, IntegerDataType::get_instance());
  auto d3 = std::make_shared<function_definition>(Location::global(), g, t3);
  top_scope->add(d3);
  auto callable = std::make_shared<CallableDataType>(g);
  auto overload = find_overload(*f, {{"f", callable}}, *top_scope);
  REQUIRE(overload == d1);
}
