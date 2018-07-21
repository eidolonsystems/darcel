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
  inline void populate_data_types(Scope& scope) {
    scope.add(BoolDataType::get_instance());
    scope.add(FloatDataType::get_instance());
    scope.add(IntegerDataType::get_instance());
    scope.add(TextDataType::get_instance());
  }

  //! Populates a scope with builtin arithmetic operators.
  /*!
    \param s The scope to populate.
  */
  inline void populate_arithmetic(Scope& s) {
    for(auto& o : {op::ADD, op::SUBTRACT, op::MULTIPLY, op::DIVIDE}) {
      auto f = std::make_shared<Function>(Location::global(),
        get_function_name(o));
      s.add(f);
      s.add(std::make_shared<CallableDataType>(f));
      for(auto& t : std::vector<std::shared_ptr<DataType>>{
          IntegerDataType::get_instance(), FloatDataType::get_instance()}) {
        std::vector<FunctionDataType::Parameter> parameters;
        parameters.emplace_back("left", t);
        parameters.emplace_back("right", t);
        auto d = std::make_shared<FunctionDefinition>(Location::global(), f,
          std::make_shared<FunctionDataType>(parameters, t));
        s.add(d);
      }
    }
    {
      std::vector<FunctionDataType::Parameter> parameters;
      parameters.emplace_back("left", TextDataType::get_instance());
      parameters.emplace_back("right", TextDataType::get_instance());
      auto name = get_function_name(op::ADD);
      auto f = s.find<Function>(name);
      auto d = std::make_shared<FunctionDefinition>(Location::global(), f,
        std::make_shared<FunctionDataType>(parameters,
        TextDataType::get_instance()));
      s.add(d);
    }
  }

  //! Populates a scope with the print function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_chain(Scope& s) {
    auto f = std::make_shared<Function>(Location::global(), "chain");
    s.add(f);
    std::vector<FunctionDataType::Parameter> parameters;
    auto t = std::make_shared<GenericDataType>(Location::global(), "`T", 0);
    parameters.emplace_back("initial", t);
    parameters.emplace_back("continuation", t);
    auto d = std::make_shared<FunctionDefinition>(Location::global(), f,
      std::make_shared<FunctionDataType>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the count function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_count(Scope& s) {
    auto f = std::make_shared<Function>(Location::global(), "count");
    s.add(f);
    std::vector<FunctionDataType::Parameter> parameters;
    parameters.emplace_back("start", IntegerDataType::get_instance());
    parameters.emplace_back("end", IntegerDataType::get_instance());
    auto d = std::make_shared<FunctionDefinition>(Location::global(), f,
      std::make_shared<FunctionDataType>(parameters,
      IntegerDataType::get_instance()));
    s.add(d);
  }

  //! Populates a scope with the first function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_first(Scope& s) {
    auto f = std::make_shared<Function>(Location::global(), "first");
    s.add(f);
    std::vector<FunctionDataType::Parameter> parameters;
    auto t = std::make_shared<GenericDataType>(Location::global(), "`T", 0);
    parameters.emplace_back("source", t);
    auto d = std::make_shared<FunctionDefinition>(Location::global(), f,
      std::make_shared<FunctionDataType>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the fold function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_fold(Scope& s) {
    auto f = std::make_shared<Function>(Location::global(), "fold");
    s.add(f);
    auto t = std::make_shared<GenericDataType>(Location::global(), "`T", 0);
    std::vector<FunctionDataType::Parameter> parameters;
    auto r = [&] {
      std::vector<FunctionDataType::Parameter> parameters;
      parameters.emplace_back("left", t);
      parameters.emplace_back("right", t);
      return std::make_shared<FunctionDataType>(std::move(parameters), t);
    }();
    parameters.emplace_back("f", std::move(r));
    parameters.emplace_back("r", t);
    auto d = std::make_shared<FunctionDefinition>(Location::global(), f,
      std::make_shared<FunctionDataType>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the last function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_last(Scope& s) {
    auto f = std::make_shared<Function>(Location::global(), "last");
    s.add(f);
    std::vector<FunctionDataType::Parameter> parameters;
    auto t = std::make_shared<GenericDataType>(Location::global(), "`T", 0);
    parameters.emplace_back("source", t);
    auto d = std::make_shared<FunctionDefinition>(Location::global(), f,
      std::make_shared<FunctionDataType>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the print function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_print(Scope& s) {
    auto f = std::make_shared<Function>(Location::global(), "print");
    s.add(f);
    std::vector<FunctionDataType::Parameter> parameters;
    auto t = std::make_shared<GenericDataType>(Location::global(), "`T", 0);
    parameters.emplace_back("source", t);
    auto d = std::make_shared<FunctionDefinition>(Location::global(), f,
      std::make_shared<FunctionDataType>(parameters, t));
    s.add(d);
  }

  //! Populates a scope with the tally function.
  /*!
    \param s The scope to populate.
  */
  inline void populate_tally(Scope& s) {
    auto f = std::make_shared<Function>(Location::global(), "tally");
    s.add(f);
    std::vector<FunctionDataType::Parameter> parameters;
    auto t = std::make_shared<GenericDataType>(Location::global(), "`T", 0);
    parameters.emplace_back("source", t);
    auto d = std::make_shared<FunctionDefinition>(Location::global(), f,
      std::make_shared<FunctionDataType>(parameters,
      IntegerDataType::get_instance()));
    s.add(d);
  }

  //! Populates a scope with all builtin elements.
  /*!
    \param s The scope to populate.
  */
  inline void populate_builtins(Scope& s) {
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
  inline std::unique_ptr<Scope> make_builtin_scope() {
    auto s = std::make_unique<Scope>();
    populate_builtins(*s);
    return s;
  }
}

#endif
