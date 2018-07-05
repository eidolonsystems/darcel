#include <catch.hpp>
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_literal_expression", "[literal_expression]") {
  auto top_scope = make_builtin_scope();
  literal_expression e(location::global(), *top_scope,
    *parse_literal("3.1415"));
  REQUIRE(e.get_literal().get_value() == "3.1415");
  REQUIRE(*e.get_literal().get_type() == *float_data_type::get_instance());
}
