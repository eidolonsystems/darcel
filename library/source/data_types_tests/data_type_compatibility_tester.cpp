#include <catch.hpp>
#include "darcel/data_types/data_type_compatibility.hpp"

using namespace darcel;

TEST_CASE("test_equal_compatibility", "[DataTypeCompatibility]") {
  scope s;
  REQUIRE(get_compatibility(FloatDataType(), FloatDataType(), s) ==
    DataTypeCompatibility::EQUAL);
  REQUIRE(get_compatibility(IntegerDataType(), FloatDataType(), s) ==
    DataTypeCompatibility::NONE);
}

TEST_CASE("test_overload_compatibility", "[DataTypeCompatibility]") {
  scope s;
  auto f = std::make_shared<function>(Location::global(), "f");
  s.add(f);
  auto t1 = make_function_data_type({}, IntegerDataType::get_instance());
  auto d1 = std::make_shared<function_definition>(Location::global(), f, t1);
  s.add(d1);
  auto t2 = make_function_data_type({{"x", BoolDataType::get_instance()}},
    IntegerDataType::get_instance());
  auto d2 = std::make_shared<function_definition>(Location::global(), f, t2);
  auto t3 = make_function_data_type({{"x", FloatDataType::get_instance()}},
    IntegerDataType::get_instance());
  s.add(d2);
  auto c1 = std::make_shared<CallableDataType>(f);
  REQUIRE(get_compatibility(*c1, *t1, s) ==
    DataTypeCompatibility::SUPER_TYPE);
  REQUIRE(get_compatibility(*c1, *t2, s) ==
    DataTypeCompatibility::SUPER_TYPE);
  REQUIRE(get_compatibility(*c1, *t3, s) == DataTypeCompatibility::NONE);
}
