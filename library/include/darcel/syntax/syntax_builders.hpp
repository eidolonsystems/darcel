#ifndef DARCEL_SYNTAX_BUILDERS_HPP
#define DARCEL_SYNTAX_BUILDERS_HPP
#include <array>
#include <functional>
#include <memory>
#include <utility>
#include "darcel/data_types/data_type_compatibility.hpp"
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/syntax/bind_enum_statement.hpp"
#include "darcel/syntax/bind_function_statement.hpp"
#include "darcel/syntax/bind_variable_statement.hpp"
#include "darcel/syntax/call_expression.hpp"
#include "darcel/syntax/function_expression.hpp"
#include "darcel/syntax/literal_expression.hpp"
#include "darcel/syntax/redefinition_syntax_error.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/variable_not_found_error.hpp"
#include "darcel/syntax/variable_expression.hpp"


namespace darcel {
  std::unique_ptr<expression> find_term(Location l, const Scope& s,
    const std::string& name);

  //! Function used to build a sub-expression.
  /*!
    \param s The scope of the sub-expression.
  */
  using expression_builder = std::function<
    std::unique_ptr<expression> (Scope& s)>;

  //! Binds a new enum.
  /*!
    \param l The location of the binding.
    \param s The scope that the binding belongs to.
    \param name The name of the enum to bind.
    \param symbols The symbols belonging to the enum.
  */
  inline std::unique_ptr<bind_enum_statement> bind_enum(Location l, Scope& s,
      std::string name, std::vector<EnumDataType::Symbol> symbols) {
    auto type = std::make_shared<EnumDataType>(std::move(l), std::move(name),
      std::move(symbols));
    s.add(type);
    return std::make_unique<bind_enum_statement>(std::move(type));
  }

  //! Binds a new function.
  /*!
    \param l The location of the binding.
    \param s The scope that the binding belongs to.
    \param name The name of the function to bind.
    \param parameters The list of function parameters.
    \param body The function called to build the body of the function.
  */
  inline std::unique_ptr<bind_function_statement> bind_function(Location l,
      Scope& s, std::string name,
      std::vector<bind_function_statement::parameter> parameters,
      const expression_builder& body) {
    auto f = [&] {
      auto existing_element = s.find_within(name);
      if(existing_element == nullptr) {
        auto parent = s.find<Function>(name);
        auto f = [&] {
          if(parent == nullptr) {
            return std::make_shared<Function>(l, name);
          } else {
            return std::make_shared<Function>(l, std::move(parent));
          }
        }();
        s.add(f);
        return f;
      }
      auto f = std::dynamic_pointer_cast<Function>(existing_element);
      if(f == nullptr) {
        throw redefinition_syntax_error(l, name,
          existing_element->get_location());
      }
      return f;
    }();
    Scope body_scope(&s);
    for(auto& parameter : parameters) {
      if(auto generic = std::dynamic_pointer_cast<GenericDataType>(
          parameter.m_type)) {
        auto existing_element = body_scope.find<DataType>(generic->get_name());
        if(existing_element == nullptr) {
          body_scope.add(generic);
        }
      }
      body_scope.add(parameter.m_variable);
    }
    Scope expression_scope(&body_scope);
    auto e = body(expression_scope);
    return std::make_unique<bind_function_statement>(std::move(l), std::move(f),
      std::move(parameters), std::move(e));
  }

  //! Binds a new function.
  /*!
    \param s The scope the statement belongs to.
    \param name The name of the function to bind.
    \param parameters The list of function parameters.
    \param body The function called to build the body of the function.
  */
  inline std::unique_ptr<bind_function_statement> bind_function(Scope& s,
      std::string name,
      std::vector<bind_function_statement::parameter> parameters,
      const expression_builder& body) {
    return bind_function(Location::global(), s, std::move(name),
      std::move(parameters), body);
  }

