#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/semantics/function_definition.hpp"

using namespace darcel;

TEST_CASE("test_function_definition", "[function_definition]") {
  auto f = std::make_shared<function>(location::global(), "f");
  auto t = std::make_shared<function_data_type>(
    std::vector<function_data_type::parameter>{
    {"x", integer_data_type::get_instance()}}, bool_data_type::get_instance());
  auto definition = std::make_shared<function_definition>(location::global(),
    f, t);
  REQUIRE(definition->get_function() == f);
  REQUIRE(definition->get_type() == t);
  REQUIRE(definition->get_name() == "f");
  REQUIRE(definition->get_location() == location::global());
}
