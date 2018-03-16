#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/text_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_text_name", "[text_data_type]") {
  REQUIRE(text_data_type().get_name() == "Text");
}

TEST_CASE("test_text_equality", "[text_data_type]") {
  REQUIRE(text_data_type() == text_data_type());
  REQUIRE(*text_data_type::get_instance() ==
    *text_data_type::get_instance());
  REQUIRE(*text_data_type::get_instance() !=
    *bool_data_type::get_instance());
}
