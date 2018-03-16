#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/syntax/variable_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_variable_expression", "[variable_expression]") {
  auto v = std::make_shared<variable>(location::global(), "abc",
    integer_data_type::get_instance());
  variable_expression e(location::global(), v);
  REQUIRE(e.get_variable() == v);
}
