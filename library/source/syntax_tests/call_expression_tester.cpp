#include <catch.hpp>
#include "darcel/syntax/call_expression.hpp"
#include "darcel/syntax/function_expression.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_call_expression", "[call_expression]") {
  auto f = std::make_shared<function>(Location::global(), "f");
  auto callable = std::make_unique<function_expression>(Location::global(), f);
  vector<std::unique_ptr<expression>> parameters;
  parameters.push_back(std::make_unique<literal_expression>(Location::global(),
    Literal("123", IntegerDataType::get_instance())));
  call_expression call(Location::global(), std::move(callable),
    std::move(parameters));
  auto fexpr = dynamic_cast<const function_expression*>(&call.get_callable());
  REQUIRE(fexpr != nullptr);
  REQUIRE(fexpr->get_function() == f);
  REQUIRE(call.get_arguments().size() == 1);
}
