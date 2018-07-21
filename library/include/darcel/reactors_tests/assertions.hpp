#ifndef DARCEL_REACTOR_TESTER_ASSERTIONS_HPP
#define DARCEL_REACTOR_TESTER_ASSERTIONS_HPP
#include <stdexcept>
#include <catch.hpp>
#include "darcel/reactors/reactor.hpp"

namespace darcel {
namespace tests {

  //! Exception class used for testing purposes.
  struct dummy_exception : std::exception {};

  //! Asserts that a reactor evaluates to a specified value after a sequence
  //! point.
  /*!
    \param reactor The reactor to test.
    \param sequence The sequence number the reactor is expecting.
    \param update The type of update at the <i>sequence</i>.
    \param expected The expected evaluation.
  */
  template<typename T, typename U>
  void assert_value(Reactor<T>& reactor, int sequence,
      BaseReactor::Update update, const U& expected) {
    REQUIRE(reactor.commit(sequence) == update);
    REQUIRE(reactor.commit(sequence) == update);
    REQUIRE(reactor.eval() == expected);
    if(is_complete(update)) {
      REQUIRE(reactor.commit(sequence + 1) == update);
      REQUIRE(reactor.eval() == expected);
    }
  }

  //! Asserts that a reactor throws an exception after a sequence point.
  /*!
    \param reactor The reactor to test.
    \param sequence The sequence number the reactor is expecting.
    \param update The type of update at the <i>sequence</i>.
  */
  template<typename E, typename T>
  void assert_exception(Reactor<T>& reactor, int sequence,
      BaseReactor::Update update) {
    REQUIRE(reactor.commit(sequence) == update);
    try {
      reactor.eval();
      FAIL("Expected exception not thrown.");
    } catch(const E&) {
    } catch(const std::exception&) {
      FAIL("Expected exception not thrown.");
    }
    if(is_complete(update)) {
      REQUIRE(reactor.commit(sequence + 1) == update);
    }
    try {
      reactor.eval();
      FAIL("Expected exception not thrown.");
    } catch(const E&) {
    } catch(const std::exception&) {
      FAIL("Expected exception not thrown.");
    }
  }
}
}

#endif
