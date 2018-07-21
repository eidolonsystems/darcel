#include <catch.hpp>
#include "darcel/semantics/scope.hpp"
#include "darcel/semantics/variable.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_scope_lookup", "[Scope]") {
  Scope s1;
  {
    REQUIRE(s1.add(std::make_shared<Variable>(Location::global(), "abc")));
    REQUIRE(!s1.add(std::make_shared<Variable>(Location::global(), "abc")));
    REQUIRE(s1.contains("abc"));
    auto abc = s1.find<Variable>("abc");
    REQUIRE(abc != nullptr);
    REQUIRE(abc->get_name() == "abc");
    REQUIRE(s1.find_within("abc") == abc);
  }
  {
    Scope s2(&s1);
    REQUIRE(!s2.contains("abc"));
    REQUIRE(s2.find_within("abc") == nullptr);
    auto abc = s2.find<Variable>("abc");
    REQUIRE(abc != nullptr);
    REQUIRE(abc->get_name() == "abc");
  }
}
