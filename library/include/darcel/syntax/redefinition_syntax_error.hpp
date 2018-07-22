#ifndef DARCEL_REDEFINITION_SYNTAX_ERROR_HPP
#define DARCEL_REDEFINITION_SYNTAX_ERROR_HPP
#include <string>
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"

namespace darcel {

  //! Indicates an identifier already has a definition within the current scope.
  class RedefinitionSyntaxError : public SyntaxError {
    public:

      //! Constructs a redefinition syntax erorr.
      /*!
        \param error_location The location of the redefinition.
        \param name The name of the identifier being redefined.
        \param original_location The location of the original identifier.
      */
      RedefinitionSyntaxError(Location error_location, std::string name,
        Location original_location);

      //! Returns the name of the identifier being redefined.
      const std::string& get_name() const;

      //! Returns the location of the original identifier.
      const Location& get_original_location() const;

    private:
      std::string m_name;
      Location m_original_location;
  };

  inline RedefinitionSyntaxError::RedefinitionSyntaxError(
      Location error_location, std::string name, Location original_location)
      : SyntaxError(SyntaxErrorCode::IDENTIFIER_ALREADY_DEFINED,
          std::move(error_location)),
        m_name(std::move(name)),
        m_original_location(std::move(original_location)) {}

  inline const std::string& RedefinitionSyntaxError::get_name() const {
    return m_name;
  }

  inline const Location& RedefinitionSyntaxError::
      get_original_location() const {
    return m_original_location;
  }
}

#endif
