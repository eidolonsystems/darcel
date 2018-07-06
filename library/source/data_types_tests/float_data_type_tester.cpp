#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/float_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_float_name", "[float_data_type]") {
  REQUIRE(float_data_type().get_name() == "Float");
}

TEST_CASE("test_float_equality", "[float_data_type]") {
  REQUIRE(float_data_type() == float_data_type());
  REQUIRE(*float_data_type::get_instance() ==
    *float_data_type::get_instance());
  REQUIRE(*float_data_type::get_instance() !=
    *bool_data_type::get_instance());
}
