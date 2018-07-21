#include <catch.hpp>
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/type_checks/conjunctive_set.hpp"

using namespace darcel;

/*
TEST_CASE("test_empty_conjunctive_set", "[conjunctive_set]") {
  conjunctive_set s;
  type_map t;
  auto result = s.is_satisfied(t, scope());
  REQUIRE(result.m_is_satisfied);
  REQUIRE(result.m_conversions.empty());
}

TEST_CASE("test_single_conjunctive_set", "[conjunctive_set]") {
  scope s;
  auto binding = bind_variable(s, "x", make_literal(5));
  type_map t;
  t.add(*binding->get_variable(), std::make_shared<IntegerDataType>());
  auto e1 = find_term("x", s);
  SECTION("Test matching types.") {
    conjunctive_set set;
    set.add(*e1, IntegerDataType::get_instance());
    auto result = set.is_satisfied(t, s);
    REQUIRE(result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
  SECTION("Test mismatched types.") {
    conjunctive_set set;
    set.add(*e1, BoolDataType::get_instance());
    auto result = set.is_satisfied(t, s);
    REQUIRE(!result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
}

TEST_CASE("test_two_constraints_conjunctive_set", "[conjunctive_set]") {
  scope s;
  auto x_binding = bind_variable(s, "x", make_literal(5));
  auto y_binding = bind_variable(s, "y", make_literal(true));
  type_map t;
  t.add(*x_binding->get_variable(), std::make_shared<IntegerDataType>());
  t.add(*y_binding->get_variable(), std::make_shared<BoolDataType>());
  auto e1 = find_term("x", s);
  auto e2 = find_term("y", s);
  SECTION("Test satisfying neither c1, c2.") {
    conjunctive_set set;
    set.add(*e1, TextDataType::get_instance());
    set.add(*e2, TextDataType::get_instance());
    auto result = set.is_satisfied(t, s);
    REQUIRE(!result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
  SECTION("Test satisfying c1 but not c2.") {
    conjunctive_set set;
    set.add(*e1, IntegerDataType::get_instance());
    set.add(*e2, TextDataType::get_instance());
    auto result = set.is_satisfied(t, s);
    REQUIRE(!result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
  SECTION("Test satisfying c2 but not c1.") {
    conjunctive_set set;
    set.add(*e1, TextDataType::get_instance());
    set.add(*e2, BoolDataType::get_instance());
    auto result = set.is_satisfied(t, s);
    REQUIRE(!result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
  SECTION("Test satisfying both c1, c2.") {
    conjunctive_set set;
    set.add(*e1, IntegerDataType::get_instance());
    set.add(*e2, BoolDataType::get_instance());
    auto result = set.is_satisfied(t, s);
    REQUIRE(result.m_is_satisfied);
    REQUIRE(result.m_conversions.empty());
  }
}
*/
