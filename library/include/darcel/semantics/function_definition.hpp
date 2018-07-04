#ifndef DARCEL_FUNCTION_DEFINITION_HPP
#define DARCEL_FUNCTION_DEFINITION_HPP
#include <memory>
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/function.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Represents a single function definition.
  class function_definition final : public element {
    public:

      //! Constructs a function_definition.
      /*!
        \param l The location of the definition.
        \param f The function being defined.
        \param t The function definition's data type.
      */
      function_definition(location l, std::shared_ptr<function> f,
        std::shared_ptr<function_data_type> t);

      //! Returns the function being defined.
      const std::shared_ptr<function>& get_function() const;

      //! Returns the function definition's data type.
      const std::shared_ptr<function_data_type>& get_type() const;

      const location& get_location() const override;

      const std::string& get_name() const override;

    private:
      location m_location;
      std::shared_ptr<function> m_function;
      std::shared_ptr<function_data_type> m_type;
  };

  inline function_definition::function_definition(location l,
      std::shared_ptr<function> f, std::shared_ptr<function_data_type> t)
      : m_location(std::move(l)),
        m_function(std::move(f)),
        m_type(std::move(t)) {}

  inline const std::shared_ptr<function>&
      function_definition::get_function() const {
    return m_function;
  }

  inline const std::shared_ptr<function_data_type>&
      function_definition::get_type() const {
    return m_type;
  }

  inline const location& function_definition::get_location() const {
    return m_location;
  }

  inline const std::string& function_definition::get_name() const {
    return m_function->get_name();
  }
}

#endif
