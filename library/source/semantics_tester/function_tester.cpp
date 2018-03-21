#include <catch.hpp>
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/semantics/function.hpp"

using namespace darcel;

TEST_CASE("test_function", "[function]") {
  auto v = std::make_shared<variable>(location::global(), "f",
    make_function_data_type(
    {{"a", integer_data_type::get_instance()}},
    integer_data_type::get_instance()));
}