  //! Binds a new function.
  /*!
    \param s The scope the statement belongs to.
    \param name The name of the function to bind.
    \param parameters The list of function parameters.
    \param body The function called to build the body of the function.
  */
  inline std::unique_ptr<bind_function_statement> bind_function(Scope& s,
      std::string name, std::vector<FunctionDataType::Parameter> parameters,
      const expression_builder& body) {
    std::vector<bind_function_statement::parameter> p;
    for(auto& parameter : parameters) {
      p.emplace_back(std::make_shared<Variable>(Location::global(),
        parameter.m_name), parameter.m_type);
    }
    return bind_function(Location::global(), s, std::move(name), std::move(p),
      body);
  }

  //! Binds a new function.
  /*!
    \param s The scope the statement belongs to.
    \param name The name of the function to bind.
    \param body The function called to build the body of the function.
  */
  inline std::unique_ptr<bind_function_statement> bind_function(Scope& s,
      std::string name, const expression_builder& body) {
    std::vector<bind_function_statement::parameter> p;
    return bind_function(Location::global(), s, std::move(name), std::move(p),
      body);
  }

  //! Binds a new variable to an expression.
  /*!
    \param l The location of the binding.
    \param s The scope the statement belongs to.
    \param name The name of the variable to bind.
    \param e The expression to bind to the variable.
  */
  inline std::unique_ptr<bind_variable_statement> bind_variable(Location l,
      Scope& s, std::string name, std::unique_ptr<expression> e) {
    auto existing_element = s.find_within(name);
    if(existing_element != nullptr) {
      throw redefinition_syntax_error(l, name,
        existing_element->get_location());
    }
    auto v = std::make_shared<Variable>(l, std::move(name));
    s.add(v);
    return std::make_unique<bind_variable_statement>(std::move(l), std::move(v),
      std::move(e));
  }

  //! Binds a new variable to an expression.
  /*!
    \param s The scope the statement belongs to.
    \param name The name of the variable to bind.
    \param e The expression to bind to the variable.
  */
  inline std::unique_ptr<bind_variable_statement> bind_variable(Scope& s,
      std::string name, std::unique_ptr<expression> e) {
    return bind_variable(Location::global(), s, std::move(name), std::move(e));
  }

  //! Makes a call expression.
  /*!
    \param l The location of the expression.
    \param callable The expression to call.
    \param arguments The list of arguments to pass to the function.
  */
  inline std::unique_ptr<call_expression> call(Location l,
      std::unique_ptr<expression> callable,
      std::vector<std::unique_ptr<expression>> arguments) {
    return std::make_unique<call_expression>(l, std::move(callable),
      std::move(arguments));
  }

  //! Makes a call expression.
  /*!
    \param l The location of the expression.
    \param s The scope containing the expression.
    \param name The name of the function to call.
    \param arguments The list of arguments to pass to the function.
    \param s The scope to find the function in.
  */
  inline std::unique_ptr<call_expression> call(Location l, const Scope& s,
      std::string name, std::vector<std::unique_ptr<expression>> arguments) {
    auto callable = find_term(l, s, name);
    return call(std::move(l), std::move(callable), std::move(arguments));
  }

  //! Makes a call expression.
  /*!
    \param s The scope to find the function in.
    \param name The name of the function to call.
    \param arguments The list of arguments to pass to the function.
  */
  inline std::unique_ptr<call_expression> call(const Scope& s, std::string name,
      std::vector<std::unique_ptr<expression>> arguments) {
    return call(Location::global(), s, std::move(name), std::move(arguments));
  }

  //! Makes a call expression.
  /*!
    \param name The name of the function to call.
    \param s The scope to find the function in.
  */
  inline std::unique_ptr<call_expression> call(const Scope& s,
      std::string name) {
    return call(Location::global(), s, std::move(name), {});
  }

