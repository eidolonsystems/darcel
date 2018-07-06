#ifndef DARCEL_FUNCTION_OVERLOADS_HPP
#define DARCEL_FUNCTION_OVERLOADS_HPP
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/data_types/data_type_compatibility.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/type_checker/type_checker.hpp"

namespace darcel {

  //! Substitutes a generic data type with a concrete instance, returning the
  //! type used to perform the substitution.
  /*!
    \param generic The generic data type being substituted.
    \param concrete The concrete data type to substitute for the generic.
    \param substitutions A map of all existing generic substitions.
    \return The data type used to perform the substitution or nullptr of no
            substitution can be performed.
  */
  inline std::shared_ptr<data_type> substitute_generic(
      const std::shared_ptr<data_type>& generic,
      const std::shared_ptr<data_type>& concrete,
      data_type_map<std::shared_ptr<generic_data_type>,
        std::shared_ptr<data_type>>& substitutions) {
    if(!is_generic(*generic)) {
      if(get_compatibility(*concrete, *generic) ==
          data_type_compatibility::EQUAL) {
        return concrete;
      }
      return nullptr;
    } else if(auto g = std::dynamic_pointer_cast<generic_data_type>(generic)) {
      auto substitution = substitutions.find(g);
      if(substitution == substitutions.end()) {
        substitutions.insert(std::make_pair(g, concrete));
        return concrete;
      }
      if(get_compatibility(*concrete, *substitution->second) ==
          data_type_compatibility::EQUAL || get_compatibility(*concrete,
          *generic) == data_type_compatibility::EQUAL) {
        return substitution->second;
      } else if(is_generic(*substitution->second) && !is_generic(*concrete)) {
        substitutions[g] = concrete;
        return concrete;
      }
      return nullptr;
    } else if(auto f = std::dynamic_pointer_cast<function_data_type>(generic)) {
      if(auto g = std::dynamic_pointer_cast<function_data_type>(concrete)) {
        if(f->get_parameters().size() != g->get_parameters().size()) {
          return nullptr;
        }
        std::vector<std::tuple<int, std::vector<std::shared_ptr<data_type>>>>
          candidates;
        int total_combinations = 1;
        for(auto& p : g->get_parameters()) {
          std::vector<std::shared_ptr<data_type>> candidate;
          candidate.push_back(p.m_type);
          candidates.emplace_back(total_combinations, candidate);
        }
        std::vector<std::shared_ptr<data_type>> candidate;
        candidate.push_back(g->get_return_type());
        candidates.emplace_back(total_combinations, candidate);
        for(auto c = 0; c < total_combinations; ++c) {
          auto candidate_substitutions = substitutions;
          auto passed = true;
          for(std::size_t i = 0; i < f->get_parameters().size(); ++i) {
            auto cycle = std::get<0>(candidates[i]);
            auto& types = std::get<1>(candidates[i]);
            auto& t = f->get_parameters()[i].m_type;
            auto& u = types[(c / cycle) % types.size()];
            if(substitute_generic(t, u, candidate_substitutions) == nullptr) {
              passed = false;
              break;
            }
          }
          if(passed) {
            auto cycle = std::get<0>(candidates.back());
            auto& types = std::get<1>(candidates.back());
            auto& r = types[(c / cycle) % types.size()];
            if(substitute_generic(f->get_return_type(), r,
                candidate_substitutions) != nullptr) {
              substitutions = std::move(candidate_substitutions);
              return substitute(f, substitutions);
            }
          }
        }
        return nullptr;
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
      const std::vector<function_data_type::parameter>& parameters,
      const scope& s) {
    auto& definitions = s.get_definitions(f);
    for(auto& definition : definitions) {
      auto type = std::static_pointer_cast<function_data_type>(
        definition->get_type());
      data_type_map<std::shared_ptr<generic_data_type>,
        std::shared_ptr<data_type>> substitutions;
      auto compatibility = [&] {
        if(is_generic(*type)) {
          auto t = std::make_shared<function_data_type>(parameters,
            type->get_return_type());
          auto substitution = substitute_generic(type, t, substitutions);
          if(substitution != nullptr) {
            return data_type_compatibility::GENERIC;
          }
          return data_type_compatibility::NONE;
        } else {
          if(type->get_parameters().size() != parameters.size()) {
            return data_type_compatibility::NONE;
          }
          auto compatibility = data_type_compatibility::EQUAL;
          for(std::size_t i = 0; i < parameters.size(); ++i) {
            auto parameter = parameters[i].m_type;
            auto overload_parameter = type->get_parameters()[i].m_type;
            auto parameter_compatibility = get_compatibility(*parameter,
              *overload_parameter);
            if(parameter_compatibility == data_type_compatibility::NONE) {
              return data_type_compatibility::NONE;
            } else if(parameter_compatibility ==
                data_type_compatibility::GENERIC) {
              compatibility = data_type_compatibility::GENERIC;
            }
          }
          return compatibility;
        }
      }();
      if(compatibility == data_type_compatibility::EQUAL ||
          compatibility == data_type_compatibility::GENERIC) {
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
      const function_data_type& signature, const scope& s) {
    auto overload = find_overload(f, signature.get_parameters(), s);
    if(overload == nullptr) {
      return nullptr;
    }
    auto return_type = overload->get_type()->get_return_type();
    auto return_compatibility = get_compatibility(*signature.get_return_type(),
      *return_type);
    if(return_compatibility != data_type_compatibility::EQUAL) {
      return nullptr;
    }
    return overload;
  }
}

#endif
