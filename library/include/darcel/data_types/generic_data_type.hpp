#ifndef DARCEL_GENERIC_DATA_TYPE_HPP
#define DARCEL_GENERIC_DATA_TYPE_HPP
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"
#include "darcel/data_types/function_data_type.hpp"

namespace darcel {

  //! Represents a generic data type.
  class generic_data_type final : public data_type {
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

      const location& get_location() const override;

      const std::string& get_name() const override;

      void apply(data_type_visitor& visitor) const override;

    protected:
      bool is_equal(const data_type& rhs) const override;

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
      return is_generic(*f->get_return_type());
    }
    return false;
  }

  //! Substitutes all generic occurances with their concrete arguments.
  /*!
    \param type The generic type to substitute.
    \param substitutions The mapping from generic types to concrete types.
    \return A data type whose generic occurances are replaced with concrete
            arguments based on the substitutions provided.
  */
  inline std::shared_ptr<data_type> substitute(
      const std::shared_ptr<data_type>& type, const data_type_map<
      std::shared_ptr<generic_data_type>, std::shared_ptr<data_type>>&
      substitutions) {
    if(auto g = std::dynamic_pointer_cast<generic_data_type>(type)) {
      return substitutions.at(g);
    } else if(auto f = std::dynamic_pointer_cast<function_data_type>(type)) {
      std::vector<function_data_type::parameter> parameters;
      for(auto& parameter : f->get_parameters()) {
        parameters.emplace_back(parameter.m_name,
          substitute(parameter.m_type, substitutions));
      }
      auto return_substitution = substitute(f->get_return_type(),
        substitutions);
      auto substitution = std::make_shared<function_data_type>(
        std::move(parameters), std::move(return_substitution));
      if(*substitution == *f) {
        return f;
      }
      return substitution;
    }
    return type;
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
