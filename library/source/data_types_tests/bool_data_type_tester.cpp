#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bool_name", "[BoolDataType]") {
  REQUIRE(BoolDataType().get_name() == "Bool");
}

TEST_CASE("test_bool_equality", "[BoolDataType]") {
  REQUIRE(BoolDataType() == BoolDataType());
  REQUIRE(*BoolDataType::get_instance() == *BoolDataType::get_instance());
  REQUIRE(*BoolDataType::get_instance() !=
    *IntegerDataType::get_instance());
}
