#include <catch.hpp>
#include "darcel/lexicon/literal.hpp"
#include "darcel/lexicon/location.hpp"
#include "darcel/syntax/bind_variable_statement.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bind_variable_statement", "[BindVariableStatement]") {
  auto v = std::make_shared<Variable>(Location::global(), "x");
  auto e = std::make_unique<LiteralExpression>(Location::global(),
    *parse_literal("123"));
  BindVariableStatement s(Location::global(), v, std::move(e));
  REQUIRE(s.get_variable() == v);
}
