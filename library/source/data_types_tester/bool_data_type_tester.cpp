#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bool_name", "[bool_data_type]") {
  REQUIRE(bool_data_type().get_name() == "Bool");
}

TEST_CASE("test_bool_equality", "[bool_data_type]") {
  REQUIRE(bool_data_type() == bool_data_type());
  REQUIRE(*bool_data_type::get_instance() == *bool_data_type::get_instance());
  REQUIRE(*bool_data_type::get_instance() !=
    *integer_data_type::get_instance());
}
