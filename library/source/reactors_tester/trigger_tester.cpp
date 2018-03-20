#include <catch.hpp>
#include "darcel/reactors/trigger.hpp"

using namespace darcel;

TEST_CASE("test_trigger_update", "[trigger]") {
  trigger t;
  int sequenceNumber;
  REQUIRE(t.get_current_sequence() == 0);
  t.fetch_next_sequence(sequenceNumber);
  REQUIRE(sequenceNumber == 1);
  t.wait();
  REQUIRE(t.get_current_sequence() == 1);
  t.fetch_next_sequence(sequenceNumber);
  REQUIRE(sequenceNumber == 2);
}
