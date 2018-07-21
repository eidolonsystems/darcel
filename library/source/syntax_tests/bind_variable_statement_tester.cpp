#include <catch.hpp>
#include "darcel/lexicon/literal.hpp"
#include "darcel/lexicon/location.hpp"
#include "darcel/syntax/bind_variable_statement.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bind_variable_statement", "[bind_variable_statement]") {
  auto v = std::make_shared<Variable>(Location::global(), "x");
  auto e = std::make_unique<literal_expression>(Location::global(),
    *parse_literal("123"));
  bind_variable_statement s(Location::global(), v, std::move(e));
  REQUIRE(s.get_variable() == v);
}
