#include <sstream>
#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/lexicon/literal.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_literal_stream", "[Literal]") {
  Literal l("5", IntegerDataType::get_instance());
  stringstream ss;
  ss << l;
  REQUIRE(ss.str() == "5");
}

TEST_CASE("test_literal_equality", "[Literal]") {
  Literal i1("5", IntegerDataType::get_instance());
  Literal i2("5", IntegerDataType::get_instance());
  Literal f1("3.14", FloatDataType::get_instance());
  REQUIRE(i1 == i1);
  REQUIRE(i1 == i2);
  REQUIRE(i1 != f1);
}

TEST_CASE("test_parse_literal", "[Literal]") {
  SECTION("Valid Literals") {
    REQUIRE(parse_literal("true") ==
      Literal("true", BoolDataType::get_instance()));
    REQUIRE(parse_literal("false") ==
      Literal("false", BoolDataType::get_instance()));
    REQUIRE(parse_literal("1") ==
      Literal("1", IntegerDataType::get_instance()));
    REQUIRE(parse_literal("1.1") ==
      Literal("1.1", FloatDataType::get_instance()));
  }
  SECTION("Literal Delimiters") {
    REQUIRE(parse_literal("trues") == nullopt);
    REQUIRE(parse_literal("true5") == nullopt);
    REQUIRE(parse_literal("true.") ==
      Literal("true", BoolDataType::get_instance()));
    REQUIRE(parse_literal("true+") ==
      Literal("true", BoolDataType::get_instance()));
    REQUIRE(parse_literal("5.5.") ==
      Literal("5.5", FloatDataType::get_instance()));
  }
  SECTION("Invalid Literals") {
    REQUIRE(parse_literal("abc") == nullopt);
  }
}

TEST_CASE("test_parse_string", "[Literal]") {
  REQUIRE(parse_literal("\"abc\"") ==
    Literal("abc", TextDataType::get_instance()));
  REQUIRE(parse_literal("\"a\\nc\"") ==
    Literal("a\nc", TextDataType::get_instance()));
}
