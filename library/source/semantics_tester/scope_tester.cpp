#include <catch.hpp>
#include "darcel/semantics/scope.hpp"
#include "darcel/semantics/variable.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_scope_lookup", "[scope]") {
  scope s1;
  {
    REQUIRE(s1.add(std::make_shared<variable>(location::global(), "abc",
      integer_data_type::get_instance())));
    REQUIRE(!s1.add(std::make_shared<variable>(location::global(), "abc",
      integer_data_type::get_instance())));
    REQUIRE(s1.contains("abc"));
    auto abc = s1.find<variable>("abc");
    REQUIRE(abc != nullptr);
    REQUIRE(abc->get_name() == "abc");
    REQUIRE(s1.find_within("abc") == abc);
  }
  {
    scope s2(&s1);
    REQUIRE(!s2.contains("abc"));
    REQUIRE(s2.find_within("abc") == nullptr);
    auto abc = s2.find<variable>("abc");
    REQUIRE(abc != nullptr);
    REQUIRE(abc->get_name() == "abc");
  }
}
