#include <catch.hpp>
#include "darcel/operations/clone_structure.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_clone_structure_literal", "[clone_structure]") {
  SECTION("Int literal") {
    auto l = std::make_unique<LiteralExpression>(Location::global(),
      Literal("123", IntegerDataType::get_instance()));
    auto m = clone_structure(*l);
    REQUIRE(m->get_literal() == l->get_literal());
  }
}
