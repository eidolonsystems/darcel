#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_integer_name", "[integer_data_type]") {
  REQUIRE(integer_data_type().get_name() == "Int");
}

TEST_CASE("test_integer_equality", "[integer_data_type]") {
  REQUIRE(integer_data_type() == integer_data_type());
  REQUIRE(*integer_data_type::get_instance() ==
    *integer_data_type::get_instance());
  REQUIRE(*integer_data_type::get_instance() !=
    *bool_data_type::get_instance());
}
