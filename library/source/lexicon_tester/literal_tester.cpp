#include <sstream>
#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/lexicon/literal.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_literal_stream", "[literal]") {
  literal l("5", integer_data_type::get_instance());
  stringstream ss;
  ss << l;
  REQUIRE(ss.str() == "5");
}

TEST_CASE("test_literal_equality", "[literal]") {
  literal i1("5", integer_data_type::get_instance());
  literal i2("5", integer_data_type::get_instance());
  literal f1("3.14", float_data_type::get_instance());
  REQUIRE(i1 == i1);
  REQUIRE(i1 == i2);
  REQUIRE(i1 != f1);
}

TEST_CASE("test_parse_literal", "[literal]") {
  SECTION("Valid Literals") {
    REQUIRE(parse_literal("true") ==
      literal("true", bool_data_type::get_instance()));
    REQUIRE(parse_literal("false") ==
      literal("false", bool_data_type::get_instance()));
    REQUIRE(parse_literal("1") ==
      literal("1", integer_data_type::get_instance()));
    REQUIRE(parse_literal("1.1") ==
      literal("1.1", float_data_type::get_instance()));
  }
  SECTION("Literal Delimiters") {
    REQUIRE(parse_literal("trues") == nullopt);
    REQUIRE(parse_literal("true5") == nullopt);
    REQUIRE(parse_literal("true.") ==
      literal("true", bool_data_type::get_instance()));
    REQUIRE(parse_literal("true+") ==
      literal("true", bool_data_type::get_instance()));
    REQUIRE(parse_literal("5.5.") ==
      literal("5.5", float_data_type::get_instance()));
  }
  SECTION("Invalid Literals") {
    REQUIRE(parse_literal("abc") == nullopt);
  }
}

TEST_CASE("test_parse_string", "[literal]") {
  REQUIRE(parse_literal("\"abc\"") ==
    literal("abc", text_data_type::get_instance()));
  REQUIRE(parse_literal("\"a\\nc\"") ==
    literal("a\nc", text_data_type::get_instance()));
}
