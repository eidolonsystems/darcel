#include <catch.hpp>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/semantics/function_definition.hpp"

using namespace darcel;

TEST_CASE("test_function_definition", "[FunctionDefinition]") {
  auto f = std::make_shared<Function>(Location::global(), "f");
  auto t = std::make_shared<FunctionDataType>(
    std::vector<FunctionDataType::Parameter>{
    {"x", IntegerDataType::get_instance()}}, BoolDataType::get_instance());
  auto definition = std::make_shared<FunctionDefinition>(Location::global(),
    f, t);
  REQUIRE(definition->get_function() == f);
  REQUIRE(definition->get_type() == t);
  REQUIRE(definition->get_name() == "f");
  REQUIRE(definition->get_location() == Location::global());
}
