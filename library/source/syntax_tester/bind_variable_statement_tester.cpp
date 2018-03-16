#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/lexicon/literal.hpp"
#include "darcel/lexicon/location.hpp"
#include "darcel/syntax/bind_variable_statement.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bind_variable_statement", "[bind_variable_statement]") {
  auto v = std::make_shared<variable>(location::global(), "x",
    integer_data_type::get_instance());
  auto e = std::make_unique<literal_expression>(location::global(),
    *parse_literal("123"));
  bind_variable_statement s(location::global(), v, std::move(e));
  REQUIRE(s.get_variable() == v);
  REQUIRE(*s.get_expression().get_data_type() == integer_data_type());
}
