#include <catch.hpp>
#include "darcel/semantics/function.hpp"

using namespace darcel;

TEST_CASE("test_function", "[function]") {
  function f(Location::global(), "f");
  REQUIRE(f.get_location() == Location::global());
  REQUIRE(f.get_name() == "f");
}
