#include <catch.hpp>
#include "darcel/operations/clone_structure.hpp"
#include "darcel/syntax/literal_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_clone_structure_literal", "[clone_structure]") {
  SECTION("Int literal") {
    auto l = std::make_unique<literal_expression>(location::global(),
      literal("123", integer_data_type::get_instance()));
    auto m = clone_structure(*l);
    REQUIRE(m->get_literal() == l->get_literal());
  }
}
