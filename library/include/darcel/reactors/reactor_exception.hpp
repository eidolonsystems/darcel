#ifndef DARCEL_REACTOR_EXCEPTION_HPP
#define DARCEL_REACTOR_EXCEPTION_HPP
#include <stdexcept>
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Signals an exception in the evaluation of a reactor.
  class reactor_exception : public std::runtime_error {
    public:

      //! Constructs a reactor exception.
      reactor_exception();

      //! Constructs a reactor exception.
      /*!
        \param message A message describing the exception.
      */
      reactor_exception(const std::string& message);
  };

  inline reactor_exception::reactor_exception()
      : std::runtime_error("Reactor failed.") {}

  inline reactor_exception::reactor_exception(const std::string& message)
      : std::runtime_error(message) {}
}

#endif
