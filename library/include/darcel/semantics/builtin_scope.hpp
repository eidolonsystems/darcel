#ifndef DARCEL_BUILTIN_SCOPE_HPP
#define DARCEL_BUILTIN_SCOPE_HPP
#include <memory>
#include <utility>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/float_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/data_types/text_data_type.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/syntax/ops.hpp"

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
    for(auto& o : {op::ADD, op::SUBTRACT, op::MULTIPLY, op::DIVIDE}) {
      scope.add(std::make_shared<function>(location::global(),
        get_function_name(o)));
    }
  }

  //! Populates a scope with the print function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_chain(scope& scope) {
    scope.add(std::make_shared<function>(location::global(), "chain"));
  }

  //! Populates a scope with the count function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_count(scope& scope) {
    scope.add(std::make_shared<function>(location::global(), "count"));
  }

  //! Populates a scope with the first function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_first(scope& scope) {
    scope.add(std::make_shared<function>(location::global(), "first"));
  }

  //! Populates a scope with the fold function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_fold(scope& scope) {
    scope.add(std::make_shared<function>(location::global(), "fold"));
  }

  //! Populates a scope with the last function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_last(scope& scope) {
    scope.add(std::make_shared<function>(location::global(), "last"));
  }

  //! Populates a scope with the print function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_print(scope& scope) {
    scope.add(std::make_shared<function>(location::global(), "print"));
  }

  //! Populates a scope with the tally function.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_tally(scope& scope) {
    scope.add(std::make_shared<function>(location::global(), "tally"));
  }

  //! Populates a scope with all builtin elements.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_builtins(scope& scope) {
    populate_data_types(scope);
    populate_arithmetic(scope);
    populate_chain(scope);
    populate_count(scope);
    populate_first(scope);
    populate_fold(scope);
    populate_last(scope);
    populate_print(scope);
    populate_tally(scope);
  }

  //! Constructs a scope with all builtins populated.
  inline std::unique_ptr<scope> make_builtin_scope() {
    auto s = std::make_unique<scope>();
    populate_builtins(*s);
    return s;
  }
}

#endif
