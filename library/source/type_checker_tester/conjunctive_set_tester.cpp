#include <catch.hpp>
#include "darcel/type_checker/conjunctive_set.hpp"

using namespace darcel;

TEST_CASE("test_empty_conjunctive_set", "[conjunctive_set]") {
  conjunctive_set s;
  type_map m;
  REQUIRE(s.is_satisfied(m));
}
