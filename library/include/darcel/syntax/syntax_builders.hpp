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
#include "darcel/syntax/bind_function_statement.hpp"
#include "darcel/syntax/call_expression.hpp"
#include "darcel/syntax/literal_expression.hpp"
#include "darcel/syntax/redefinition_syntax_error.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/variable_not_found_error.hpp"
#include "darcel/syntax/variable_expression.hpp"


namespace darcel {
  std::unique_ptr<expression> find_term(location l, const std::string& name,
    const scope& s);

  //! Function used to build a sub-expression.
  /*!
    \param s The scope of the sub-expression.
  */
  using expression_builder = std::function<
    std::unique_ptr<expression> (scope& s)>;

  //! Binds a new enum.
  /*!
    \param l The location of the binding.
    \param s The scope that the binding belongs to.
    \param name The name of the enum to bind.
    \param symbols The symbols belonging to the enum.
  */
  inline std::unique_ptr<bind_enum_statement> bind_enum(location l, scope& s,
      std::string name, std::vector<enum_data_type::symbol> symbols) {
    auto type = std::make_shared<enum_data_type>(std::move(l), std::move(name),
      std::move(symbols));
    s.add(type);
    return std::make_unique<bind_enum_statement>(s, std::move(type));
  }

  //! Binds a new function.
  /*!
    \param l The location of the binding.
    \param s The scope that the binding belongs to.
    \param name The name of the function to bind.
    \param parameters The list of function parameters.
    \param body The function called to build the body of the function.
  */
  inline std::unique_ptr<bind_function_statement> bind_function(location l,
      scope& s, std::string name,
      std::vector<bind_function_statement::parameter> parameters,
      const expression_builder& body) {
    auto f = [&] {
      auto existing_element = s.find_within(name);
      if(existing_element == nullptr) {
        auto f = std::make_shared<function>(l, name);
        s.add(f);
        return f;
      }
      auto f = std::dynamic_pointer_cast<function>(existing_element);
      if(f == nullptr) {
        throw redefinition_syntax_error(l, name,
          existing_element->get_location());
      }
      return f;
    }();
    auto& body_scope = s.build_child();
    for(auto& parameter : parameters) {
      if(parameter.m_type.has_value()) {
        if(auto generic = std::dynamic_pointer_cast<generic_data_type>(
            *parameter.m_type)) {
          auto existing_element = body_scope.find<data_type>(
            generic->get_name());
          if(existing_element == nullptr) {
            body_scope.add(generic);
          }
        }
      }
      body_scope.add(parameter.m_variable);
    }
    auto& expression_scope = body_scope.build_child();
    auto e = body(expression_scope);
    return std::make_unique<bind_function_statement>(std::move(l), s,
      std::move(f), std::move(parameters), std::move(e));
  }

  //! Binds a new function.
  /*!
    \param s The scope the statement belongs to.
    \param name The name of the function to bind.
    \param parameters The list of function parameters.
    \param body The function called to build the body of the function.
  */
  inline std::unique_ptr<bind_function_statement> bind_function(scope& s,
      std::string name,
      std::vector<bind_function_statement::parameter> parameters,
      const expression_builder& body) {
    return bind_function(location::global(), s, std::move(name),
      std::move(parameters), body);
  }

  //! Binds a new variable to an expression.
  /*!
    \param l The location of the binding.
    \param s The scope the statement belongs to.
    \param name The name of the variable to bind.
    \param e The expression to bind to the variable.
  */
  inline std::unique_ptr<bind_variable_statement> bind_variable(location l,
      scope& s, std::string name, std::unique_ptr<expression> e) {
    auto existing_element = s.find_within(name);
    if(existing_element != nullptr) {
      throw redefinition_syntax_error(l, name,
        existing_element->get_location());
    }
    auto v = std::make_shared<variable>(l, std::move(name));
    s.add(v);
    return std::make_unique<bind_variable_statement>(std::move(l), s,
      std::move(v), std::move(e));
  }

  //! Binds a new variable to an expression.
  /*!
    \param s The scope the statement belongs to.
    \param name The name of the variable to bind.
    \param e The expression to bind to the variable.
  */
  inline std::unique_ptr<bind_variable_statement> bind_variable(scope& s,
      std::string name, std::unique_ptr<expression> e) {
    return bind_variable(location::global(), s, std::move(name), std::move(e));
  }

