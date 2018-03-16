#include <catch.hpp>
#include "darcel/syntax/void_expression.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_void_expression", "[void_expression]") {
  void_expression e(location::global());
}
