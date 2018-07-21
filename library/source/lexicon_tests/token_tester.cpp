#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/token.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_token_stream", "[Token]") {
  SECTION("Keyword") {
    Token t(Keyword::Word::LET, 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "let");
  }
  SECTION("Punctuation") {
    Token t(Punctuation::Mark::DOT, 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == ".");
  }
  SECTION("Operation") {
    Token t(Operation::Symbol::LESS_OR_EQUAL, 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "<=");
  }
  SECTION("Identifier") {
    Token t(Identifier("hello"), 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "hello");
  }
  SECTION("Literal") {
    Token t(Literal("123", IntegerDataType::get_instance()), 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "123");
  }
  SECTION("Terminal") {
    Token t(Terminal::Type::END_OF_FILE, 0, 0);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str().empty());
  }
}
