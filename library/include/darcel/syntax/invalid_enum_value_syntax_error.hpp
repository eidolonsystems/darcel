#ifndef DARCEL_INVALID_ENUM_VALUE_SYNTAX_ERROR_HPP
#define DARCEL_INVALID_ENUM_VALUE_SYNTAX_ERROR_HPP
#include <utility>
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"

namespace darcel {

  //! Indicates an invalid value was given for an enum.
  class InvalidEnumValueSyntaxError : public SyntaxError {
    public:

      //! Constructs an invalid enum value syntax erorr.
      /*!
        \param error_location The location of the operation.
        \param next_value The next minimum permissible value.
      */
      InvalidEnumValueSyntaxError(Location error_location, int next_value);

      //! Returns the next minimum permissible value.
      int get_next_value() const;

    private:
      int m_next_value;
  };

  inline InvalidEnumValueSyntaxError::InvalidEnumValueSyntaxError(
      Location error_location, int next_value)
      : SyntaxError(SyntaxErrorCode::INVALID_ENUM_VALUE,
          std::move(error_location)),
        m_next_value(next_value) {}

  inline int InvalidEnumValueSyntaxError::get_next_value() const {
    return m_next_value;
  }
}

#endif
