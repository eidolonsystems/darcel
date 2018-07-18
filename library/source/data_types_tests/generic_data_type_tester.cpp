#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/callable_data_type.hpp"
#include "darcel/data_types/enum_data_type.hpp"
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;

TEST_CASE("test_generic_name", "[generic_data_type]") {
  REQUIRE(generic_data_type(location::global(), "`T", 0).get_name() == "`T");
}

TEST_CASE("test_generic_equality", "[generic_data_type]") {
  REQUIRE(generic_data_type(location::global(), "`T", 0) !=
    generic_data_type(location::global(), "`T", 1));
  REQUIRE(generic_data_type(location::global(), "`A", 2) ==
    generic_data_type(location::global(), "`B", 2));
  REQUIRE(generic_data_type(location::global(), "`A", 2) !=
    integer_data_type());
}

TEST_CASE("test_is_generic", "[generic_data_type]") {
  SECTION("Generic terms.") {
    REQUIRE(is_generic(generic_data_type(location::global(), "`T", 0)));
    REQUIRE(!is_generic(integer_data_type()));
    REQUIRE(!is_generic(callable_data_type(
      std::make_shared<function>(location::global(), "f"))));
    REQUIRE(!is_generic(enum_data_type(location::global(), "e", {})));
  }
  SECTION("Compound generics.") {
    auto f1 = make_function_data_type({}, integer_data_type::get_instance());
    REQUIRE(!is_generic(*f1));
    auto f2 = make_function_data_type({},
      std::make_shared<generic_data_type>(location::global(), "`T", 0));
    REQUIRE(is_generic(*f2));
  }
}

TEST_CASE("test_extracting_generics", "[generic_data_type]") {
  SECTION("Concrete atomic type.") {
    auto t = integer_data_type::get_instance();
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
    auto t = make_function_data_type({{"x", integer_data_type::get_instance()}},
      bool_data_type::get_instance());
    auto terms = extract_generic_terms(t);
    REQUIRE(terms.empty());
  }
  SECTION("Single generic composite type.") {
    auto g = make_generic_data_type("`T", 0);
    auto t = make_function_data_type({{"x", g}},
      bool_data_type::get_instance());
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
