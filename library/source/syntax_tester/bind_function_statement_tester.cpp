#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/lexicon/literal.hpp"
#include "darcel/lexicon/location.hpp"
#include "darcel/syntax/bind_function_statement.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bind_function_statement", "[bind_function_statement]") {
  auto t = make_function_data_type({{"a", integer_data_type::get_instance()}},
    integer_data_type::get_instance());
  auto v = std::make_shared<variable>(location::global(), "f", t);
  auto f = std::make_shared<function>(v);
  auto e = std::make_unique<literal_expression>(location::global(),
    *parse_literal("123"));
  bind_function_statement s(location::global(), f, v, std::move(e));
  REQUIRE(s.get_function() == f);
  REQUIRE(s.get_overload() == v);
  REQUIRE(*s.get_expression().get_data_type() == integer_data_type());
}
