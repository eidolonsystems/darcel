#include <catch.hpp>
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/type_checks/conjunctive_set.hpp"

using namespace darcel;

TEST_CASE("test_empty_conjunctive_set", "[conjunctive_set]") {
  conjunctive_set s;
  type_map m;
  REQUIRE(s.is_satisfied(m));
}

TEST_CASE("test_single_conjunctive_set", "[conjunctive_set]") {
  auto top_scope = make_builtin_scope();
  bind_variable(*top_scope, "x", make_literal(*top_scope, 5));
  auto e1 = find_term("x", *top_scope);
  conjunctive_set s;
  s.add(*e1, integer_data_type::get_instance());
  SECTION("Test matching types.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x").get(),
      integer_data_type::get_instance()));
    REQUIRE(s.is_satisfied(m));
  }
  SECTION("Test mismatched types.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x").get(),
      bool_data_type::get_instance()));
    REQUIRE(!s.is_satisfied(m));
  }
  SECTION("Test no type assigned.") {
    type_map m;
    REQUIRE(!s.is_satisfied(m));
  }
}

TEST_CASE("test_two_constraints_conjunctive_set", "[conjunctive_set]") {
  auto top_scope = make_builtin_scope();
  bind_variable(*top_scope, "x", make_literal(*top_scope, 5));
  bind_variable(*top_scope, "y", make_literal(*top_scope, true));
  auto e1 = find_term("x", *top_scope);
  auto e2 = find_term("y", *top_scope);
  conjunctive_set s;
  s.add(*e1, integer_data_type::get_instance());
  s.add(*e2, bool_data_type::get_instance());
  SECTION("Test not satisfying x and y.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x").get(),
      text_data_type::get_instance()));
    m.insert(std::make_pair(top_scope->find<variable>("y").get(),
      text_data_type::get_instance()));
    REQUIRE(!s.is_satisfied(m));
  }
  SECTION("Test satisfying x but not y.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x").get(),
      integer_data_type::get_instance()));
    m.insert(std::make_pair(top_scope->find<variable>("y").get(),
      text_data_type::get_instance()));
    REQUIRE(!s.is_satisfied(m));
  }
  SECTION("Test satisfying y but not x.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x").get(),
      text_data_type::get_instance()));
    m.insert(std::make_pair(top_scope->find<variable>("y").get(),
      bool_data_type::get_instance()));
    REQUIRE(!s.is_satisfied(m));
  }
  SECTION("Test satisfying both x, y.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x").get(),
      integer_data_type::get_instance()));
    m.insert(std::make_pair(top_scope->find<variable>("y").get(),
      bool_data_type::get_instance()));
    REQUIRE(s.is_satisfied(m));
  }
  SECTION("Test no type mappings.") {
    type_map m;
    REQUIRE(!s.is_satisfied(m));
  }
}
