#include <catch.hpp>
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_generic_name", "[generic_data_type]") {
  REQUIRE(generic_data_type(location::global(), "`T", 0).get_name() == "`T");
}

TEST_CASE("test_generic_equality", "[generic_data_type]") {
  REQUIRE(generic_data_type(location::global(), "`T", 0) !=
    generic_data_type(location::global(), "`T", 1));
  REQUIRE(generic_data_type(location::global(), "`A", 2) ==
    generic_data_type(location::global(), "`B", 2));
  REQUIRE(generic_data_type(location::global(), "`A", 2) !=
    integer_data_type());
}
