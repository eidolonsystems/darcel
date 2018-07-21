#include <sstream>
#include <catch.hpp>
#include "darcel/lexicon/terminal.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_terminal_stream", "[terminal]") {
  SECTION("New line") {
    terminal t(terminal::type::new_line);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str() == "\n");
  }
  SECTION("End of file") {
    terminal t(terminal::type::end_of_file);
    stringstream ss;
    ss << t;
    REQUIRE(ss.str().empty());
  }
}

TEST_CASE("test_terminal_equality", "[terminal]") {
  terminal t1(terminal::type::end_of_file);
  terminal t2(terminal::type::new_line);
  REQUIRE(t1 == t1);
  REQUIRE(t1 == terminal::type::end_of_file);
  REQUIRE(t1 != t2);
  REQUIRE(t2 == terminal::type::new_line);
}

TEST_CASE("test_parse_terminal", "[terminal]") {
  SECTION("Valid Terminal") {
    auto s = "\0";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      terminal::type::end_of_file);
    REQUIRE(parse_terminal("\n") == terminal::type::new_line);
  }
  SECTION("Delimiters") {
    auto s = "\09";
    REQUIRE(parse_terminal(LexicalIterator(s, 2)) ==
      terminal::type::end_of_file);
    s = "\0+";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      terminal::type::end_of_file);
    s = "\0:";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      terminal::type::end_of_file);
    s = "\0,";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      terminal::type::end_of_file);
    s = "\0a";
    REQUIRE(parse_terminal(LexicalIterator(s, 1)) ==
      terminal::type::end_of_file);
  }
  SECTION("Invalid Terminal") {
    REQUIRE(parse_terminal("abc") == std::nullopt);
  }
}
