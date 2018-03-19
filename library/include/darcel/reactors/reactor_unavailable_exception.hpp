#ifndef DARCEL_REACTOR_UNAVAILABLE_EXCEPTION_HPP
#define DARCEL_REACTOR_UNAVAILABLE_EXCEPTION_HPP
#include "darcel/reactors/reactor_exception.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Signals that a reactor is unavailable.
  class reactor_unavailable_exception : public reactor_exception {
    public:

      //! Constructs a reactor unavailable exception with a default message.
      reactor_unavailable_exception() = default;

      //! Constructs a reactor unavailable exception with a message.
      /*!
        \param message A message describing the exception.
      */
      reactor_unavailable_exception(const std::string& message);
  };

  inline reactor_unavailable_exception::reactor_unavailable_exception(
      const std::string& message)
      : reactor_exception(message) {}
}

#endif
