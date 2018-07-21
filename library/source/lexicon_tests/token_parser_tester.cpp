#include <catch.hpp>
#include "darcel/lexicon/token_parser.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_keyword_token_parser", "[TokenParser]") {
  TokenParser parser;
  parser.feed("return");
  {
    auto t = parser.parse_token();
    REQUIRE(!t.has_value());
  }
  parser.feed("\n");
  {
    auto t = parser.parse_token();
    REQUIRE(t.has_value());
  }
}

TEST_CASE("test_identifier_token_parser", "[TokenParser]") {
  TokenParser parser;
  parser.feed("abc ");
  {
    auto t = parser.parse_token();
    REQUIRE(t.has_value());
  }
}

TEST_CASE("test_spaces_token_parser", "[TokenParser]") {
  TokenParser parser;
  parser.feed("   \nlet  \n  \r\n   let ");
  {
    auto t = parser.parse_token();
    REQUIRE(t.has_value());
    REQUIRE(match(*t, Keyword(Keyword::Word::LET)));
    REQUIRE(t->get_line_number() == 2);
    REQUIRE(t->get_column_number() == 1);
  }
  {
    auto t = parser.parse_token();
    REQUIRE(t.has_value());
    REQUIRE(match(*t, Terminal::Type::NEW_LINE));
    REQUIRE(t->get_line_number() == 2);
    REQUIRE(t->get_column_number() == 6);
  }
  {
    auto t = parser.parse_token();
    REQUIRE(t.has_value());
    REQUIRE(match(*t, Keyword(Keyword::Word::LET)));
    REQUIRE(t->get_line_number() == 4);
    REQUIRE(t->get_column_number() == 4);
  }
}

TEST_CASE("test_line_continuations", "[TokenParser]") {
  SECTION("Operator continuation.") {
    TokenParser parser;
    parser.feed("5 +\n5\0");
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<Literal>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<Operation>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<Literal>(&t->get_instance()) != nullptr);
    }
  }
  SECTION("Round bracket continuation.") {
    TokenParser parser;
    parser.feed("5 +(\n6\n)7\0");
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<Literal>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<Operation>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<Bracket>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<Literal>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<Bracket>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<Literal>(&t->get_instance()) != nullptr);
    }
  }
}
