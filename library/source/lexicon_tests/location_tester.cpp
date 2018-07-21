#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/location.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_location_equality", "[Location]") {
  Location l1("a", 1, 1);
  REQUIRE(l1 == l1);
  REQUIRE(!(l1 != l1));
  Location l2("a", 1, 2);
  REQUIRE(l1 != l2);
  REQUIRE(!(l1 == l2));
  Location l3("a", 2, 1);
  REQUIRE(l1 != l3);
  REQUIRE(!(l1 == l3));
  Location l4("b", 1, 1);
  REQUIRE(l1 != l4);
  REQUIRE(!(l1 == l4));
  Location l5("a", 1, 1);
  REQUIRE(l1 == l5);
  REQUIRE(!(l1 != l5));
}

TEST_CASE("test_location_stream", "[Location]") {
  Location l("/home/darcel/main.dcl", 5, 12);
  stringstream ss;
  ss << l;
  REQUIRE(ss.str() == "/home/darcel/main.dcl:5:12");
}
