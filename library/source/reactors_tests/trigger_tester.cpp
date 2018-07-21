#include <catch.hpp>
#include "darcel/reactors/trigger.hpp"

using namespace darcel;

TEST_CASE("test_trigger_update", "[Trigger]") {
  Trigger t;
  auto sequence = t.get_current_sequence();
  REQUIRE(sequence == t.get_current_sequence());
  t.signal_update();
  t.wait(sequence);
  ++sequence;
  REQUIRE(t.get_current_sequence() == 1);
  t.signal_update();
  t.wait(sequence);
  ++sequence;
  REQUIRE(t.get_current_sequence() == 2);
}