  //! Makes a call expression.
  /*!
    \param s The scope to find the function in.
    \param name The name of the function to call.
    \param arg1 The argument to pass to the function.
  */
  inline std::unique_ptr<call_expression> call(const Scope& s, std::string name,
      std::unique_ptr<expression> arg1) {
    std::vector<std::unique_ptr<expression>> arguments;
    arguments.push_back(std::move(arg1));
    return call(Location::global(), s, std::move(name), std::move(arguments));
  }

  //! Makes a call expression.
  /*!
    \param s The scope to find the function in.
    \param name The name of the function to call.
    \param arg1 The first argument to pass to the function.
    \param arg2 The second argument to pass to the function.
  */
  inline std::unique_ptr<call_expression> call(const Scope& s, std::string name,
      std::unique_ptr<expression> arg1, std::unique_ptr<expression> arg2) {
    std::vector<std::unique_ptr<expression>> arguments;
    arguments.push_back(std::move(arg1));
    arguments.push_back(std::move(arg2));
    return call(Location::global(), s, std::move(name), std::move(arguments));
  }

  //! Makes a literal boolean expression.
  /*!
    \param l The location of the expression.
    \param s The scope containing the expression.
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal(Location l,
      bool value) {
    std::string v = [&] {
      if(value) {
        return "true";
      }
      return "false";
    }();
    return std::make_unique<literal_expression>(std::move(l),
      Literal(v, BoolDataType::get_instance()));
  }

  //! Makes a literal integer expression.
  /*!
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal(bool value) {
    return make_literal(Location::global(), value);
  }

  //! Makes a literal integer expression.
  /*!
    \param l The location of the expression.
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal(Location l,
      int value) {
    return std::make_unique<literal_expression>(std::move(l),
      Literal(std::to_string(value), IntegerDataType::get_instance()));
  }

  //! Makes a literal integer expression.
  /*!
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal(int value) {
    return make_literal(Location::global(), value);
  }

  //! Makes a literal text expression.
  /*!
    \param l The location of the expression.
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_text(Location l,
      std::string value) {
    return std::make_unique<literal_expression>(std::move(l),
      Literal(std::move(value), TextDataType::get_instance()));
  }

  //! Makes a literal text expression.
  /*!
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_text(std::string value) {
    return make_text(Location::global(), std::move(value));
  }

  //! Makes a variable expression.
  /*!
    \param l The location of the expression.
    \param s The scope containing the expression.
    \param name The name of the variable.
  */
  inline std::unique_ptr<variable_expression> make_variable_expression(
      Location l, const Scope& s, const std::string& name) {
    auto v = s.find<Variable>(name);
    if(v == nullptr) {
      throw variable_not_found_error(l, name);
    }
    return std::make_unique<variable_expression>(l, std::move(v));
  }

  //! Makes a variable expression.
  /*!
    \param s The scope the variable can be found in.
    \param name The name of the variable.
  */
  inline std::unique_ptr<variable_expression> make_variable_expression(
      const Scope& s, const std::string& name) {
    return make_variable_expression(Location::global(), s, name);
  }

  //! Returns an expression that is either a variable term or a function term.
  /*!
    \param l The location of the term.
    \param s The scope to find the term in.
    \param name The name of the term.
  */
  inline std::unique_ptr<expression> find_term(Location l, const Scope& s,
      const std::string& name) {
    auto e = s.find(name);
    if(auto f = std::dynamic_pointer_cast<Function>(e)) {
      return std::make_unique<function_expression>(std::move(l), std::move(f));
    } else if(auto v = std::dynamic_pointer_cast<Variable>(e)) {
      return std::make_unique<variable_expression>(std::move(l), std::move(v));
    }
    throw variable_not_found_error(l, name);
  }

  //! Returns an expression that is either a variable term or a function term.
  /*!
    \param name The name of the term.
    \param s The scope to find the term in.
  */
  inline std::unique_ptr<expression> find_term(const std::string& name,
      const Scope& s) {
    return find_term(Location::global(), s, name);
  }
}

#endif
