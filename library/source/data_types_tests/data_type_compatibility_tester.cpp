#include <catch.hpp>
#include "darcel/data_types/data_type_compatibility.hpp"

using namespace darcel;

TEST_CASE("test_equal_compatibility", "[data_type_compatibility]") {
  scope s;
  REQUIRE(get_compatibility(float_data_type(), float_data_type(), s) ==
    data_type_compatibility::EQUAL);
  REQUIRE(get_compatibility(integer_data_type(), float_data_type(), s) ==
    data_type_compatibility::NONE);
}

TEST_CASE("test_overload_compatibility", "[data_type_compatibility]") {
  scope s;
  auto f = std::make_shared<function>(location::global(), "f");
  s.add(f);
  auto t1 = make_function_data_type({}, integer_data_type::get_instance());
  auto d1 = std::make_shared<function_definition>(location::global(), f, t1);
  s.add(d1);
  auto t2 = make_function_data_type({{"x", bool_data_type::get_instance()}},
    integer_data_type::get_instance());
  auto d2 = std::make_shared<function_definition>(location::global(), f, t2);
  auto t3 = make_function_data_type({{"x", float_data_type::get_instance()}},
    integer_data_type::get_instance());
  s.add(d2);
  auto c1 = std::make_shared<callable_data_type>(f);
  REQUIRE(get_compatibility(*c1, *t1, s) ==
    data_type_compatibility::SUPER_TYPE);
  REQUIRE(get_compatibility(*c1, *t2, s) ==
    data_type_compatibility::SUPER_TYPE);
  REQUIRE(get_compatibility(*c1, *t3, s) == data_type_compatibility::NONE);
}
