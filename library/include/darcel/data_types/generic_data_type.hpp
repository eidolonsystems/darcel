#ifndef DARCEL_GENERIC_DATA_TYPE_HPP
#define DARCEL_GENERIC_DATA_TYPE_HPP
#include <algorithm>
#include <functional>
#include <vector>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"
#include "darcel/data_types/function_data_type.hpp"

namespace darcel {

  //! Represents a generic data type.
  class GenericDataType final : public DataType {
    public:

      //! Constructs a generic data type.
      /*!
        \param l The location where the generic was declared.
        \param name The name of the generic.
        \param id The generic's unique id.
      */
      GenericDataType(location l, std::string name, int id);

      //! Returns the generic's id.
      int get_id() const;

      const location& get_location() const override;

      const std::string& get_name() const override;

      void apply(DataTypeVisitor& visitor) const override;

    protected:
      bool is_equal(const DataType& rhs) const override;

    private:
      location m_location;
      std::string m_name;
      int m_id;
  };

  //! Tests if a data type is generic.
  //! A data type is said to be generic if it is an instance of generic
  //! data type, or it's a function that contains a parameter whose type is
  //! generic.
  /*!
    \param t The data type to test.
    \return true iff the data type is generic.
  */
  inline bool is_generic(const DataType& t) {
    if(dynamic_cast<const GenericDataType*>(&t) != nullptr) {
      return true;
    } else if(auto f = dynamic_cast<const FunctionDataType*>(&t)) {
      for(auto& p : f->get_parameters()) {
        if(is_generic(*p.m_type)) {
          return true;
        }
      }
      return is_generic(*f->get_return_type());
    }
    return false;
  }

  //! Returns a list of generic terms composing a data type.
  /*!
    \param type The data type to extract the generic terms from.
    \return The list of generic terms in <i>type</i>
  */
  inline std::vector<std::shared_ptr<GenericDataType>> extract_generic_terms(
      std::shared_ptr<DataType> t) {
    std::vector<std::shared_ptr<GenericDataType>> result;
    if(auto g = std::dynamic_pointer_cast<GenericDataType>(t)) {
      result.push_back(std::move(g));
    } else if(auto g = std::dynamic_pointer_cast<FunctionDataType>(t)) {
      for(auto& parameter : g->get_parameters()) {
        auto sub_result = extract_generic_terms(parameter.m_type);
        std::copy_if(sub_result.begin(), sub_result.end(),
          std::back_inserter(result),
          [&] (auto& candidate) {
            return !contains(result, *candidate);
          });
      }
      auto sub_result = extract_generic_terms(g->get_return_type());
      std::copy_if(sub_result.begin(), sub_result.end(),
        std::back_inserter(result),
        [&] (auto& candidate) {
          return !contains(result, *candidate);
        });
    }
    return result;
  }

  //! Substitutes all generic occurances with their concrete arguments.
  /*!
    \param type The generic type to substitute.
    \param substitutions The mapping from generic types to concrete types.
    \return A data type whose generic occurances are replaced with concrete
            arguments based on the substitutions provided.
  */
  inline std::shared_ptr<DataType> substitute(
      const std::shared_ptr<DataType>& type, const DataTypeMap<
      std::shared_ptr<GenericDataType>, std::shared_ptr<DataType>>&
      substitutions) {
    if(auto g = std::dynamic_pointer_cast<GenericDataType>(type)) {
      auto substitution = substitutions.find(g);
      if(substitution == substitutions.end()) {
        return g;
      }
      return substitution->second;
    } else if(auto f = std::dynamic_pointer_cast<FunctionDataType>(type)) {
      std::vector<FunctionDataType::Parameter> parameters;
      for(auto& parameter : f->get_parameters()) {
        parameters.emplace_back(parameter.m_name,
          substitute(parameter.m_type, substitutions));
      }
      auto return_substitution = substitute(f->get_return_type(),
        substitutions);
      auto substitution = std::make_shared<FunctionDataType>(
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
  inline std::shared_ptr<GenericDataType> make_generic_data_type(
      location l, std::string name, int index) {
    return std::make_shared<GenericDataType>(std::move(l), std::move(name),
      index);
  }

  //! Makes a generic data type.
  /*!
    \param name The name of the generic data type.
    \param index The generic's index.
  */
  inline std::shared_ptr<GenericDataType> make_generic_data_type(
      std::string name, int index) {
    return make_generic_data_type(location::global(), std::move(name), index);
  }

  inline GenericDataType::GenericDataType(location l, std::string name,
      int id)
      : m_location(std::move(l)),
        m_name(std::move(name)),
        m_id(id) {}

  inline int GenericDataType::get_id() const {
    return m_id;
  }

  inline const location& GenericDataType::get_location() const {
    return m_location;
  }

  inline const std::string& GenericDataType::get_name() const {
    return m_name;
  }

  inline void GenericDataType::apply(DataTypeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool GenericDataType::is_equal(const DataType& rhs) const {
    auto& other = static_cast<const GenericDataType&>(rhs);
    return m_id == other.get_id();
  }

  inline void DataTypeVisitor::visit(const GenericDataType& node) {
    visit(static_cast<const DataType&>(node));
  }
}

#endif
