#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/syntax/variable_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_variable_expression", "[VariableExpression]") {
  auto v = std::make_shared<Variable>(Location::global(), "abc");
  VariableExpression e(Location::global(), v);
  REQUIRE(e.get_variable() == v);
}
