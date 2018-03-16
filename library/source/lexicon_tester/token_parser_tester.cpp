#include <catch.hpp>
#include "darcel/lexicon/token_parser.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_keyword_token_parser", "[token_parser]") {
  token_parser parser;
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

TEST_CASE("test_identifier_token_parser", "[token_parser]") {
  token_parser parser;
  parser.feed("abc ");
  {
    auto t = parser.parse_token();
    REQUIRE(t.has_value());
  }
}

TEST_CASE("test_spaces_token_parser", "[token_parser]") {
  token_parser parser;
  parser.feed("   \nlet  \n  \r\n   let ");
  {
    auto t = parser.parse_token();
    REQUIRE(t.has_value());
    REQUIRE(match(*t, keyword(keyword::word::LET)));
    REQUIRE(t->get_line_number() == 1);
    REQUIRE(t->get_column_number() == 0);
  }
  {
    auto t = parser.parse_token();
    REQUIRE(t.has_value());
    REQUIRE(match(*t, terminal::type::new_line));
    REQUIRE(t->get_line_number() == 1);
    REQUIRE(t->get_column_number() == 5);
  }
  {
    auto t = parser.parse_token();
    REQUIRE(t.has_value());
    REQUIRE(match(*t, keyword(keyword::word::LET)));
    REQUIRE(t->get_line_number() == 3);
    REQUIRE(t->get_column_number() == 3);
  }
}

TEST_CASE("test_line_continuations", "[token_parser]") {
  SECTION("Operator continuation.") {
    token_parser parser;
    parser.feed("5 +\n5\0");
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<literal>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<operation>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<literal>(&t->get_instance()) != nullptr);
    }
  }
  SECTION("Round bracket continuation.") {
    token_parser parser;
    parser.feed("5 +(\n6\n)7\0");
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<literal>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<operation>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<bracket>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<literal>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<bracket>(&t->get_instance()) != nullptr);
    }
    {
      auto t = parser.parse_token();
      REQUIRE(t.has_value());
      REQUIRE(std::get_if<literal>(&t->get_instance()) != nullptr);
    }
  }
}
