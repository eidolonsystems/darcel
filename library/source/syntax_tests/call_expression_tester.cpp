#include <catch.hpp>
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax/call_expression.hpp"
#include "darcel/syntax/function_expression.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_call_expression", "[call_expression]") {
  auto top_scope = make_builtin_scope();
  auto f = std::make_shared<function>(location::global(), "f");
  top_scope->add(f);
  auto callable = std::make_unique<function_expression>(location::global(),
    *top_scope, f);
  vector<std::unique_ptr<expression>> parameters;
  parameters.push_back(std::make_unique<literal_expression>(location::global(),
    *top_scope, literal("123", integer_data_type::get_instance())));
  call_expression call(location::global(), *top_scope, std::move(callable),
    std::move(parameters));
  auto fexpr = dynamic_cast<const function_expression*>(&call.get_callable());
  REQUIRE(fexpr != nullptr);
  REQUIRE(fexpr->get_function() == f);
  REQUIRE(call.get_parameters().size() == 1);
}
