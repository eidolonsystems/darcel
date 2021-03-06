#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/lexicon/literal.hpp"
#include "darcel/lexicon/location.hpp"
#include "darcel/syntax/bind_function_statement.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bind_function_statement", "[BindFunctionStatement]") {
  auto f = std::make_shared<Function>(Location::global(), "f");
  auto p1 = std::make_shared<Variable>(Location::global(), "a");
  auto p = std::vector<BindFunctionStatement::parameter>{
    {p1, IntegerDataType::get_instance()}};
  auto e = std::make_unique<LiteralExpression>(Location::global(),
    *parse_literal("123"));
  BindFunctionStatement s(Location::global(), f, p, std::move(e));
  REQUIRE(s.get_function() == f);
  REQUIRE(s.get_parameters().size() == 1);
  REQUIRE(s.get_parameters()[0].m_variable == p1);
  REQUIRE(s.get_parameters()[0].m_type != nullptr);
  REQUIRE(*s.get_parameters()[0].m_type == IntegerDataType());
}
