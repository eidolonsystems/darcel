#include <catch.hpp>
#include "darcel/syntax/call_expression.hpp"
#include "darcel/syntax/function_expression.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_call_expression", "[CallExpression]") {
  auto f = std::make_shared<Function>(Location::global(), "f");
  auto callable = std::make_unique<FunctionExpression>(Location::global(), f);
  vector<std::unique_ptr<Expression>> parameters;
  parameters.push_back(std::make_unique<LiteralExpression>(Location::global(),
    Literal("123", IntegerDataType::get_instance())));
  CallExpression call(Location::global(), std::move(callable),
    std::move(parameters));
  auto fexpr = dynamic_cast<const FunctionExpression*>(&call.get_callable());
  REQUIRE(fexpr != nullptr);
  REQUIRE(fexpr->get_function() == f);
  REQUIRE(call.get_arguments().size() == 1);
}
