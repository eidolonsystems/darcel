#ifndef DARCEL_GENERIC_DATA_TYPE_HPP
#define DARCEL_GENERIC_DATA_TYPE_HPP
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"
#include "darcel/data_types/function_data_type.hpp"

namespace darcel {

  //! Represents a generic data type.
  class generic_data_type : public data_type {
    public:

      //! Constructs a generic data type.
      /*!
        \param l The location where the generic was declared.
        \param name The name of the generic.
        \param index The generic's index.
      */
      generic_data_type(location l, std::string name, int index);

      //! Returns the generic's index.
      int get_index() const;

      const location& get_location() const override final;

      const std::string& get_name() const override final;

      void apply(data_type_visitor& visitor) const override final;

    protected:
      bool is_equal(const data_type& rhs) const override final;

    private:
      location m_location;
      std::string m_name;
      int m_index;
  };

  //! Tests if a data type is generic.
  //! A data type is said to be generic if it is an instance of generic
  //! data type, or it's a function that contains a parameter whose type is
  //! generic.
  /*!
    \param t The data type to test.
    \return true iff the data type is generic.
  */
  inline bool is_generic(const data_type& t) {
    if(dynamic_cast<const generic_data_type*>(&t) != nullptr) {
      return true;
    } else if(auto f = dynamic_cast<const function_data_type*>(&t)) {
      for(auto& p : f->get_parameters()) {
        if(is_generic(*p.m_type)) {
          return true;
        }
      }
    }
    return false;
  }

  //! Returns the function data type constructed from a generic function type
  //! with a specified list of arguments.
  /*!
    \param type The function data type to instantiate.
    \param arguments The list of argument data types.
    \return The function data type instantiated with the specified arguments.
  */
  inline std::shared_ptr<function_data_type> instantiate(
      const function_data_type& type,
      const std::vector<function_data_type::parameter>& arguments) {
    std::vector<function_data_type::parameter> parameters;
    data_type_map<std::shared_ptr<generic_data_type>,
      std::shared_ptr<data_type>> substitutions;
    for(std::size_t i = 0; i != arguments.size(); ++i) {
      auto& argument = arguments[i];
      auto& parameter = type.get_parameters()[i];
      if(auto generic = std::dynamic_pointer_cast<generic_data_type>(
          parameter.m_type)) {
        auto substitution = [&] {
          auto i = substitutions.find(generic);
          if(i == substitutions.end()) {
            i = substitutions.insert(std::make_pair(generic,
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
        return substitutions.at(generic);
      } else {
        return type.get_return_type();
      }
    }();
    auto instantiation = std::make_shared<function_data_type>(
      std::move(parameters), std::move(return_type));
    return instantiation;
  }

  //! Makes a generic data type.
  /*!
    \param l The declaration's location.
    \param name The name of the generic data type.
    \param index The generic's index.
  */
  inline std::shared_ptr<generic_data_type> make_generic_data_type(
      location l, std::string name, int index) {
    return std::make_shared<generic_data_type>(std::move(l), std::move(name),
      index);
  }

  //! Makes a generic data type.
  /*!
    \param name The name of the generic data type.
    \param index The generic's index.
  */
  inline std::shared_ptr<generic_data_type> make_generic_data_type(
      std::string name, int index) {
    return make_generic_data_type(location::global(), std::move(name), index);
  }

  inline generic_data_type::generic_data_type(location l, std::string name,
      int index)
      : m_location(std::move(l)),
        m_name(std::move(name)),
        m_index(index) {}

  inline int generic_data_type::get_index() const {
    return m_index;
  }

  inline const location& generic_data_type::get_location() const {
    return m_location;
  }

  inline const std::string& generic_data_type::get_name() const {
    return m_name;
  }

  inline void generic_data_type::apply(data_type_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool generic_data_type::is_equal(const data_type& rhs) const {
    auto& other = static_cast<const generic_data_type&>(rhs);
    return m_index == other.get_index();
  }

  inline void data_type_visitor::visit(const generic_data_type& node) {
    visit(static_cast<const data_type&>(node));
  }
}

#endif
