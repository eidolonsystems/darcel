#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax_tests/syntax_tests.hpp"

using namespace darcel;
using namespace darcel::tests;
using namespace std;

TEST_CASE("test_parsing_terminal", "[syntax_parser]") {
  auto top_scope = make_builtin_scope();
  syntax_parser p(*top_scope);
  feed(p, "");
  auto expression = p.parse_node();
  REQUIRE(dynamic_cast<terminal_node*>(expression.get()) != nullptr);
}

TEST_CASE("test_parsing_term", "[syntax_parser]") {
  SECTION("Parse identifiers.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "hello world");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse identifiers with new lines.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "hello\nworld");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse brackets.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "(()()()())");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse brackets with new lines.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "(\n5)");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse operators.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "5 + 5");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse operators with new line.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "5 +\n 5");
    auto c = p.get_next_terminal();
  }
  SECTION("Parse colon.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "def x:5 6\n 7 end def y: 3 1 end");
    auto c = p.get_next_terminal();
  }
}

TEST_CASE("test_parsing_literal_expression", "[syntax_parser]") {
  auto top_scope = make_builtin_scope();
  syntax_parser p(*top_scope);
  feed(p, "123");
  auto expression = p.parse_node();
  auto literal = dynamic_cast<literal_expression*>(expression.get());
  REQUIRE(literal != nullptr);
  REQUIRE(literal->get_literal().get_value() == "123");
  REQUIRE(*literal->get_literal().get_type() == integer_data_type());
}

TEST_CASE("test_parsing_bind_variable_statement", "[syntax_parser]") {
  auto top_scope = make_builtin_scope();
  syntax_parser p(*top_scope);
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
  auto top_scope = make_builtin_scope();
  syntax_parser p(*top_scope);
  feed(p, "let y = false\ny");
  auto let = p.parse_node();
  auto expression = p.parse_node();
  auto variable = dynamic_cast<variable_expression*>(expression.get());
  REQUIRE(variable != nullptr);
  REQUIRE(variable->get_variable()->get_name() == "y");
}

TEST_CASE("test_parsing_no_line_break", "[syntax_parser]") {
  SECTION("Test two literal expressions one after another.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "1 2");
    REQUIRE_THROWS(p.parse_node());
  }
  SECTION("Test a let expression followed by the declared variable.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "let x = true x");
    REQUIRE_THROWS(p.parse_node());
  }
}

TEST_CASE("test_incremental_parsing", "[syntax_parser]") {
  SECTION("Test feeding a few tokens at a time.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    incremental_feed(p, "let x ");
    REQUIRE_THROWS(p.parse_node());
    feed(p, "= false");
    auto statement = p.parse_node();
    auto bind = dynamic_cast<bind_variable_statement*>(statement.get());
  }
}

TEST_CASE("test_parsing_arithmetic_expression", "[syntax_parser]") {
  SECTION("Parse literal expression.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "1 + 2 * 3");
    auto e = p.parse_node();
  }
  SECTION("Parse variable expression.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
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
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "let x = 1 + (2 *\n 3)");
    auto e = p.parse_node();
  }
  SECTION("Parse continuation from an operator.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "let x = 1 +\n 2 * 3");
    auto e = p.parse_node();
  }
}

TEST_CASE("test_parsing_function_definition", "[syntax_parser]") {
  SECTION("Parse no parameter function.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "let f() = 5");
    auto e = p.parse_node();
  }
  SECTION("Parse single parameter function.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "let f(x: Int) = x");
    auto e = p.parse_node();
  }
  SECTION("Parse nested function parameter.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, "let f(g: (x: Int) -> Int) = g(5)");
    auto e = p.parse_node();
    auto f = dynamic_cast<const bind_function_statement*>(e.get());
    REQUIRE(f != nullptr);
    REQUIRE(f->get_parameters().size() == 1);
    REQUIRE(f->get_parameters()[0].m_type.has_value());
    auto g = dynamic_pointer_cast<function_data_type>(
      *f->get_parameters()[0].m_type);
    REQUIRE(g != nullptr);
  }
}
