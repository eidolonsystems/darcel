#ifndef DARCEL_REACTOR_ERROR_HPP
#define DARCEL_REACTOR_ERROR_HPP
#include <utility>
#include "darcel/reactors/reactor_exception.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Signals an error that can not be recovered from.
  class reactor_error : public reactor_exception {
    public:

      //! Constructs a reactor error with a default error message.
      reactor_error() = default;

      //! Constructs a reactor error with a message.
      /*!
        \param message A message describing the error.
      */
      reactor_error(const std::string& message);
  };

  inline reactor_error::reactor_error(const std::string& message)
      : reactor_exception(message) {}
}

#endif
