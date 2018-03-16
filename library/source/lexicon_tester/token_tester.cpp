#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/token.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_token_stream", "[token]") {
  SECTION("Keyword") {
    token t(keyword::word::LET, 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "let");
  }
  SECTION("Punctuation") {
    token t(punctuation::mark::DOT, 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == ".");
  }
  SECTION("Operation") {
    token t(operation::symbol::LESS_OR_EQUAL, 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "<=");
  }
  SECTION("Identifier") {
    token t(identifier("hello"), 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "hello");
  }
  SECTION("Literal") {
    token t(literal("123", integer_data_type::get_instance()), 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "123");
  }
  SECTION("Terminal") {
    token t(terminal::type::end_of_file, 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str().empty());
  }
}
