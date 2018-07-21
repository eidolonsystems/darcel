#include <catch.hpp>
#include "darcel/data_types/callable_data_type.hpp"

using namespace darcel;
using namespace std;

TEST_CASE("test_callable_name", "[CallableDataType]") {
  auto f = std::make_shared<function>(Location::global(), "f");
  auto t = std::make_shared<CallableDataType>(f);
  REQUIRE(t->get_name() == "@f");
  REQUIRE(t->get_function() == f);
}

TEST_CASE("test_callable_equality", "[CallableDataType]") {
  auto f1 = std::make_shared<function>(Location::global(), "f");
  auto f2 = std::make_shared<function>(Location::global(), "f");
  auto t1 = std::make_shared<CallableDataType>(f1);
  auto t2 = std::make_shared<CallableDataType>(f2);
  auto t3 = std::make_shared<CallableDataType>(f1);
  REQUIRE(*t1 != *t2);
  REQUIRE(*t1 == *t3);
  REQUIRE(*t2 != *t3);
}
