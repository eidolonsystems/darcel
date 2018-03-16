#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/semantics/variable.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_variable", "[variable]") {
  variable v(location::global(), "abc", integer_data_type::get_instance());
  REQUIRE(v.get_name() == "abc");
  REQUIRE(*v.get_data_type() == integer_data_type());
}
