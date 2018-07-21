#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/callable_data_type.hpp"
#include "darcel/data_types/enum_data_type.hpp"
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;

TEST_CASE("test_generic_name", "[GenericDataType]") {
  REQUIRE(GenericDataType(Location::global(), "`T", 0).get_name() == "`T");
}

TEST_CASE("test_generic_equality", "[GenericDataType]") {
  REQUIRE(GenericDataType(Location::global(), "`T", 0) !=
    GenericDataType(Location::global(), "`T", 1));
  REQUIRE(GenericDataType(Location::global(), "`A", 2) ==
    GenericDataType(Location::global(), "`B", 2));
  REQUIRE(GenericDataType(Location::global(), "`A", 2) !=
    IntegerDataType());
}

TEST_CASE("test_is_generic", "[GenericDataType]") {
  SECTION("Generic terms.") {
    REQUIRE(is_generic(GenericDataType(Location::global(), "`T", 0)));
    REQUIRE(!is_generic(IntegerDataType()));
    REQUIRE(!is_generic(CallableDataType(
      std::make_shared<Function>(Location::global(), "f"))));
    REQUIRE(!is_generic(EnumDataType(Location::global(), "e", {})));
  }
  SECTION("Compound generics.") {
    auto f1 = make_function_data_type({}, IntegerDataType::get_instance());
    REQUIRE(!is_generic(*f1));
    auto f2 = make_function_data_type({},
      std::make_shared<GenericDataType>(Location::global(), "`T", 0));
    REQUIRE(is_generic(*f2));
  }
}

TEST_CASE("test_extracting_generics", "[GenericDataType]") {
  SECTION("Concrete atomic type.") {
    auto t = IntegerDataType::get_instance();
    auto terms = extract_generic_terms(t);
    REQUIRE(terms.empty());
  }
  SECTION("Generic term.") {
    auto g = make_generic_data_type("`T", 0);
    auto terms = extract_generic_terms(g);
    REQUIRE(terms.size() == 1);
    REQUIRE(*terms[0] == *g);
  }
  SECTION("Concrete composite type.") {
    auto t = make_function_data_type({{"x", IntegerDataType::get_instance()}},
      BoolDataType::get_instance());
    auto terms = extract_generic_terms(t);
    REQUIRE(terms.empty());
  }
  SECTION("Single generic composite type.") {
    auto g = make_generic_data_type("`T", 0);
    auto t = make_function_data_type({{"x", g}},
      BoolDataType::get_instance());
    auto terms = extract_generic_terms(t);
    REQUIRE(terms.size() == 1);
    REQUIRE(*terms[0] == *g);
  }
  SECTION("Duplicate generic composite type.") {
    auto g = make_generic_data_type("`T", 0);
    auto t = make_function_data_type({{"x", g}}, g);
    auto terms = extract_generic_terms(t);
    REQUIRE(terms.size() == 1);
    REQUIRE(*terms[0] == *g);
  }
  SECTION("Multiple generics composite type.") {
    auto g1 = make_generic_data_type("`T", 0);
    auto g2 = make_generic_data_type("`U", 1);
    auto t = make_function_data_type({{"x", g1}}, g2);
    auto terms = extract_generic_terms(t);
    REQUIRE(terms.size() == 2);
    REQUIRE(std::find_if(terms.begin(), terms.end(),
      [&] (auto& term) { return *term == *g1; }) != terms.end());
    REQUIRE(std::find_if(terms.begin(), terms.end(),
      [&] (auto& term) { return *term == *g2; }) != terms.end());
  }
}
