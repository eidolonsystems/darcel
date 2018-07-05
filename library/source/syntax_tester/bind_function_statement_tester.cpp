#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/lexicon/literal.hpp"
#include "darcel/lexicon/location.hpp"
#include "darcel/semantics/builtin_scope.hpp"
#include "darcel/syntax/bind_function_statement.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_bind_function_statement", "[bind_function_statement]") {
  auto top_scope = make_builtin_scope();
  auto f = std::make_shared<function>(location::global(), "f");
  top_scope->add(f);
  auto& body_scope = top_scope->build_child();
  auto p1 = std::make_shared<variable>(location::global(), "a");
  body_scope.add(p1);
  auto p = std::vector<bind_function_statement::parameter>{
    {p1, integer_data_type::get_instance()}};
  auto e = std::make_unique<literal_expression>(location::global(),
    body_scope, *parse_literal("123"));
  bind_function_statement s(location::global(), *top_scope, f, p, std::move(e));
  REQUIRE(s.get_function() == f);
  REQUIRE(s.get_parameters().size() == 1);
  REQUIRE(s.get_parameters()[0].m_variable == p1);
  REQUIRE(s.get_parameters()[0].m_type.has_value());
  REQUIRE(**s.get_parameters()[0].m_type == integer_data_type());
}
