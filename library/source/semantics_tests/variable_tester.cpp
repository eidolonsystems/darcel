#include <catch.hpp>
#include "darcel/semantics/variable.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_variable", "[variable]") {
  variable v(Location::global(), "abc");
  REQUIRE(v.get_location() == Location::global());
  REQUIRE(v.get_name() == "abc");
}
