#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/syntax/variable_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_variable_expression", "[variable_expression]") {
  scope top_scope;
  auto v = std::make_shared<variable>(location::global(), "abc");
  top_scope.add(v);
  variable_expression e(location::global(), top_scope, v);
  REQUIRE(e.get_variable() == v);
}
