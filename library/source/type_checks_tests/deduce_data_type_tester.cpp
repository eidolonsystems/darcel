#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/type_checks/deduce_data_type.hpp"

using namespace darcel;

TEST_CASE("test_deduce_literal", "[deduce_data_type]") {
  auto top_scope = make_builtin_scope();
  type_map m;
  auto s = make_literal(*top_scope, 123);
  auto t = deduce_data_type(*s, m);
  REQUIRE(t != nullptr);
  REQUIRE(*t == integer_data_type());
}

TEST_CASE("test_deduce_variable", "[deduce_data_type]") {
  auto top_scope = make_builtin_scope();
  type_map m;
  auto x = std::make_shared<variable>(location::global(), "x");
  m.insert(std::make_pair(x.get(), integer_data_type::get_instance()));
  auto s = std::make_unique<variable_expression>(location::global(),
    *top_scope, x);
  auto t = deduce_data_type(*s, m);
  REQUIRE(t != nullptr);
  REQUIRE(*t == integer_data_type());
}

TEST_CASE("test_deduce_add", "[deduce_data_type]") {
  SECTION("Deduce Int addition.") {
    auto top_scope = make_builtin_scope();
    type_map m;
    auto s = call(*top_scope, "add", make_literal(*top_scope, 2),
      make_literal(*top_scope, 3));
    auto t = deduce_data_type(*s, m);
    REQUIRE(t != nullptr);
    REQUIRE(*t == integer_data_type());
  }
  SECTION("Deduce Text addition.") {
    auto top_scope = make_builtin_scope();
    type_map m;
    auto s = call(*top_scope, "add", make_text(*top_scope, "a"),
      make_text(*top_scope, "b"));
    auto t = deduce_data_type(*s, m);
    REQUIRE(t != nullptr);
    REQUIRE(*t == text_data_type());
  }
  SECTION("Invalid addition.") {
    auto top_scope = make_builtin_scope();
    type_map m;
    auto s = call(*top_scope, "add", make_literal(*top_scope, true),
      make_literal(*top_scope, 3));
    REQUIRE_THROWS(deduce_data_type(*s, m));
  }
}
