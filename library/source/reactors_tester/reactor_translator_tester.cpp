#include <catch.hpp>
#include "darcel/reactors/reactor_translator.hpp"

using namespace darcel;

TEST_CASE("test_translating_literal", "[reactor_translator]") {
  trigger t;
  reactor_translator rt(t);
  auto node = bind_variable("main", make_literal_expression(123));
  rt.translate(*node);
  auto result = rt.get_main();
  auto l = std::dynamic_pointer_cast<constant_reactor<int>>(result);
  REQUIRE(l != nullptr);
}
