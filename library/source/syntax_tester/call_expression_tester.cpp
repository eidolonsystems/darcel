#include <catch.hpp>
#include "darcel/syntax/call_expression.hpp"
#include "darcel/syntax/variable_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_call_expression", "[call_expression]") {

/* TODO
  call_expression call(location::global(),
    std::make_unique<variable_expression>(location::global(), "f"), {});
  auto fexpr = dynamic_cast<const variable_expression*>(&call.get_callable());
  REQUIRE(fexpr != nullptr);
  REQUIRE(fexpr->get_name() == "f");
  REQUIRE(call.get_parameters().empty());
*/
}
