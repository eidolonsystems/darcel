#include <catch.hpp>
#include "darcel/syntax_tester/syntax_tester.hpp"
#include "darcel/utilities/utilities.hpp"

using namespace darcel;
using namespace darcel::tests;

TEST_CASE("test_declaring_empty_enum", "[syntax_parser]") {
  syntax_parser p;
  feed(p, "let e = enum()");
  auto e = dynamic_pointer_cast<bind_enum_statement>(p.parse_node());
  REQUIRE(e != nullptr);
  REQUIRE(e->get_enum()->get_name() == "e");
  REQUIRE(e->get_enum()->get_symbols().empty());
}

TEST_CASE("test_singleton_enum", "[syntax_parser]") {
  SECTION("No explicit value.") {
    syntax_parser p;
    feed(p, "let e = enum(A)");
    auto e = dynamic_pointer_cast<bind_enum_statement>(p.parse_node());
    REQUIRE(e != nullptr);
    REQUIRE(e->get_enum()->get_name() == "e");
    REQUIRE(e->get_enum()->get_symbols().size() == 1);
    REQUIRE(e->get_enum()->get_symbols()[0].m_name == "A");
    REQUIRE(e->get_enum()->get_symbols()[0].m_value == 0);
  }
  SECTION("Explicit value.") {
    syntax_parser p;
    feed(p, "let e = enum(A = 123)");
    auto e = dynamic_pointer_cast<bind_enum_statement>(p.parse_node());
    REQUIRE(e != nullptr);
    REQUIRE(e->get_enum()->get_name() == "e");
    REQUIRE(e->get_enum()->get_symbols().size() == 1);
    REQUIRE(e->get_enum()->get_symbols()[0].m_name == "A");
    REQUIRE(e->get_enum()->get_symbols()[0].m_value == 123);
  }
}

TEST_CASE("test_double_enum", "[syntax_parser]") {
  SECTION("No explicit values.") {
    syntax_parser p;
    feed(p, "let e = enum(A, B)");
    auto e = dynamic_pointer_cast<bind_enum_statement>(p.parse_node());
    REQUIRE(e != nullptr);
    REQUIRE(e->get_enum()->get_name() == "e");
    REQUIRE(e->get_enum()->get_symbols().size() == 2);
    REQUIRE(e->get_enum()->get_symbols()[0].m_name == "A");
    REQUIRE(e->get_enum()->get_symbols()[0].m_value == 0);
    REQUIRE(e->get_enum()->get_symbols()[1].m_name == "B");
    REQUIRE(e->get_enum()->get_symbols()[1].m_value == 1);
  }
  SECTION("First explicit value.") {
    syntax_parser p;
    feed(p, "let e = enum(A = 321, B)");
    auto e = dynamic_pointer_cast<bind_enum_statement>(p.parse_node());
    REQUIRE(e != nullptr);
    REQUIRE(e->get_enum()->get_name() == "e");
    REQUIRE(e->get_enum()->get_symbols().size() == 2);
    REQUIRE(e->get_enum()->get_symbols()[0].m_name == "A");
    REQUIRE(e->get_enum()->get_symbols()[0].m_value == 321);
    REQUIRE(e->get_enum()->get_symbols()[1].m_name == "B");
    REQUIRE(e->get_enum()->get_symbols()[1].m_value == 322);
  }
  SECTION("Second explicit value.") {
    syntax_parser p;
    feed(p, "let e = enum(A, B = 911)");
    auto e = dynamic_pointer_cast<bind_enum_statement>(p.parse_node());
    REQUIRE(e != nullptr);
    REQUIRE(e->get_enum()->get_name() == "e");
    REQUIRE(e->get_enum()->get_symbols().size() == 2);
    REQUIRE(e->get_enum()->get_symbols()[0].m_name == "A");
    REQUIRE(e->get_enum()->get_symbols()[0].m_value == 0);
    REQUIRE(e->get_enum()->get_symbols()[1].m_name == "B");
    REQUIRE(e->get_enum()->get_symbols()[1].m_value == 911);
  }
  SECTION("Both valid explicit values.") {
    syntax_parser p;
    feed(p, "let e = enum(A = 100, B = 200)");
    auto e = dynamic_pointer_cast<bind_enum_statement>(p.parse_node());
    REQUIRE(e != nullptr);
    REQUIRE(e->get_enum()->get_name() == "e");
    REQUIRE(e->get_enum()->get_symbols().size() == 2);
    REQUIRE(e->get_enum()->get_symbols()[0].m_name == "A");
    REQUIRE(e->get_enum()->get_symbols()[0].m_value == 100);
    REQUIRE(e->get_enum()->get_symbols()[1].m_name == "B");
    REQUIRE(e->get_enum()->get_symbols()[1].m_value == 200);
  }
  SECTION("Invalid explicit values.") {
    syntax_parser p;
    feed(p, "let e = enum(A = 100, B = 50)");
    REQUIRE_THROWS_AS(p.parse_node(), invalid_enum_value_syntax_error);
  }
}

TEST_CASE("test_duplicate_enum_symbols", "[syntax_parser]") {
  syntax_parser p;
  feed(p, "let e = enum(A, A)");
  REQUIRE_THROWS_AS(p.parse_node(), redefinition_syntax_error);
}

TEST_CASE("test_enum_access", "[syntax_parser]") {
  syntax_parser p;
  feed(p, R"(let e = enum(A, B)
             e.B)");
  p.parse_node();
  auto access = dynamic_pointer_cast<enum_expression>(p.parse_node());
  REQUIRE(access != nullptr);
  REQUIRE(access->get_index() == 1);
  REQUIRE(access->get_enum()->get_symbols()[1].m_name == "B");
}

TEST_CASE("test_invalid_enum_access", "[syntax_parser]") {
  syntax_parser p;
  feed(p, R"(let e = enum(A, B)
             e.C)");
  p.parse_node();
  REQUIRE_THROWS_AS(p.parse_node(), invalid_member_syntax_error);
}
