#ifndef DARCEL_REACTOR_UNAVAILABLE_EXCEPTION_HPP
#define DARCEL_REACTOR_UNAVAILABLE_EXCEPTION_HPP
#include "darcel/reactors/reactor_exception.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Signals that a reactor is unavailable.
  class ReactorUnavailableException : public ReactorException {
    public:

      //! Constructs a reactor unavailable exception with a default message.
      ReactorUnavailableException() = default;

      //! Constructs a reactor unavailable exception with a message.
      /*!
        \param message A message describing the exception.
      */
      ReactorUnavailableException(const std::string& message);
  };

  inline ReactorUnavailableException::ReactorUnavailableException(
      const std::string& message)
      : ReactorException(message) {}
}

#endif
