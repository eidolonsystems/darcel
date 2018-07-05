#include <catch.hpp>
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax_tester/syntax_tester.hpp"
#include "darcel/utilities/utilities.hpp"

using namespace darcel;
using namespace darcel::tests;

namespace {
  std::unique_ptr<bind_function_statement> ensure_bind_function(
      syntax_parser& p, int parameter_count) {
    auto f = dynamic_pointer_cast<bind_function_statement>(p.parse_node());
    REQUIRE(f != nullptr);
    REQUIRE(f->get_parameters().size() == parameter_count);
    return f;
  }

  void ensure_generic(const bind_function_statement& f, int index,
      const generic_data_type& t) {
    REQUIRE(static_cast<int>(f.get_parameters().size()) > index);
    REQUIRE(f.get_parameters()[index].m_type.has_value());
    REQUIRE(**f.get_parameters()[index].m_type == t);
  }
}

TEST_CASE("test_parsing_generic_type", "[syntax_parser]") {
  SECTION("Single generic.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, R"(let f(x: `T) = x)");
    auto f = ensure_bind_function(p, 1);
    ensure_generic(*f, 0, generic_data_type(location::global(), "`T", 0));
  }
  SECTION("Two equal generics.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, R"(let f(x: `T, y: `T) = y)");
    auto f = ensure_bind_function(p, 2);
    ensure_generic(*f, 0, generic_data_type(location::global(), "`T", 0));
    ensure_generic(*f, 1, generic_data_type(location::global(), "`T", 0));
  }
  SECTION("Two distinct generics.") {
    auto top_scope = make_builtin_scope();
    syntax_parser p(*top_scope);
    feed(p, R"(let f(x: `T, y: `U) = y)");
    auto f = ensure_bind_function(p, 2);
    ensure_generic(*f, 0, generic_data_type(location::global(), "`T", 0));
    ensure_generic(*f, 1, generic_data_type(location::global(), "`U", 1));
  }
}

TEST_CASE("test_parsing_generic_function_type_one_parameter",
    "[syntax_parser]") {
  auto top_scope = make_builtin_scope();
  syntax_parser p(*top_scope);
  feed(p, R"(let f(x: (a: `T) -> Int) = 1)");
  auto f = ensure_bind_function(p, 1);
  REQUIRE(f->get_parameters()[0].m_type.has_value());
  auto x = std::dynamic_pointer_cast<function_data_type>(
    *f->get_parameters()[0].m_type);
  REQUIRE(x != nullptr);
  REQUIRE(x->get_parameters().size() == 1);
  REQUIRE(*x->get_parameters()[0].m_type ==
    generic_data_type(location::global(), "`T", 0));
}

TEST_CASE("test_parsing_generic_function_type_two_equal_parameters",
    "[syntax_parser]") {
  auto top_scope = make_builtin_scope();
  syntax_parser p(*top_scope);
  feed(p, R"(let f(x: (a: `T) -> Int, y: `T) = 1)");
  auto f = ensure_bind_function(p, 2);
  auto x = std::dynamic_pointer_cast<function_data_type>(
    *f->get_parameters()[0].m_type);
  REQUIRE(x != nullptr);
  REQUIRE(x->get_parameters().size() == 1);
  REQUIRE(*x->get_parameters()[0].m_type ==
    generic_data_type(location::global(), "`T", 0));
  ensure_generic(*f, 1, generic_data_type(location::global(), "`T", 0));
}

TEST_CASE("test_generic_function_substitution", "[syntax_parser]") {
  auto top_scope = make_builtin_scope();
  syntax_parser p(*top_scope);
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
