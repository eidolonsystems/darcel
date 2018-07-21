#ifndef DARCEL_FUNCTION_DEFINITION_HPP
#define DARCEL_FUNCTION_DEFINITION_HPP
#include <memory>
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/semantics/function.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Represents a single function definition.
  class FunctionDefinition final : public Element {
    public:

      //! Constructs a function_definition.
      /*!
        \param l The location of the definition.
        \param f The function being defined.
        \param t The function definition's data type.
      */
      FunctionDefinition(Location l, std::shared_ptr<Function> f,
        std::shared_ptr<FunctionDataType> t);

      //! Returns the function being defined.
      const std::shared_ptr<Function>& get_function() const;

      //! Returns the function definition's data type.
      const std::shared_ptr<FunctionDataType>& get_type() const;

      const Location& get_location() const override;

      const std::string& get_name() const override;

    private:
      Location m_location;
      std::shared_ptr<Function> m_function;
      std::shared_ptr<FunctionDataType> m_type;
  };

  inline FunctionDefinition::FunctionDefinition(Location l,
      std::shared_ptr<Function> f, std::shared_ptr<FunctionDataType> t)
      : m_location(std::move(l)),
        m_function(std::move(f)),
        m_type(std::move(t)) {}

  inline const std::shared_ptr<Function>&
      FunctionDefinition::get_function() const {
    return m_function;
  }

  inline const std::shared_ptr<FunctionDataType>&
      FunctionDefinition::get_type() const {
    return m_type;
  }

  inline const Location& FunctionDefinition::get_location() const {
    return m_location;
  }

  inline const std::string& FunctionDefinition::get_name() const {
    return m_function->get_name();
  }
}

#endif
