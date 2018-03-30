#include <catch.hpp>
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_generic_name", "[generic_data_type]") {
  REQUIRE(generic_data_type(location::global(), "T").get_name() == "T");
}

TEST_CASE("test_generic_equality", "[generic_data_type]") {
  REQUIRE(generic_data_type(location::global(), "T") !=
    generic_data_type(location::global(), "T"));
  generic_data_type t(location::global(), "T");
  REQUIRE(t == t);
  REQUIRE(t != integer_data_type());
}
