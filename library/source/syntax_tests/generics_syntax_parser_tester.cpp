#include <catch.hpp>
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax_tests/syntax_tests.hpp"
#include "darcel/utilities/utilities.hpp"

using namespace darcel;
using namespace darcel::tests;

namespace {
  std::unique_ptr<BindFunctionStatement> ensure_bind_function(
      SyntaxParser& p, int parameter_count) {
    auto f = dynamic_pointer_cast<BindFunctionStatement>(p.parse_node());
    REQUIRE(f != nullptr);
    REQUIRE(f->get_parameters().size() == parameter_count);
    return f;
  }

  void ensure_generic(const BindFunctionStatement& f, int index,
      const GenericDataType& t) {
    REQUIRE(static_cast<int>(f.get_parameters().size()) > index);
    REQUIRE(f.get_parameters()[index].m_type != nullptr);
    REQUIRE(*f.get_parameters()[index].m_type == t);
  }
}

TEST_CASE("test_parsing_generic_type", "[SyntaxParser]") {
  SECTION("Single generic.") {
    auto top_scope = make_builtin_scope();
    SyntaxParser p(*top_scope);
    feed(p, R"(let f(x: `T) = x)");
    auto f = ensure_bind_function(p, 1);
    ensure_generic(*f, 0, GenericDataType(Location::global(), "`T", 0));
  }
  SECTION("Two equal generics.") {
    auto top_scope = make_builtin_scope();
    SyntaxParser p(*top_scope);
    feed(p, R"(let f(x: `T, y: `T) = y)");
    auto f = ensure_bind_function(p, 2);
    ensure_generic(*f, 0, GenericDataType(Location::global(), "`T", 0));
    ensure_generic(*f, 1, GenericDataType(Location::global(), "`T", 0));
  }
  SECTION("Two distinct generics.") {
    auto top_scope = make_builtin_scope();
    SyntaxParser p(*top_scope);
    feed(p, R"(let f(x: `T, y: `U) = y)");
    auto f = ensure_bind_function(p, 2);
    ensure_generic(*f, 0, GenericDataType(Location::global(), "`T", 0));
    ensure_generic(*f, 1, GenericDataType(Location::global(), "`U", 1));
  }
}

TEST_CASE("test_parsing_generic_function_type_one_parameter",
    "[SyntaxParser]") {
  auto top_scope = make_builtin_scope();
  SyntaxParser p(*top_scope);
  feed(p, R"(let f(x: (a: `T) -> Int) = 1)");
  auto f = ensure_bind_function(p, 1);
  REQUIRE(f->get_parameters()[0].m_type != nullptr);
  auto x = std::dynamic_pointer_cast<FunctionDataType>(
    f->get_parameters()[0].m_type);
  REQUIRE(x != nullptr);
  REQUIRE(x->get_parameters().size() == 1);
  REQUIRE(*x->get_parameters()[0].m_type ==
    GenericDataType(Location::global(), "`T", 0));
}

TEST_CASE("test_parsing_generic_function_type_two_equal_parameters",
    "[SyntaxParser]") {
  auto top_scope = make_builtin_scope();
  SyntaxParser p(*top_scope);
  feed(p, R"(let f(x: (a: `T) -> Int, y: `T) = 1)");
  auto f = ensure_bind_function(p, 2);
  auto x = std::dynamic_pointer_cast<FunctionDataType>(
    f->get_parameters()[0].m_type);
  REQUIRE(x != nullptr);
  REQUIRE(x->get_parameters().size() == 1);
  REQUIRE(*x->get_parameters()[0].m_type ==
    GenericDataType(Location::global(), "`T", 0));
  ensure_generic(*f, 1, GenericDataType(Location::global(), "`T", 0));
}

TEST_CASE("test_generic_function_substitution", "[SyntaxParser]") {
  auto top_scope = make_builtin_scope();
  SyntaxParser p(*top_scope);
  feed(p, R"(let f(x: Int) = x
             let h(f: (x: `T) -> `T) = f
             let g = h(f))");
  auto f = dynamic_pointer_cast<BindFunctionStatement>(p.parse_node());
  REQUIRE(f != nullptr);
  auto h = dynamic_pointer_cast<BindFunctionStatement>(p.parse_node());
  REQUIRE(h != nullptr);
  auto g = dynamic_pointer_cast<BindVariableStatement>(p.parse_node());
  REQUIRE(g != nullptr);
}
