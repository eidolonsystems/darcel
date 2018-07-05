#include <catch.hpp>
#include "darcel/semantics/scope.hpp"
#include "darcel/syntax/terminal_node.hpp"

using namespace darcel;

TEST_CASE("test_terminal_node", "[test_terminal_node]") {
  scope top_scope;
  terminal_node n(location::global(), top_scope);
}
