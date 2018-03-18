#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/lexicon/token_parser.hpp"
#include "darcel/syntax/syntax_parser.hpp"

using namespace darcel;
using namespace std;

namespace {
  template<std::size_t N>
  void incremental_feed(syntax_parser& p, const char (&s)[N]) {
    token_parser t;
    t.feed(s);
    while(auto token = t.parse_token()) {
      if(match(*token, terminal::type::end_of_file)) {
        p.feed(std::move(*token));
        break;
      }
      p.feed(std::move(*token));
    }
  }

  template<std::size_t N>
  void feed(syntax_parser& p, const char (&s)[N]) {
    char q[N + 1];
    std::copy(begin(s), end(s), begin(q));
    q[N] = '\0';
    incremental_feed(p, q);
  }
}

TEST_CASE("test_parsing_terminal", "[syntax_parser]") {
  syntax_parser p;
  feed(p, "");
  auto expression = p.parse_node();
  REQUIRE(dynamic_cast<terminal_node*>(expression.get()) != nullptr);
}

TEST_CASE("test_parsing_term", "[syntax_parser]") {
  SECTION("Parse identifiers.") {
    syntax_parser p;
    feed(p, "hello world");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse identifiers with new lines.") {
    syntax_parser p;
    feed(p, "hello\nworld");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse brackets.") {
    syntax_parser p;
    feed(p, "(()()()())");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse brackets with new lines.") {
    syntax_parser p;
    feed(p, "(\n5)");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse operators.") {
    syntax_parser p;
    feed(p, "5 + 5");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse operators with new line.") {
    syntax_parser p;
    feed(p, "5 +\n 5");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse colon.") {
    syntax_parser p;
    feed(p, "def x:5 6\n 7 end def y: 3 1 end");
    auto c = p.get_next_terminal();
  }
}

TEST_CASE("test_parsing_literal_expression", "[syntax_parser]") {
  syntax_parser p;
  feed(p, "123");
  auto expression = p.parse_node();
  auto literal = dynamic_cast<literal_expression*>(expression.get());
  REQUIRE(literal != nullptr);
  REQUIRE(literal->get_literal().get_value() == "123");
  REQUIRE(*literal->get_literal().get_type() == integer_data_type());
}

TEST_CASE("test_parsing_bind_variable_statement", "[syntax_parser]") {
  syntax_parser p;
  feed(p, "let x = 321");
  auto expression = p.parse_node();
  auto bind = dynamic_cast<bind_variable_statement*>(expression.get());
  REQUIRE(bind != nullptr);
  REQUIRE(bind->get_variable()->get_name() == "x");
  auto initializer = dynamic_cast<const literal_expression*>(
    &bind->get_expression());
  REQUIRE(initializer->get_literal().get_value() == "321");
  REQUIRE(*initializer->get_literal().get_type() == integer_data_type());
}

TEST_CASE("test_parsing_variable_expression", "[syntax_parser]") {
  syntax_parser p;
  feed(p, "let y = false\ny");
  auto let = p.parse_node();
  auto expression = p.parse_node();
  auto variable = dynamic_cast<variable_expression*>(expression.get());
  REQUIRE(variable != nullptr);
  REQUIRE(variable->get_variable()->get_name() == "y");
  REQUIRE(*variable->get_variable()->get_data_type() ==
    *bool_data_type::get_instance());
}

TEST_CASE("test_parsing_no_line_break", "[syntax_parser]") {
  SECTION("Test two literal expressions one after another.") {
    syntax_parser p;
    feed(p, "1 2");
    REQUIRE_THROWS(p.parse_node());
  }
  SECTION("Test a let expression followed by the declared variable.") {
    syntax_parser p;
    feed(p, "let x = true x");
    REQUIRE_THROWS(p.parse_node());
  }
}

TEST_CASE("test_incremental_parsing", "[syntax_parser]") {
  SECTION("Test feeding a few tokens at a time.") {
    syntax_parser p;
    incremental_feed(p, "let x ");
    REQUIRE_THROWS(p.parse_node());
    feed(p, "= false");
    auto statement = p.parse_node();
    auto bind = dynamic_cast<bind_variable_statement*>(statement.get());
  }
}

TEST_CASE("test_parsing_arithmetic_expression", "[syntax_parser]") {
  SECTION("Parse literal expression.") {
    syntax_parser p;
    feed(p, "1 + 2 * 3");
    auto e = p.parse_node();
  }
  SECTION("Parse variable expression.") {
    syntax_parser p;
    feed(p,
      "let x = 1\n"
      "let y = 2\n"
      "let z = 3\n"
      "x + y * z");
    auto let_x = p.parse_node();
    auto let_y = p.parse_node();
    auto let_z = p.parse_node();
    auto expression = p.parse_node();
  }
}

TEST_CASE("test_parsing_with_line_continuations", "[syntax_parser]") {
  SECTION("Parse continuation from a bracket.") {
    syntax_parser p;
    feed(p, "let x = 1 + (2 *\n 3)");
    auto e = p.parse_node();
  }
  SECTION("Parse continuation from an operator.") {
    syntax_parser p;
    feed(p, "let x = 1 +\n 2 * 3");
    auto e = p.parse_node();
  }
}