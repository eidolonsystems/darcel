#include <catch.hpp>
#include "darcel/data_types/data_type_compatibility.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_equal_compatibility", "[data_type_compatibility]") {
  REQUIRE(get_compatibility(float_data_type(), float_data_type()) ==
    data_type_compatibility::EQUAL);
  REQUIRE(get_compatibility(integer_data_type(), float_data_type()) ==
    data_type_compatibility::NONE);
}
