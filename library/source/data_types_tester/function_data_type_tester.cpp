#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_function_name", "[function_data_type]") {
  REQUIRE(function_data_type({}, bool_data_type::get_instance()).get_name() ==
    "() -> Bool");
  REQUIRE(function_data_type(
    {{"a", bool_data_type::get_instance()}},
    bool_data_type::get_instance()).get_name() == "(a: Bool) -> Bool");
  REQUIRE(function_data_type(
    {{"", bool_data_type::get_instance()}},
    bool_data_type::get_instance()).get_name() == "(Bool) -> Bool");
  REQUIRE(function_data_type(
    {{"a", bool_data_type::get_instance()},
     {"b", integer_data_type::get_instance()}},
    bool_data_type::get_instance()).get_name() == "(a: Bool, b: Int) -> Bool");
}

TEST_CASE("test_function_equality", "[function_data_type]") {
  function_data_type f1({}, bool_data_type::get_instance());
  function_data_type f2({{"a", bool_data_type::get_instance()}},
    bool_data_type::get_instance());
  function_data_type f3({{"b", bool_data_type::get_instance()}},
    bool_data_type::get_instance());
  function_data_type f4({{"b", integer_data_type::get_instance()}},
    bool_data_type::get_instance());
  function_data_type f5({{"b", bool_data_type::get_instance()}},
    integer_data_type::get_instance());
  REQUIRE(f1 == f1);
  REQUIRE(f2 == f2);
  REQUIRE(f3 == f3);
  REQUIRE(f4 == f4);
  REQUIRE(f5 == f5);
  REQUIRE(f1 != f2);
  REQUIRE(f2 == f3);
  REQUIRE(f3 != f4);
  REQUIRE(f4 != f5);
}
