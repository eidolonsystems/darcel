#include <catch.hpp>
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/type_checks/disjunctive_set.hpp"

using namespace darcel;

/*
TEST_CASE("test_empty_disjunctive_set", "[disjunctive_set]") {
  disjunctive_set s;
  type_map t;
  auto result = s.is_satisfied(t, scope());
  REQUIRE(!result.m_is_satisfied);
  REQUIRE(result.m_conversions.empty());
}

TEST_CASE("test_single_disjunctive_set", "[disjunctive_set]") {
  scope s;
  auto binding = bind_variable(s, "x", make_literal(5));
  type_map t;
  t.add(*binding->get_variable(), std::make_shared<IntegerDataType>());
  auto e1 = find_term("x", s);
  SECTION("Test matching types.") {
    disjunctive_set c;
    c.add(*e1, IntegerDataType::get_instance());
    auto result = c.is_satisfied(t, s);
    REQUIRE(result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
  SECTION("Test mismatched types.") {
    disjunctive_set c;
    c.add(*e1, BoolDataType::get_instance());
    auto result = c.is_satisfied(t, s);
    REQUIRE(!result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
}

TEST_CASE("test_two_constraints_disjunctive_set", "[disjunctive_set]") {
  scope s;
  auto x_binding = bind_variable(s, "x", make_literal(5));
  auto y_binding = bind_variable(s, "y", make_literal(true));
  type_map t;
  t.add(*x_binding->get_variable(), std::make_shared<IntegerDataType>());
  t.add(*y_binding->get_variable(), std::make_shared<BoolDataType>());
  auto e1 = find_term("x", s);
  auto e2 = find_term("y", s);
  SECTION("Test satisfying neither c1, c2.") {
    disjunctive_set c;
    c.add(*e1, TextDataType::get_instance());
    c.add(*e2, TextDataType::get_instance());
    auto result = c.is_satisfied(t, s);
    REQUIRE(!result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
  SECTION("Test satisfying c1 but not c2.") {
    disjunctive_set c;
    c.add(*e1, IntegerDataType::get_instance());
    c.add(*e2, TextDataType::get_instance());
    auto result = c.is_satisfied(t, s);
    REQUIRE(result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
  SECTION("Test satisfying c2 but not c1.") {
    disjunctive_set c;
    c.add(*e1, TextDataType::get_instance());
    c.add(*e2, BoolDataType::get_instance());
    auto result = c.is_satisfied(t, s);
    REQUIRE(result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
  SECTION("Test satisfying both c1, c2.") {
    disjunctive_set c;
    c.add(*e1, IntegerDataType::get_instance());
    c.add(*e2, BoolDataType::get_instance());
    auto result = c.is_satisfied(t, s);
    REQUIRE(result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
}
*/
