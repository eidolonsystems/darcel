#ifndef DARCEL_FUNCTION_HPP
#define DARCEL_FUNCTION_HPP
#include <vector>
#include "darcel/data_types/data_type_compatibility.hpp"
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/semantics/variable.hpp"

namespace darcel {

  //! Represents a potentially overloaded, named function.
  class function : public element {
    public:

      //! Constructs a function.
      /*!
        \param initial The initial overload.
      */
      function(std::shared_ptr<variable> initial);

      //! Returns the parent of this function.
      const std::shared_ptr<function>& get_parent() const;

      //! Returns the list of overloads.
      const std::vector<std::shared_ptr<variable>>& get_overloads() const;

      //! Adds an overload to this function.
      /*!
        \return true iff the overload was added, otherwise the overload clashes
                with an existing overload.
      */
      bool add(std::shared_ptr<variable> overload);

      const location& get_location() const override final;

      const std::string& get_name() const override final;

    private:
      std::shared_ptr<function> m_parent;
      std::vector<std::shared_ptr<variable>> m_overloads;
  };

  //! Finds the function overload matching a set of parameters.
  /*!
    \param f The overloaded function to search over.
    \param parameters The parameter's to search.
    \return The variable representing the specific function overload.
  */
  inline std::shared_ptr<variable> find_overload(function& f,
      const std::vector<function_data_type::parameter>& parameters) {
    for(auto& overload : f.get_overloads()) {
      auto type = std::static_pointer_cast<function_data_type>(
        overload->get_data_type());
      if(type->get_parameters().size() != parameters.size()) {
        continue;
      }
      auto compatibility = [&] {
        auto compatibility = data_type_compatibility::EQUAL;
        for(std::size_t i = 0; i < parameters.size(); ++i) {
          auto parameter_compatibility = get_compatibility(
            *parameters[i].m_type, *type->get_parameters()[i].m_type);
          if(parameter_compatibility == data_type_compatibility::NONE) {
            return data_type_compatibility::NONE;
          } else if(parameter_compatibility ==
              data_type_compatibility::GENERIC) {
            compatibility = data_type_compatibility::GENERIC;
          }
        }
        return compatibility;
      }();
      if(compatibility == data_type_compatibility::EQUAL) {
        return overload;
      } else if(compatibility == data_type_compatibility::GENERIC) {
        auto t = instantiate(*type, parameters);
        auto instantiation = std::make_shared<variable>(f.get_location(),
          f.get_name(), std::move(t));
        f.add(instantiation);
        return instantiation;
      }
    }
    if(f.get_parent() != nullptr) {
      return find_overload(*f.get_parent(), parameters);
    }
    return nullptr;
  }

  inline function::function(std::shared_ptr<variable> initial) {
    m_overloads.push_back(std::move(initial));
  }

  inline const std::vector<std::shared_ptr<variable>>&
      function::get_overloads() const {
    return m_overloads;
  }

  inline const std::shared_ptr<function>& function::get_parent() const {
    return m_parent;
  }

  inline bool function::add(std::shared_ptr<variable> overload) {
    auto overload_type = std::dynamic_pointer_cast<function_data_type>(
      overload->get_data_type());
    if(overload_type == nullptr) {
      return false;
    }
    auto has_overload = false;
    auto& overload_parameters = overload_type->get_parameters();
    for(auto& instance : m_overloads) {
      auto instance_type = std::static_pointer_cast<function_data_type>(
        instance->get_data_type());
      auto& instance_parameters = instance_type->get_parameters();
      if(overload_parameters.size() != instance_parameters.size()) {
        continue;
      }
      std::unordered_map<std::shared_ptr<generic_data_type>, int>
        generic_parameters;
      auto parameters_match = true;
      for(std::size_t i = 0; i < overload_parameters.size(); ++i) {
        auto& overload_parameter = overload_parameters[i].m_type;
        auto& instance_parameter = instance_parameters[i].m_type;
        auto overload_generic = std::dynamic_pointer_cast<generic_data_type>(
          overload_parameter);
        auto instance_generic = std::dynamic_pointer_cast<generic_data_type>(
          instance_parameter);
        if(overload_generic == nullptr && instance_generic == nullptr) {
          if(*overload_parameter != *instance_parameter) {
            parameters_match = false;
            break;
          }
        } else if(overload_generic == nullptr && instance_generic != nullptr ||
            overload_generic != nullptr && instance_generic == nullptr) {
          parameters_match = false;
          break;
        } else {
          auto overload_index = generic_parameters.find(overload_generic);
          auto instance_index = generic_parameters.find(instance_generic);
          if(overload_index != generic_parameters.end() &&
              instance_index != generic_parameters.end()) {
            if(overload_index->second != instance_index->second) {
              parameters_match = false;
              break;
            }
          } else if(overload_index == generic_parameters.end() &&
              instance_index == generic_parameters.end()) {
            auto index = static_cast<int>(generic_parameters.size());
            generic_parameters.insert(std::make_pair(overload_generic, index));
            generic_parameters.insert(std::make_pair(instance_generic, index));
          } else {
            parameters_match = false;
            break;
          }
        }
      }
      if(parameters_match) {
        has_overload = true;
        break;
      }
    }
    if(has_overload) {
      return false;
    }
    m_overloads.push_back(std::move(overload));
    return true;
  }

  inline const location& function::get_location() const {
    return m_overloads.front()->get_location();
  }

  inline const std::string& function::get_name() const {
    return m_overloads.front()->get_name();
  }
}

#endif
