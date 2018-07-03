#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/type_inference/deduce_data_type.hpp"

using namespace darcel;

TEST_CASE("test_deduce_variable", "[deduce_data_type]") {
  type_map m;
  auto x = std::make_shared<variable>(location::global(), "x");
  m.insert(std::make_pair(x, integer_data_type::get_instance()));
  auto s = std::make_unique<variable_expression>(location::global(), x);
  auto t = deduce_data_type(*s, m);
  REQUIRE(t != nullptr);
  REQUIRE(*t == integer_data_type());
}
