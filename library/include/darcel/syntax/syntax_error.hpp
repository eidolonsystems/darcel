#ifndef DARCEL_SYNTAX_ERROR_HPP
#define DARCEL_SYNTAX_ERROR_HPP
#include <stdexcept>
#include <utility>
#include "darcel/lexicon/location.hpp"
#include "darcel/syntax/syntax_error_code.hpp"
#include "darcel/syntax/syntax.hpp"

namespace darcel {

  //! Indicates a syntax error.
  class SyntaxError : public std::runtime_error {
    public:

      //! Constructs a syntax error.
      /*!
        \param code The error code.
        \param location The location in the source code of the error.
      */
      SyntaxError(SyntaxErrorCode code, Location location);

      //! Returns the error code.
      SyntaxErrorCode get_code() const;

      //! Returns the location of the error.
      const Location& get_location() const;

    private:
      SyntaxErrorCode m_code;
      Location m_location;
  };

  inline SyntaxError::SyntaxError(SyntaxErrorCode code, Location location)
      : std::runtime_error("Syntax error."),
        m_code(code),
        m_location(std::move(location)) {}

  inline SyntaxErrorCode SyntaxError::get_code() const {
    return m_code;
  }

  inline const Location& SyntaxError::get_location() const {
    return m_location;
  }
}

#endif
