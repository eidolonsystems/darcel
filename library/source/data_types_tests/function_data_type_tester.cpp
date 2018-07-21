#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_function_name", "[FunctionDataType]") {
  REQUIRE(FunctionDataType({}, BoolDataType::get_instance()).get_name() ==
    "() -> Bool");
  REQUIRE(FunctionDataType(
    {{"a", BoolDataType::get_instance()}},
    BoolDataType::get_instance()).get_name() == "(a: Bool) -> Bool");
  REQUIRE(FunctionDataType(
    {{"", BoolDataType::get_instance()}},
    BoolDataType::get_instance()).get_name() == "(Bool) -> Bool");
  REQUIRE(FunctionDataType(
    {{"a", BoolDataType::get_instance()},
     {"b", IntegerDataType::get_instance()}},
    BoolDataType::get_instance()).get_name() == "(a: Bool, b: Int) -> Bool");
}

TEST_CASE("test_function_equality", "[FunctionDataType]") {
  FunctionDataType f1({}, BoolDataType::get_instance());
  FunctionDataType f2({{"a", BoolDataType::get_instance()}},
    BoolDataType::get_instance());
  FunctionDataType f3({{"b", BoolDataType::get_instance()}},
    BoolDataType::get_instance());
  FunctionDataType f4({{"b", IntegerDataType::get_instance()}},
    BoolDataType::get_instance());
  FunctionDataType f5({{"b", BoolDataType::get_instance()}},
    IntegerDataType::get_instance());
  REQUIRE(f1 == f1);
  REQUIRE(f2 == f2);
  REQUIRE(f3 == f3);
  REQUIRE(f4 == f4);
  REQUIRE(f5 == f5);
  REQUIRE(f1 != f2);
  REQUIRE(f2 == f3);
  REQUIRE(f3 != f4);
  REQUIRE(f4 != f5);
}
