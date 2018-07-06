#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/location.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_location_equality", "[location]") {
  location l1("a", 1, 1);
  REQUIRE(l1 == l1);
  REQUIRE(!(l1 != l1));
  location l2("a", 1, 2);
  REQUIRE(l1 != l2);
  REQUIRE(!(l1 == l2));
  location l3("a", 2, 1);
  REQUIRE(l1 != l3);
  REQUIRE(!(l1 == l3));
  location l4("b", 1, 1);
  REQUIRE(l1 != l4);
  REQUIRE(!(l1 == l4));
  location l5("a", 1, 1);
  REQUIRE(l1 == l5);
  REQUIRE(!(l1 != l5));
}

TEST_CASE("test_location_stream", "[location]") {
  location l("/home/darcel/main.dcl", 5, 12);
  stringstream ss;
  ss << l;
  REQUIRE(ss.str() == "/home/darcel/main.dcl:5:12");
}
