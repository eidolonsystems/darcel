#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/terminal.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_terminal_stream", "[Terminal]") {
  SECTION("New line") {
    Terminal t(Terminal::Type::NEW_LINE);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "\n");
  }
  SECTION("End of file") {
    Terminal t(Terminal::Type::END_OF_FILE);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str().empty());
  }
}

TEST_CASE("test_terminal_equality", "[Terminal]") {
  Terminal t1(Terminal::Type::END_OF_FILE);
  Terminal t2(Terminal::Type::NEW_LINE);
  REQUIRE(t1 == t1);
  REQUIRE(t1 == Terminal::Type::END_OF_FILE);
  REQUIRE(t1 != t2);
  REQUIRE(t2 == Terminal::Type::NEW_LINE);
}

TEST_CASE("test_parse_terminal", "[Terminal]") {
  SECTION("Valid Terminal") {
    auto s = "\0";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      Terminal::Type::END_OF_FILE);
    REQUIRE(parse_terminal("\n") == Terminal::Type::NEW_LINE);
  }
  SECTION("Delimiters") {
    auto s = "\09";
    REQUIRE(parse_terminal(LexicalIterator(s, 2)) ==
      Terminal::Type::END_OF_FILE);
    s = "\0+";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      Terminal::Type::END_OF_FILE);
    s = "\0:";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      Terminal::Type::END_OF_FILE);
    s = "\0,";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      Terminal::Type::END_OF_FILE);
    s = "\0a";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      Terminal::Type::END_OF_FILE);
  }
  SECTION("Invalid Terminal") {
    REQUIRE(parse_terminal("abc") == std::nullopt);
  }
}
