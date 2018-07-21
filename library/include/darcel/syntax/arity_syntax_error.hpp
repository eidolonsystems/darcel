#ifndef DARCEL_ARITY_SYNTAX_ERROR_HPP
#define DARCEL_ARITY_SYNTAX_ERROR_HPP
#include <utility>
#include "darcel/syntax/ops.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"

namespace darcel {

  //! Indicates an operator was passed fewer arguments than expected, ie.
  //! a division operation given only one argument (a / ???).
  class arity_syntax_error : public syntax_error {
    public:

      //! Constructs an arity syntax erorr.
      /*!
        \param error_location The location of the operation.
        \param argument_count The number of arguments passed to the operation.
        \param operation The operation called.
      */
      arity_syntax_error(Location error_location, int argument_count,
        op operation);

      //! Returns the number of arguments passed to the operation.
      int get_argument_count() const;

      //! Returns the operation called.
      op get_operation() const;

    private:
      int m_argument_count;
      op m_operation;
  };

  inline arity_syntax_error::arity_syntax_error(Location error_location,
      int argument_count, op operation)
      : syntax_error(syntax_error_code::INSUFFICIENT_ARGUMENTS,
          std::move(error_location)),
        m_argument_count(argument_count),
        m_operation(operation) {}

  inline int arity_syntax_error::get_argument_count() const {
    return m_argument_count;
  }

  inline op arity_syntax_error::get_operation() const {
    return m_operation;
  }
}

#endif
