#ifndef DARCEL_INVALID_ENUM_VALUE_SYNTAX_ERROR_HPP
#define DARCEL_INVALID_ENUM_VALUE_SYNTAX_ERROR_HPP
#include <utility>
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"

namespace darcel {

  //! Indicates an invalid value was given for an enum.
  class invalid_enum_value_syntax_error : public syntax_error {
    public:

      //! Constructs an invalid enum value syntax erorr.
      /*!
        \param error_location The location of the operation.
        \param next_value The next minimum permissible value.
      */
      invalid_enum_value_syntax_error(location error_location,
        int next_value);

      //! Returns the next minimum permissible value.
      int get_next_value() const;

    private:
      int m_next_value;
  };

  inline invalid_enum_value_syntax_error::invalid_enum_value_syntax_error(
      location error_location, int next_value)
      : syntax_error(syntax_error_code::INVALID_ENUM_VALUE,
          std::move(error_location)),
        m_next_value(next_value) {}

  inline int invalid_enum_value_syntax_error::get_next_value() const {
    return m_next_value;
  }
}

#endif
