#ifndef DARCEL_FUNCTION_DEFINITION_HPP
#define DARCEL_FUNCTION_DEFINITION_HPP
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/syntax/bind_function_statement.hpp"

namespace darcel {

  //! Represents a single function definition.
  class function_definition final : public element {
    public:

      //! Constructs a function_definition.
      /*!
        \param definition The statement defining the function.
      */
      function_definition(const bind_function_statement& definition);

      //! Returns the function definition.
      const bind_function_statement& get_definition() const;

      const location& get_location() const override;

      const std::string& get_name() const override;

    private:
      const bind_function_statement* m_definition;
  };

  inline function_definition::function_definition(
      const bind_function_statement& definition)
      : m_definition(&definition) {}

  inline const bind_function_statement&
      function_definition::get_definition() const {
    return *m_definition;
  }

  inline const std::shared_ptr<function_data_type>&
      function_definition::get_data_type() const {
    return m_data_type;
  }

  inline const location& function_definition::get_location() const {
    return m_location;
  }

  inline const std::string& function_definition::get_name() const {
    return m_function->get_name();
  }
}

#endif