  //! Makes a call expression.
  /*!
    \param l The location of the expression.
    \param s The scope containing the expression.
    \param callable The expression to call.
    \param arguments The list of arguments to pass to the function.
  */
  inline std::unique_ptr<call_expression> call(location l, const scope& s,
      std::unique_ptr<expression> callable,
      std::vector<std::unique_ptr<expression>> arguments) {
    return std::make_unique<call_expression>(l, s, std::move(callable),
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
  inline std::unique_ptr<call_expression> call(location l, const scope& s,
      std::string name, std::vector<std::unique_ptr<expression>> arguments) {
    auto callable = find_term(l, name, s);
    return call(std::move(l), s, std::move(callable), std::move(arguments));
  }

  //! Makes a call expression.
  /*!
    \param s The scope to find the function in.
    \param name The name of the function to call.
    \param arguments The list of arguments to pass to the function.
  */
  inline std::unique_ptr<call_expression> call(const scope& s, std::string name,
      std::vector<std::unique_ptr<expression>> arguments) {
    return call(location::global(), s, std::move(name), std::move(arguments));
  }

  //! Makes a call expression.
  /*!
    \param name The name of the function to call.
    \param s The scope to find the function in.
  */
  inline std::unique_ptr<call_expression> call(const scope& s,
      std::string name) {
    return call(location::global(), s, std::move(name), {});
  }

  //! Makes a call expression.
  /*!
    \param s The scope to find the function in.
    \param name The name of the function to call.
    \param arg1 The argument to pass to the function.
  */
  inline std::unique_ptr<call_expression> call(const scope& s, std::string name,
      std::unique_ptr<expression> arg1) {
    std::vector<std::unique_ptr<expression>> arguments;
    arguments.push_back(std::move(arg1));
    return call(location::global(), s, std::move(name), std::move(arguments));
  }

  //! Makes a call expression.
  /*!
    \param s The scope to find the function in.
    \param name The name of the function to call.
    \param arg1 The first argument to pass to the function.
    \param arg2 The second argument to pass to the function.
  */
  inline std::unique_ptr<call_expression> call(const scope& s, std::string name,
      std::unique_ptr<expression> arg1, std::unique_ptr<expression> arg2) {
    std::vector<std::unique_ptr<expression>> arguments;
    arguments.push_back(std::move(arg1));
    arguments.push_back(std::move(arg2));
    return call(location::global(), s, std::move(name), std::move(arguments));
  }

  //! Makes a literal boolean expression.
  /*!
    \param l The location of the expression.
    \param s The scope containing the expression.
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal_expression(
      const scope& s, location l, bool value) {
    std::string v = [&] {
      if(value) {
        return "true";
      }
      return "false";
    }();
    return std::make_unique<literal_expression>(std::move(l), s,
      literal(v, bool_data_type::get_instance()));
  }

  //! Makes a literal integer expression.
  /*!
    \param s The scope containing the expression.
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal_expression(
      const scope& s, bool value) {
    return make_literal_expression(location::global(), s, value);
  }

  //! Makes a literal integer expression.
  /*!
    \param l The location of the expression.
    \param s The scope containing the expression.
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal_expression(
      location l, const scope& s, int value) {
    return std::make_unique<literal_expression>(std::move(l), s
      literal(std::to_string(value), integer_data_type::get_instance()));
  }

  //! Makes a literal integer expression.
  /*!
    \param s The scope containing the expression.
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal_expression(
      const scope& s, int value) {
    return make_literal_expression(location::global(), s, value);
  }

  //! Makes a variable expression.
  /*!
    \param l The location of the expression.
    \param s The scope containing the expression.
    \param name The name of the variable.
  */
  inline std::unique_ptr<variable_expression> make_variable_expression(
      location l, const scope& s, const std::string& name) {
    auto v = s.find<variable>(name);
    if(v == nullptr) {
      throw variable_not_found_error(l, name);
    }
    return std::make_unique<variable_expression>(l, s, std::move(v));
  }

  //! Makes a variable expression.
  /*!
    \param s The scope the variable can be found in.
    \param name The name of the variable.
  */
  inline std::unique_ptr<variable_expression> make_variable_expression(
      const scope& s, const std::string& name) {
    return make_variable_expression(location::global(), s, name);
  }

  //! Returns an expression that is either a variable term or a function term.
  /*!
    \param l The location of the term.
    \param s The scope to find the term in.
    \param name The name of the term.
  */
  inline std::unique_ptr<expression> find_term(location l, const scope& s,
      const std::string& name) {
    auto e = s.find(name);
    if(auto f = std::dynamic_pointer_cast<function>(e)) {
      return std::make_unique<function_expression>(std::move(l), std::move(f));
    } else if(auto v = std::dynamic_pointer_cast<variable>(e)) {
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
      const scope& s) {
    return find_term(location::global(), name, s);
  }
}

#endif
