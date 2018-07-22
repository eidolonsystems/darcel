#ifndef DARCEL_VARIABLE_NOT_FOUND_ERROR_HPP
#define DARCEL_VARIABLE_NOT_FOUND_ERROR_HPP
#include <string>
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"

namespace darcel {

  //! Indicates a variable with a given name was not found.
  class VariableNotFoundError : public SyntaxError {
    public:

      //! Constructs a variable not found erorr.
      /*!
        \param l The location of the identifier.
        \param name The name of the variable not found.
      */
      VariableNotFoundError(Location l, std::string name);

      //! Returns the name of the variable that wasn't found.
      const std::string& get_name() const;

    private:
      std::string m_name;
  };

  inline VariableNotFoundError::VariableNotFoundError(Location l,
      std::string name)
      : SyntaxError(SyntaxErrorCode::VARIABLE_NOT_FOUND, std::move(l)),
        m_name(std::move(name)) {}

  inline const std::string& VariableNotFoundError::get_name() const {
    return m_name;
  }
}

#endif
