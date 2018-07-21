#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/float_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_float_name", "[FloatDataType]") {
  REQUIRE(FloatDataType().get_name() == "Float");
}

TEST_CASE("test_float_equality", "[FloatDataType]") {
  REQUIRE(FloatDataType() == FloatDataType());
  REQUIRE(*FloatDataType::get_instance() ==
    *FloatDataType::get_instance());
  REQUIRE(*FloatDataType::get_instance() !=
    *BoolDataType::get_instance());
}
