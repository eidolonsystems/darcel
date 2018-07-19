#ifndef DARCEL_BUILTIN_SCOPE_HPP
#define DARCEL_BUILTIN_SCOPE_HPP
#include <memory>
#include <utility>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/callable_data_type.hpp"
#include "darcel/data_types/float_data_type.hpp"
#include "darcel/data_types/generic_data_type.hpp"
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
    \param s The scope to populate.
  */
  inline void populate_arithmetic(scope& s) {
    for(auto& o : {op::ADD, op::SUBTRACT, op::MULTIPLY, op::DIVIDE}) {
      auto f = std::make_shared<function>(location::global(),
        get_function_name(o));
      s.add(f);
      s.add(std::make_shared<callable_data_type>(f));
      for(auto& t : std::vector<std::shared_ptr<data_type>>{
          integer_data_type::get_instance(), float_data_type::get_instance()}) {
        std::vector<function_data_type::parameter> parameters;
        parameters.emplace_back("left", t);
        parameters.emplace_back("right", t);
        auto d = std::make_shared<function_definition>(location::global(), f,
          std::make_shared<function_data_type>(parameters, t));
        s.add(d);
      }
    }
    {
      std::vector<function_data_type::parameter> parameters;
      parameters.emplace_back("left", text_data_type::get_instance());
      parameters.emplace_back("right", text_data_type::get_instance());
      auto name = get_function_name(op::ADD);
      auto f = s.find<function>(name);
      auto d = std::make_shared<function_definition>(location::global(), f,
        std::make_shared<function_data_type>(parameters,
        text_data_type::get_instance()));
      s.add(d);
    }
  }

  //! Populates a scope with the print function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_chain(scope& s) {
    auto f = std::make_shared<function>(location::global(), "chain");
    s.add(f);
    std::vector<function_data_type::parameter> parameters;
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    parameters.emplace_back("initial", t);
    parameters.emplace_back("continuation", t);
    auto d = std::make_shared<function_definition>(location::global(), f,
      std::make_shared<function_data_type>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the count function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_count(scope& s) {
    auto f = std::make_shared<function>(location::global(), "count");
    s.add(f);
    std::vector<function_data_type::parameter> parameters;
    parameters.emplace_back("start", integer_data_type::get_instance());
    parameters.emplace_back("end", integer_data_type::get_instance());
    auto d = std::make_shared<function_definition>(location::global(), f,
      std::make_shared<function_data_type>(parameters,
      integer_data_type::get_instance()));
    s.add(d);
  }

  //! Populates a scope with the first function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_first(scope& s) {
    auto f = std::make_shared<function>(location::global(), "first");
    s.add(f);
    std::vector<function_data_type::parameter> parameters;
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    parameters.emplace_back("source", t);
    auto d = std::make_shared<function_definition>(location::global(), f,
      std::make_shared<function_data_type>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the fold function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_fold(scope& s) {
    auto f = std::make_shared<function>(location::global(), "fold");
    s.add(f);
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    std::vector<function_data_type::parameter> parameters;
    auto r = [&] {
      std::vector<function_data_type::parameter> parameters;
      parameters.emplace_back("left", t);
      parameters.emplace_back("right", t);
      return std::make_shared<function_data_type>(std::move(parameters), t);
    }();
    parameters.emplace_back("f", std::move(r));
    parameters.emplace_back("r", t);
    auto d = std::make_shared<function_definition>(location::global(), f,
      std::make_shared<function_data_type>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the last function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_last(scope& s) {
    auto f = std::make_shared<function>(location::global(), "last");
    s.add(f);
    std::vector<function_data_type::parameter> parameters;
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    parameters.emplace_back("source", t);
    auto d = std::make_shared<function_definition>(location::global(), f,
      std::make_shared<function_data_type>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the print function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_print(scope& s) {
    auto f = std::make_shared<function>(location::global(), "print");
    s.add(f);
    std::vector<function_data_type::parameter> parameters;
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    parameters.emplace_back("source", t);
    auto d = std::make_shared<function_definition>(location::global(), f,
      std::make_shared<function_data_type>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the tally function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_tally(scope& s) {
    auto f = std::make_shared<function>(location::global(), "tally");
    s.add(f);
    std::vector<function_data_type::parameter> parameters;
    auto t = std::make_shared<generic_data_type>(location::global(), "`T", 0);
    parameters.emplace_back("source", t);
    auto d = std::make_shared<function_definition>(location::global(), f,
      std::make_shared<function_data_type>(parameters,
      integer_data_type::get_instance()));
    s.add(d);
  }

  //! Populates a scope with all builtin elements.
  /*!
    \param s The scope to populate.
  */
  inline void populate_builtins(scope& s) {
    populate_data_types(s);
    populate_arithmetic(s);
    populate_chain(s);
    populate_count(s);
    populate_first(s);
    populate_fold(s);
    populate_last(s);
    populate_print(s);
    populate_tally(s);
  }

  //! Constructs a scope with all builtins populated.
  inline std::unique_ptr<scope> make_builtin_scope() {
    auto s = std::make_unique<scope>();
    populate_builtins(*s);
    return s;
  }
}

#endif
