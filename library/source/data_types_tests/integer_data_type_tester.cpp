#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_integer_name", "[IntegerDataType]") {
  REQUIRE(IntegerDataType().get_name() == "Int");
}

TEST_CASE("test_integer_equality", "[IntegerDataType]") {
  REQUIRE(IntegerDataType() == IntegerDataType());
  REQUIRE(*IntegerDataType::get_instance() ==
    *IntegerDataType::get_instance());
  REQUIRE(*IntegerDataType::get_instance() !=
    *BoolDataType::get_instance());
}
