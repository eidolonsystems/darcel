#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/text_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_text_name", "[TextDataType]") {
  REQUIRE(TextDataType().get_name() == "Text");
}

TEST_CASE("test_text_equality", "[TextDataType]") {
  REQUIRE(TextDataType() == TextDataType());
  REQUIRE(*TextDataType::get_instance() ==
    *TextDataType::get_instance());
  REQUIRE(*TextDataType::get_instance() !=
    *BoolDataType::get_instance());
}
