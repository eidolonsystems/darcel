#include <sstream>
#include <catch.hpp>
#include "darcel/operations/pretty_print.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_pretty_print_let", "[pretty_print]") {
  SECTION("Int literal") {
    std::stringstream ss;
    auto l = std::make_unique<literal_expression>(location::global(),
      literal("123", integer_data_type::get_instance()));
    pretty_print(*l, ss);
    REQUIRE(ss.str() == "123");
  }
  SECTION("Bool literal") {
    std::stringstream ss;
    auto l = std::make_unique<literal_expression>(location::global(),
      literal("true", bool_data_type::get_instance()));
    pretty_print(*l, ss);
    REQUIRE(ss.str() == "true");
  }
  SECTION("String literal") {
    std::stringstream ss;
    auto l = std::make_unique<literal_expression>(location::global(),
      literal("hello", text_data_type::get_instance()));
    pretty_print(*l, ss);
    REQUIRE(ss.str() == "\"hello\"");
  }
}
