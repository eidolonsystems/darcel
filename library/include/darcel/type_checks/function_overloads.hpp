#ifndef DARCEL_FUNCTION_OVERLOADS_HPP
#define DARCEL_FUNCTION_OVERLOADS_HPP
#include <vector>
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/data_types/data_type_compatibility.hpp"
#include "darcel/semantics/function.hpp"
#include "darcel/semantics/function_definition.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {

  //! Substitutes a generic data type with a concrete instance, returning the
  //! type used to perform the substitution.
  /*!
    \param generic The generic data type being substituted.
    \param concrete The concrete data type to substitute for the generic.
    \param s The scope containing the generic and concrete data types.
    \param substitutions A map of all existing generic substitions.
    \return The data type used to perform the substitution or nullptr of no
            substitution can be performed.
  */
  inline std::shared_ptr<DataType> substitute_generic(
      const std::shared_ptr<DataType>& generic,
      const std::shared_ptr<DataType>& concrete, const scope& s,
      DataTypeMap<std::shared_ptr<GenericDataType>,
        std::shared_ptr<DataType>>& substitutions) {
    if(!is_generic(*generic)) {
      if(get_compatibility(*concrete, *generic, s) ==
          DataTypeCompatibility::EQUAL) {
        return concrete;
      }
      return nullptr;
    } else if(auto g = std::dynamic_pointer_cast<GenericDataType>(generic)) {
      auto substitution = substitutions.find(g);
      if(substitution == substitutions.end()) {
        substitutions.insert(std::make_pair(g, concrete));
        return concrete;
      }
      if(get_compatibility(*concrete, *substitution->second, s) ==
          DataTypeCompatibility::EQUAL ||
          get_compatibility(*concrete, *generic, s) ==
          DataTypeCompatibility::EQUAL) {
        return substitution->second;
      } else if(is_generic(*substitution->second) && !is_generic(*concrete)) {
        substitutions[g] = concrete;
        return concrete;
      }
      return nullptr;
    } else if(auto f = std::dynamic_pointer_cast<FunctionDataType>(generic)) {
      if(auto g = std::dynamic_pointer_cast<FunctionDataType>(concrete)) {
        if(f->get_parameters().size() != g->get_parameters().size()) {
          return nullptr;
        }
        std::vector<std::tuple<int, std::vector<std::shared_ptr<DataType>>>>
          candidates;
        int total_combinations = 1;
        auto add_combinations = [&] (auto& type) {
          if(auto c = std::dynamic_pointer_cast<CallableDataType>(type)) {
            std::vector<std::shared_ptr<DataType>> overloads;
            s.find(*c->get_function(),
              [&] (auto& definition) {
                overloads.push_back(definition.get_type());
                return false;
              });
            auto combinations = static_cast<int>(overloads.size());
            candidates.emplace_back(total_combinations, std::move(overloads));
            total_combinations *= combinations;
          } else {
            std::vector<std::shared_ptr<DataType>> candidate;
            candidate.push_back(type);
            candidates.emplace_back(total_combinations, candidate);
          }
        };
        for(auto& p : g->get_parameters()) {
          add_combinations(p.m_type);
        }
        add_combinations(g->get_return_type());
        for(auto c = 0; c < total_combinations; ++c) {
          auto candidate_substitutions = substitutions;
          auto passed = true;
          for(std::size_t i = 0; i < f->get_parameters().size(); ++i) {
            auto cycle = std::get<0>(candidates[i]);
            auto& types = std::get<1>(candidates[i]);
            auto& t = f->get_parameters()[i].m_type;
            auto& u = types[(c / cycle) % types.size()];
            if(substitute_generic(t, u, s, candidate_substitutions) ==
                nullptr) {
              passed = false;
              break;
            }
          }
          if(passed) {
            auto cycle = std::get<0>(candidates.back());
            auto& types = std::get<1>(candidates.back());
            auto& r = types[(c / cycle) % types.size()];
            if(substitute_generic(f->get_return_type(), r, s,
                candidate_substitutions) != nullptr) {
              substitutions = std::move(candidate_substitutions);
              return substitute(f, substitutions);
            }
          }
        }
        return nullptr;
      } else if(auto g =
          std::dynamic_pointer_cast<CallableDataType>(concrete)) {
        auto definition = s.find(*g->get_function(),
          [&] (auto& definition) {
            auto candidate_substitutions = substitutions;
            if(substitute_generic(generic, definition.get_type(), s,
                candidate_substitutions)) {
              substitutions = std::move(candidate_substitutions);
              return true;
            }
            return false;
          });
        if(definition != nullptr) {
          return substitute(f, substitutions);
        } else {
          return nullptr;
        }
      }
      return nullptr;
    } else {
      return nullptr;
    }
  }

  //! Finds the function definition matching a set of parameters.
  /*!
    \param f The overloaded function to search over.
    \param parameters The parameters that the overload must match.
    \param s The scope to search through.
    \return The function definition representing the matching overload or
            nullptr if no such match exists.
  */
  inline std::shared_ptr<function_definition> find_overload(const function& f,
      const std::vector<FunctionDataType::Parameter>& parameters,
      const scope& s) {
    auto& definitions = s.get_definitions(f);
    for(auto& definition : definitions) {
      auto type = std::static_pointer_cast<FunctionDataType>(
        definition->get_type());
      DataTypeMap<std::shared_ptr<GenericDataType>,
        std::shared_ptr<DataType>> substitutions;
      auto compatibility = [&] {
        if(is_generic(*type)) {
          auto t = std::make_shared<FunctionDataType>(parameters,
            type->get_return_type());
          auto substitution = substitute_generic(type, t, s, substitutions);
          if(substitution != nullptr) {
            return DataTypeCompatibility::GENERIC;
          }
          return DataTypeCompatibility::NONE;
        } else {
          if(type->get_parameters().size() != parameters.size()) {
            return DataTypeCompatibility::NONE;
          }
          auto compatibility = DataTypeCompatibility::EQUAL;
          for(std::size_t i = 0; i < parameters.size(); ++i) {
            auto parameter = parameters[i].m_type;
            auto overload_parameter = type->get_parameters()[i].m_type;
            auto parameter_compatibility = get_compatibility(*parameter,
              *overload_parameter, s);
            if(parameter_compatibility == DataTypeCompatibility::NONE) {
              return DataTypeCompatibility::NONE;
            } else if(parameter_compatibility ==
                DataTypeCompatibility::GENERIC) {
              compatibility = DataTypeCompatibility::GENERIC;
            }
          }
          return compatibility;
        }
      }();
      if(compatibility == DataTypeCompatibility::EQUAL ||
          compatibility == DataTypeCompatibility::GENERIC) {
        return definition;
      }
    }
    if(s.get_parent() != nullptr) {
      return find_overload(f, parameters, *s.get_parent());
    }
    return nullptr;
  }

  //! Finds the function overload matching a signature.
  /*!
    \param f The overloaded function to search over.
    \param signature The function data type to match.
    \param s The scope to search through.
    \return The function definition matching the specific function overload.
  */
  inline std::shared_ptr<function_definition> find_overload(const function& f,
      const FunctionDataType& signature, const scope& s) {
    auto overload = find_overload(f, signature.get_parameters(), s);
    if(overload == nullptr) {
      return nullptr;
    }
    auto return_type = overload->get_type()->get_return_type();
    auto return_compatibility = get_compatibility(*signature.get_return_type(),
      *return_type, s);
    if(return_compatibility == DataTypeCompatibility::EQUAL ||
        return_compatibility == DataTypeCompatibility::GENERIC) {
      return overload;
    }
    return nullptr;
  }

  //! Instantiates a generic function called with a given list of parameters.
  /*!
    \param f The function definition to instantiate.
    \param p The parameters to instantiate the function with.
    \param s The scope containing the parameters.
    \return The function type representing the instantiation of f with
            parameters p.
  */
  inline std::shared_ptr<FunctionDataType> instantiate(
      const function_definition& f,
      const std::vector<FunctionDataType::Parameter>& p, const scope& s) {
    if(is_generic(*f.get_type())) {
      DataTypeMap<std::shared_ptr<GenericDataType>,
        std::shared_ptr<DataType>> substitutions;
      auto t = std::make_shared<FunctionDataType>(p,
        f.get_type()->get_return_type());
      return std::static_pointer_cast<FunctionDataType>(
        substitute_generic(f.get_type(), t, s, substitutions));
    } else {
      return f.get_type();
    }
  }
}

#endif
