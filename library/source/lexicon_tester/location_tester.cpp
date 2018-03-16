#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/location.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_location_stream", "[location]") {
  location l("/home/darcel/main.dcl", 5, 12);
  stringstream ss;
  ss << l;
  REQUIRE(ss.str() == "/home/darcel/main.dcl:5:12");
}
