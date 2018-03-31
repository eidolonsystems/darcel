#ifndef DARCEL_FUNCTION_DATA_TYPE_HPP
#define DARCEL_FUNCTION_DATA_TYPE_HPP
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"
#include "darcel/data_types/generic_data_type.hpp"

namespace darcel {

  //! The data type used to represent functions.
  class function_data_type : public data_type {
    public:

      //! Stores a single parameter.
      struct parameter {

        //! The name of the parameter.
        std::string m_name;

        //! The parameter's type.
        std::shared_ptr<data_type> m_type;

        //! Constructs a parameter.
        parameter(std::string name, std::shared_ptr<data_type> type);
      };

      //! Constructs a function data type.
      /*!
        \param parameters The function's parameters.
        \param return_type The return type.
      */
      function_data_type(std::vector<parameter> parameters,
        std::shared_ptr<data_type> return_type);

      //! Returns the list of parameters.
      const std::vector<parameter>& get_parameters() const;

      //! Returns the return type.
      const std::shared_ptr<data_type>& get_return_type() const;

      const location& get_location() const override final;

      const std::string& get_name() const override final;

      void apply(data_type_visitor& visitor) const override final;

    protected:
      bool is_equal(const data_type& rhs) const override final;

    private:
      std::vector<parameter> m_parameters;
      std::shared_ptr<data_type> m_return_type;
      std::string m_name;
  };

  //! Makes a function data type.
  /*!
    \param parameters The function's parameters.
    \param return_type The return type.
  */
  inline std::shared_ptr<function_data_type> make_function_data_type(
      std::initializer_list<function_data_type::parameter> parameters,
      std::shared_ptr<data_type> return_type) {
    return std::make_shared<function_data_type>(
      std::vector<function_data_type::parameter>(std::move(parameters)),
      std::move(return_type));
  }

  //! Returns the function data type constructed from a generic function type
  //! with a specified list of parameters.
  /*!
    \param type The function data type to instantiate.
    \param arguments The types of the arguments being passed to the function
           data type.
    \return The function data type instantiated with the given list of
            parameters.
  */
  inline std::shared_ptr<function_data_type> instantiate(
      const function_data_type& type,
      const std::vector<function_data_type::parameter>& arguments) {
    std::vector<function_data_type::parameter> parameters;
    std::unordered_map<generic_data_type*, std::shared_ptr<data_type>>
      substitutions;
    for(std::size_t i = 0; i != arguments.size(); ++i) {
      auto& argument = arguments[i];
      auto& parameter = type.get_parameters()[i];
      if(auto generic = std::dynamic_pointer_cast<generic_data_type>(
          parameter.m_type)) {
        auto substitution = [&] {
          auto i = substitutions.find(generic.get());
          if(i == substitutions.end()) {
            i = substitutions.insert(std::make_pair(generic.get(),
              argument.m_type)).first;
          }
          return i->second;
        }();
        parameters.emplace_back(parameter.m_name, std::move(substitution));
      } else {
        parameters.push_back(argument);
      }
    }
    auto return_type = [&] {
      if(auto generic = std::dynamic_pointer_cast<generic_data_type>(
          type.get_return_type())) {
        return substitutions[generic.get()];
      } else {
        return type.get_return_type();
      }
    }();
    auto instantiation = std::make_shared<function_data_type>(
      std::move(parameters), std::move(return_type));
    return instantiation;
  }

  inline function_data_type::parameter::parameter(std::string name,
      std::shared_ptr<data_type> type)
      : m_name(std::move(name)),
        m_type(std::move(type)) {}

  inline function_data_type::function_data_type(
      std::vector<parameter> parameters, std::shared_ptr<data_type> return_type)
      : m_parameters(std::move(parameters)),
        m_return_type(std::move(return_type)) {
    m_name = "(";
    auto is_first = true;
    for(auto& parameter : m_parameters) {
      if(!is_first) {
        m_name += ", ";
      } else {
        is_first = false;
      }
      if(!parameter.m_name.empty()) {
        m_name += parameter.m_name + ": ";
      }
      m_name += parameter.m_type->get_name();
    }
    m_name += ") -> " + m_return_type->get_name();
  }

  inline const std::vector<function_data_type::parameter>&
      function_data_type::get_parameters() const {
    return m_parameters;
  }

  inline const std::shared_ptr<data_type>&
      function_data_type::get_return_type() const {
    return m_return_type;
  }

  inline const location& function_data_type::get_location() const {
    return location::global();
  }

  inline const std::string& function_data_type::get_name() const {
    return m_name;
  }

  inline void function_data_type::apply(data_type_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool function_data_type::is_equal(const data_type& rhs) const {
    auto& f = static_cast<const function_data_type&>(rhs);
    if(*m_return_type != *f.get_return_type() ||
        m_parameters.size() != f.get_parameters().size()) {
      return false;
    }
    for(std::size_t i = 0; i < m_parameters.size(); ++i) {
      if(*m_parameters[i].m_type != *f.get_parameters()[i].m_type) {
        return false;
      }
    }
    return true;
  }

  inline void data_type_visitor::visit(const function_data_type& node) {
    visit(static_cast<const data_type&>(node));
  }
}

#endif
