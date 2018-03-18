#include <catch.hpp>
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/syntax/call_expression.hpp"
#include "darcel/syntax/literal_expression.hpp"
#include "darcel/syntax/variable_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_call_expression", "[call_expression]") {
  vector<function_data_type::parameter> type_parameters;
  type_parameters.push_back({"a", integer_data_type::get_instance()});
  auto f_type = std::make_shared<function_data_type>(type_parameters,
    integer_data_type::get_instance());
  auto f = std::make_shared<variable>(location::global(), "f", f_type);
  auto callable = std::make_unique<variable_expression>(location::global(), f);
  vector<std::unique_ptr<expression>> parameters;
  parameters.push_back(std::make_unique<literal_expression>(location::global(),
    literal("123", integer_data_type::get_instance())));
  call_expression call(location::global(), std::move(callable),
    std::move(parameters));
  auto fexpr = dynamic_cast<const variable_expression*>(&call.get_callable());
  REQUIRE(fexpr != nullptr);
  REQUIRE(fexpr->get_variable() == f);
  REQUIRE(call.get_parameters().size() == 1);
}
