#ifndef DARCEL_BUILTIN_SCOPE_HPP
#define DARCEL_BUILTIN_SCOPE_HPP
#include <memory>
#include <utility>
#include <vector>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/float_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/data_types/text_data_type.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Populates a scope with all builtin data types.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_data_types(scope& scope) {
    scope.add(bool_data_type::get_instance());
    scope.add(float_data_type::get_instance());
    scope.add(integer_data_type::get_instance());
    scope.add(text_data_type::get_instance());
  }

  //! Populates a scope with builtin arithmetic operators.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_arithmetic(scope& scope) {
    for(auto& t : std::vector<std::shared_ptr<data_type>>{
        integer_data_type::get_instance(), float_data_type::get_instance()}) {
      std::vector<function_data_type::parameter> parameters;
      parameters.push_back({"lhs", t});
      parameters.push_back({"rhs", t});
      for(auto& o : {op::ADD, op::SUBTRACT, op::MULTIPLY, op::DIVIDE}) {
        auto f = std::make_shared<variable>(location::global(),
          get_function_name(o),
          std::make_shared<function_data_type>(parameters, t));
        scope.add(f);
      }
    }
    {
      std::vector<function_data_type::parameter> parameters;
      parameters.push_back({"lhs", text_data_type::get_instance()});
      parameters.push_back({"rhs", text_data_type::get_instance()});
      auto name = get_function_name(op::ADD);
      auto f = std::make_shared<variable>(location::global(), name,
        std::make_shared<function_data_type>(parameters,
        text_data_type::get_instance()));
      scope.add(f);
    }
  }

  //! Populates a scope with the print function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_chain(scope& scope) {
    std::vector<function_data_type::parameter> parameters;
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    parameters.push_back({"initial", t});
    parameters.push_back({"continuation", t});
    auto f = std::make_shared<variable>(location::global(), "chain",
      std::make_shared<function_data_type>(parameters, t));
    scope.add(f);
  }

  //! Populates a scope with the first function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_first(scope& scope) {
    std::vector<function_data_type::parameter> parameters;
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    parameters.push_back({"v", t});
    auto f = std::make_shared<variable>(location::global(), "first",
      std::make_shared<function_data_type>(parameters, t));
    scope.add(f);
  }

  //! Populates a scope with the last function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_last(scope& scope) {
    std::vector<function_data_type::parameter> parameters;
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    parameters.push_back({"v", t});
    auto f = std::make_shared<variable>(location::global(), "last",
      std::make_shared<function_data_type>(parameters, t));
    scope.add(f);
  }

  //! Populates a scope with the print function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_print(scope& scope) {
    std::vector<function_data_type::parameter> parameters;
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    parameters.push_back({"v", t});
    auto f = std::make_shared<variable>(location::global(), "print",
      std::make_shared<function_data_type>(parameters, t));
    scope.add(f);
  }

  //! Populates a scope with all builtin elements.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_builtins(scope& scope) {
    populate_data_types(scope);
    populate_arithmetic(scope);
    populate_chain(scope);
    populate_first(scope);
    populate_last(scope);
    populate_print(scope);
  }

  //! Constructs a scope with all builtins populated.
  inline std::unique_ptr<scope> make_builtin_scope() {
    auto s = std::make_unique<scope>();
    populate_builtins(*s);
    return s;
  }
}

#endif
