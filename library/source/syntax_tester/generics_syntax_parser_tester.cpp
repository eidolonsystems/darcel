#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/lexicon/token_parser.hpp"
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax/syntax_parser.hpp"
#include "darcel/utilities/utilities.hpp"

using namespace darcel;

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
    std::copy(std::begin(s), std::end(s), std::begin(q));
    q[N] = '\0';
    incremental_feed(p, q);
  }
}

TEST_CASE("test_parsing_generic_type", "[syntax_parser]") {
  SECTION("Single generic.") {
    syntax_parser p;
    feed(p, R"(let f(x: `T) = x
               let y = f(5))");
    p.parse_node();
    auto y = dynamic_pointer_cast<bind_variable_statement>(p.parse_node());
    REQUIRE(y != nullptr);
    REQUIRE(*y->get_variable()->get_data_type() == integer_data_type());
  }
  SECTION("Call two equal generics with equal types.") {
    syntax_parser p;
    feed(p, R"(let f(x: `T, y: `T) = y
               let z = f(true, false))");
    p.parse_node();
    auto z = dynamic_pointer_cast<bind_variable_statement>(p.parse_node());
    REQUIRE(z != nullptr);
    REQUIRE(*z->get_variable()->get_data_type() == bool_data_type());
  }
  SECTION("Call two equal generics with distinct types.") {
    syntax_parser p;
    feed(p, R"(let f(x: `T, y: `T) = y
               let z = f(true, 5))");
    p.parse_node();
    REQUIRE_THROWS(p.parse_node());
  }
}

TEST_CASE("test_generic_instantiation", "[syntax_parser]") {
  syntax_parser p;
  feed(p, R"(let f(x: `T, y: `T) = x
             let g(x: `T, y: `U) = f(x, y))");
  p.parse_node();
  REQUIRE_THROWS(p.parse_node());
}

TEST_CASE("test_built_in_generic_instantiation", "[syntax_parser]") {
  syntax_parser p(make_builtin_scope());
  feed(p, R"(let x = fold(add, 5))");
  auto x = dynamic_pointer_cast<bind_variable_statement>(p.parse_node());
  REQUIRE(x != nullptr);
}

TEST_CASE("test_parsing_generic_function_type_one_parameter",
    "[syntax_parser]") {
  syntax_parser p(make_builtin_scope());
  feed(p, R"(let f(x: (a: `T) -> Int) = 1)");
  auto f = dynamic_pointer_cast<bind_function_statement>(p.parse_node());
  REQUIRE(f != nullptr);
  REQUIRE(f->get_function()->get_name() == "f");
  auto t = std::dynamic_pointer_cast<function_data_type>(
    f->get_overload()->get_data_type());
  REQUIRE(t != nullptr);
  REQUIRE(t->get_parameters().size() == 1);
  auto x = std::dynamic_pointer_cast<function_data_type>(
    t->get_parameters().front().m_type);
  REQUIRE(x != nullptr);
  REQUIRE(x->get_parameters().size() == 1);
  REQUIRE(*x->get_parameters()[0].m_type ==
    generic_data_type(location::global(), "`T", 0));
}

TEST_CASE("test_parsing_generic_function_type_two_equal_parameters",
    "[syntax_parser]") {
  syntax_parser p(make_builtin_scope());
  feed(p, R"(let f(x: (a: `T) -> Int, y: `T) = 1)");
  auto f = dynamic_pointer_cast<bind_function_statement>(p.parse_node());
  REQUIRE(f != nullptr);
  REQUIRE(f->get_function()->get_name() == "f");
  auto t = std::dynamic_pointer_cast<function_data_type>(
    f->get_overload()->get_data_type());
  REQUIRE(t != nullptr);
  REQUIRE(t->get_parameters().size() == 2);
  auto x = std::dynamic_pointer_cast<function_data_type>(
    t->get_parameters()[0].m_type);
  REQUIRE(x != nullptr);
  REQUIRE(x->get_parameters().size() == 1);
  REQUIRE(*x->get_parameters()[0].m_type ==
    generic_data_type(location::global(), "`T", 0));
  auto y = std::dynamic_pointer_cast<generic_data_type>(
    t->get_parameters()[1].m_type);
  REQUIRE(y != nullptr);
  REQUIRE(*x->get_parameters()[0].m_type == *y);
}

TEST_CASE("test_generic_function_substitution", "[syntax_parser]") {
  syntax_parser p(make_builtin_scope());
  feed(p, R"(let f(x: Int) = x
             let h(f: (x: `T) -> `T) = f
             let g = h(f))");
  auto f = dynamic_pointer_cast<bind_function_statement>(p.parse_node());
  REQUIRE(f != nullptr);
  auto h = dynamic_pointer_cast<bind_function_statement>(p.parse_node());
  REQUIRE(h != nullptr);
  auto g = dynamic_pointer_cast<bind_variable_statement>(p.parse_node());
  REQUIRE(g != nullptr);
}
