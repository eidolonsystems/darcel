#include <catch.hpp>
#include "darcel/data_types/callable_data_type.hpp"
#include "darcel/data_types/enum_data_type.hpp"
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"

using namespace darcel;
using namespace std;

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
