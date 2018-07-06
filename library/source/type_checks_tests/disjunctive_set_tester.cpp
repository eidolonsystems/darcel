#include <catch.hpp>
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/type_checks/disjunctive_set.hpp"

using namespace darcel;

TEST_CASE("test_empty_disjunctive_set", "[disjunctive_set]") {
  disjunctive_set s;
  type_map m;
  REQUIRE(!s.is_satisfied(m));
}

TEST_CASE("test_single_disjunctive_set", "[disjunctive_set]") {
  auto top_scope = make_builtin_scope();
  bind_variable(*top_scope, "x", make_literal(*top_scope, 5));
  auto e1 = find_term("x", *top_scope);
  disjunctive_set s;
  s.add(*e1, integer_data_type::get_instance());
  SECTION("Test matching types.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x"),
      integer_data_type::get_instance()));
    REQUIRE(s.is_satisfied(m));
  }
  SECTION("Test mismatched types.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x"),
      bool_data_type::get_instance()));
    REQUIRE(!s.is_satisfied(m));
  }
  SECTION("Test no type assigned.") {
    type_map m;
    REQUIRE(!s.is_satisfied(m));
  }
}

TEST_CASE("test_two_constraints_disjunctive_set", "[disjunctive_set]") {
  auto top_scope = make_builtin_scope();
  bind_variable(*top_scope, "x", make_literal(*top_scope, 5));
  bind_variable(*top_scope, "y", make_literal(*top_scope, true));
  auto e1 = find_term("x", *top_scope);
  auto e2 = find_term("y", *top_scope);
  disjunctive_set s;
  s.add(*e1, integer_data_type::get_instance());
  s.add(*e2, bool_data_type::get_instance());
  SECTION("Test satisfying neither c1, c2.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x"),
      text_data_type::get_instance()));
    m.insert(std::make_pair(top_scope->find<variable>("y"),
      text_data_type::get_instance()));
    REQUIRE(!s.is_satisfied(m));
  }
  SECTION("Test satisfying c1 but not c2.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x"),
      integer_data_type::get_instance()));
    m.insert(std::make_pair(top_scope->find<variable>("y"),
      text_data_type::get_instance()));
    REQUIRE(s.is_satisfied(m));
  }
  SECTION("Test satisfying c2 but not c1.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x"),
      text_data_type::get_instance()));
    m.insert(std::make_pair(top_scope->find<variable>("y"),
      bool_data_type::get_instance()));
    REQUIRE(s.is_satisfied(m));
  }
  SECTION("Test satisfying both c1, c2.") {
    type_map m;
    m.insert(std::make_pair(top_scope->find<variable>("x"),
      integer_data_type::get_instance()));
    m.insert(std::make_pair(top_scope->find<variable>("y"),
      bool_data_type::get_instance()));
    REQUIRE(s.is_satisfied(m));
  }
  SECTION("Test no type mappings.") {
    type_map m;
    REQUIRE(!s.is_satisfied(m));
  }
}
