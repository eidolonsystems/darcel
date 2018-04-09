#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/semantics/function.hpp"

using namespace darcel;

TEST_CASE("test_function", "[function]") {
  auto f = std::make_shared<function>(std::make_shared<variable>(
    location::global(), "f", make_function_data_type(
    {{"a", integer_data_type::get_instance()}},
    integer_data_type::get_instance())));
}

TEST_CASE("test_generic_function_parameter_overload", "[function]") {
  SECTION("Valid substitution.") {
    auto generic_type = make_function_data_type(
      {{"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0));
    auto instantiated_type = make_function_data_type(
      {{"x", integer_data_type::get_instance()}},
      integer_data_type::get_instance());
    auto h = std::make_shared<function>(std::make_shared<variable>(
      location::global(), "h", make_function_data_type({{"f", generic_type}},
      integer_data_type::get_instance())));
    auto f = std::make_shared<variable>(location::global(), "f",
      instantiated_type);
    auto overload = find_overload(*h, {{"f", f->get_data_type()}});
    REQUIRE(overload != nullptr);
    REQUIRE(*overload->get_data_type() == *make_function_data_type(
      {{"f", instantiated_type}}, integer_data_type::get_instance()));
  }
  SECTION("Invalid substitution.") {
    auto generic_type = make_function_data_type(
      {{"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0));
    auto instantiated_type = make_function_data_type(
      {{"x", integer_data_type::get_instance()}},
      bool_data_type::get_instance());
    auto h = std::make_shared<function>(std::make_shared<variable>(
      location::global(), "h", make_function_data_type({{"f", generic_type}},
      integer_data_type::get_instance())));
    auto f = std::make_shared<variable>(location::global(), "f",
      instantiated_type);
    auto overload = find_overload(*h, {{"f", f->get_data_type()}});
    REQUIRE(overload == nullptr);
  }
}

TEST_CASE("test_two_generic_function_parameters_overload", "[function]") {
  SECTION("Valid substitution.") {
    auto generic_type = make_function_data_type(
      {{"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0));
    auto instantiated_type = make_function_data_type(
      {{"x", integer_data_type::get_instance()}},
      integer_data_type::get_instance());
    auto h = std::make_shared<function>(std::make_shared<variable>(
      location::global(), "h", make_function_data_type(
      {{"f", generic_type}, {"x", make_generic_data_type("`T", 0)}},
      make_generic_data_type("`T", 0))));
    auto f = std::make_shared<variable>(location::global(), "f",
      generic_type);
    auto overload = find_overload(*h,
      {{"f", f->get_data_type()}, {"x", integer_data_type::get_instance()}});
    REQUIRE(overload != nullptr);
    REQUIRE(*overload->get_data_type() == *make_function_data_type(
      {{"f", instantiated_type}, {"x", integer_data_type::get_instance()}},
      integer_data_type::get_instance()));
  }
}
