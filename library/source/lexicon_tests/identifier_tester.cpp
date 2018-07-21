#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/identifier.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_identifier_stream", "[Identifier]") {
  Identifier id("hello");
  stringstream ss;
  ss << id;
  REQUIRE(ss.str() == "hello");
}

TEST_CASE("test_identifier_equality", "[Identifier]") {
  Identifier id1("hello");
  Identifier id2("goodbye");
  REQUIRE(id1 == id1);
  REQUIRE(id1 != id2);
}

TEST_CASE("test_parse_identifier", "[Identifier]") {
  REQUIRE(parse_identifier("abc") == std::string("abc"));
  REQUIRE(parse_identifier("_abc") == std::string("_abc"));
  REQUIRE(parse_identifier("abc_") == std::string("abc_"));
  REQUIRE(parse_identifier("a_bc") == std::string("a_bc"));
  REQUIRE(parse_identifier("5abc") == std::nullopt);
  REQUIRE(parse_identifier(".abc") == std::nullopt);
  REQUIRE(parse_identifier("+abc") == std::nullopt);
  REQUIRE(parse_identifier("abc5") == std::string("abc5"));
  REQUIRE(parse_identifier("ab5c") == std::string("ab5c"));
  REQUIRE(parse_identifier("abc+") == std::string("abc"));
  REQUIRE(parse_identifier("abc.") == std::string("abc"));
}
