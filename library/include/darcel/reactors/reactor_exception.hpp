#ifndef DARCEL_REACTOR_EXCEPTION_HPP
#define DARCEL_REACTOR_EXCEPTION_HPP
#include <stdexcept>
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Signals an exception in the evaluation of a reactor.
  class ReactorException : public std::runtime_error {
    public:

      //! Constructs a reactor exception.
      ReactorException();

      //! Constructs a reactor exception.
      /*!
        \param message A message describing the exception.
      */
      ReactorException(const std::string& message);
  };

  inline ReactorException::ReactorException()
      : std::runtime_error("Reactor failed.") {}

  inline ReactorException::ReactorException(const std::string& message)
      : std::runtime_error(message) {}
}

#endif
