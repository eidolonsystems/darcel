#ifndef DARCEL_REDEFINITION_SYNTAX_ERROR_HPP
#define DARCEL_REDEFINITION_SYNTAX_ERROR_HPP
#include <string>
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"

namespace darcel {

  //! Indicates an identifier already has a definition within the current scope.
  class redefinition_syntax_error : public syntax_error {
    public:

      //! Constructs a redefinition syntax erorr.
      /*!
        \param error_location The location of the redefinition.
        \param name The name of the identifier being redefined.
        \param original_location The location of the original identifier.
      */
      redefinition_syntax_error(location error_location, std::string name,
        location original_location);

      //! Returns the name of the identifier being redefined.
      const std::string& get_name() const;

      //! Returns the location of the original identifier.
      const location& get_original_location() const;

    private:
      std::string m_name;
      location m_original_location;
  };

  inline redefinition_syntax_error::redefinition_syntax_error(
      location error_location, std::string name, location original_location)
      : syntax_error(syntax_error_code::IDENTIFIER_ALREADY_DEFINED,
          std::move(error_location)),
        m_name(std::move(name)),
        m_original_location(std::move(original_location)) {}

  inline const std::string& redefinition_syntax_error::get_name() const {
    return m_name;
  }

  inline const location& redefinition_syntax_error::
      get_original_location() const {
    return m_original_location;
  }
}

#endif
