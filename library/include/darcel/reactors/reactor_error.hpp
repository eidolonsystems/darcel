#ifndef DARCEL_REACTOR_ERROR_HPP
#define DARCEL_REACTOR_ERROR_HPP
#include <utility>
#include "darcel/reactors/reactor_exception.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Signals an error that can not be recovered from.
  class ReactorError : public ReactorException {
    public:

      //! Constructs a reactor error with a default error message.
      ReactorError() = default;

      //! Constructs a reactor error with a message.
      /*!
        \param message A message describing the error.
      */
      ReactorError(const std::string& message);
  };

  inline ReactorError::ReactorError(const std::string& message)
      : ReactorException(message) {}
}

#endif
