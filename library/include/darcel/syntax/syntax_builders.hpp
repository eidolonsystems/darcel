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

  //! Converts an expression to return a value of a specified data type.
  /*!
    \param e The expression to convert.
    \param type The data type the expression should evaluate to.
    \return An expression that converts e to evaluate to type.
  */
  inline std::unique_ptr<expression> convert(std::unique_ptr<expression>&& e,
      const std::shared_ptr<data_type>& type) {
    auto compatibility = get_compatibility(*e->get_data_type(), *type);
    if(compatibility == data_type_compatibility::EQUAL) {
      return std::move(e);
    }
    if(auto f = dynamic_cast<function_expression*>(e.get())) {
      if(auto t = std::dynamic_pointer_cast<function_data_type>(type)) {
        auto overload = find_overload(*f->get_function(), *t);
        if(overload != nullptr) {
          return std::make_unique<variable_expression>(e->get_location(),
            overload);
        }
      }
    }
    return nullptr;
  }

  //! Binds a new enum.
  /*!
    \param l The location of the binding.
    \param name The name of the enum to bind.
    \param symbols The symbols belonging to the enum.
    \param s The scope that the binding belongs to.
  */
  inline std::unique_ptr<bind_enum_statement> bind_enum(location l,
      std::string name, std::vector<enum_data_type::symbol> symbols,
      scope& s) {
    auto type = std::make_shared<enum_data_type>(std::move(l),
      std::move(name), std::move(symbols));
    s.add(type);
    return std::make_unique<bind_enum_statement>(std::move(type));
  }

  //! Binds a new function.
  /*!
    \param l The location of the binding.
    \param name The name of the function to bind.
    \param parameters The list of function parameters.
    \param body The function called to build the body of the function.
    \param s The scope that the binding belongs to.
  */
  inline std::unique_ptr<bind_function_statement> bind_function(location l,
      std::string name, std::vector<function_data_type::parameter> parameters,
      const expression_builder& body, scope& s) {
    scope parameter_scope(&s);
    std::vector<std::shared_ptr<variable>> parameter_variables;
    for(auto& parameter : parameters) {
      if(auto generic = std::dynamic_pointer_cast<generic_data_type>(
          parameter.m_type)) {
        auto existing_element = parameter_scope.find<data_type>(
          generic->get_name());
        if(existing_element == nullptr) {
          parameter_scope.add(generic);
        }
      }
      auto v = std::make_shared<variable>(l, parameter.m_name,
        parameter.m_type);
      parameter_scope.add(v);
      parameter_variables.push_back(std::move(v));
    }
    scope body_scope(&parameter_scope);
    auto e = body(body_scope);
    auto type = std::make_shared<function_data_type>(std::move(parameters),
      e->get_data_type());
    auto existing_element = s.find_within(name);
    auto v = std::make_shared<variable>(l, name, std::move(type));
    auto f = [&] {
      if(existing_element == nullptr) {
        auto f = std::make_shared<function>(v);
        s.add(f);
        return f;
      }
      auto f = std::dynamic_pointer_cast<function>(existing_element);
      if(f == nullptr) {
        throw redefinition_syntax_error(l, name,
          existing_element->get_location());
      }
      if(!f->add(v)) {
        throw redefinition_syntax_error(l, name,
          existing_element->get_location());
      }
      return f;
    }();
    return std::make_unique<bind_function_statement>(std::move(l), std::move(f),
      std::move(v), std::move(parameter_variables), std::move(e));
  }

  //! Binds a new function.
  /*!
    \param name The name of the function to bind.
    \param parameters The list of function parameters.
    \param body The function called to build the body of the function.
  */
  inline std::unique_ptr<bind_function_statement> bind_function(
      std::string name, std::vector<function_data_type::parameter> parameters,
      const expression_builder& body, scope& s) {
    return bind_function(location::global(), std::move(name),
      std::move(parameters), body, s);
  }

  //! Binds a new variable to an expression.
  /*!
    \param l The location of the binding.
    \param name The name of the variable to bind.
    \param e The expression to bind to the variable.
  */
  inline std::unique_ptr<bind_variable_statement> bind_variable(location l,
      std::string name, std::unique_ptr<expression> e, scope& s) {
    auto existing_element = s.find_within(name);
    if(existing_element != nullptr) {
      throw redefinition_syntax_error(l, name,
        existing_element->get_location());
    }
    auto v = std::make_shared<variable>(l, std::move(name), e->get_data_type());
    s.add(v);
    return std::make_unique<bind_variable_statement>(std::move(l), std::move(v),
      std::move(e));
  }

  //! Binds a new variable to an expression.
  /*!
    \param name The name of the variable to bind.
    \param e The expression to bind to the variable.
  */
  inline std::unique_ptr<bind_variable_statement> bind_variable(
      std::string name, std::unique_ptr<expression> e, scope& s) {
    return bind_variable(location::global(), std::move(name), std::move(e), s);
  }

  //! Makes a call expression.
  /*!
    \param l The location of the expression.
    \param callable The expression to call.
    \param arguments The list of arguments to pass to the function.
  */
  inline std::unique_ptr<call_expression> call(location l,
      std::unique_ptr<expression> callable,
      std::vector<std::unique_ptr<expression>> arguments) {
    if(auto f = dynamic_cast<const function_expression*>(callable.get())) {
      std::vector<function_data_type::parameter> types;
      std::transform(arguments.begin(), arguments.end(),
        std::back_inserter(types),
        [] (auto& p) {
          return function_data_type::parameter("", p->get_data_type());
        });
      auto overload = find_overload(*f->get_function(), types);
      if(overload == nullptr) {
        throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND, l);
      }
      callable = std::make_unique<variable_expression>(f->get_location(),
        std::move(overload));
    }
    if(auto type = std::dynamic_pointer_cast<function_data_type>(
        callable->get_data_type())) {
      if(type->get_parameters().size() != arguments.size()) {
        throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND, l);
      }
      for(std::size_t i = 0; i < arguments.size(); ++i) {
        auto conversion = convert(std::move(arguments[i]),
          type->get_parameters()[i].m_type);
        if(conversion == nullptr) {
          throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND, l);
        }
        arguments[i] = std::move(conversion);
      }
      return std::make_unique<call_expression>(l, std::move(callable),
        std::move(arguments));
    }
    throw syntax_error(syntax_error_code::EXPRESSION_NOT_CALLABLE, l);
  }

  //! Makes a call expression.
  /*!
    \param l The location of the expression.
    \param name The name of the function to call.
    \param arguments The list of arguments to pass to the function.
    \param s The scope to find the function in.
  */
  inline std::unique_ptr<call_expression> call(location l, std::string name,
      std::vector<std::unique_ptr<expression>> arguments, scope& s) {
    auto callable = find_term(l, name, s);
    return call(std::move(l), std::move(callable), std::move(arguments));
  }

  //! Makes a call expression.
  /*!
    \param name The name of the function to call.
    \param arguments The list of arguments to pass to the function.
    \param s The scope to find the function in.
  */
  inline std::unique_ptr<call_expression> call(std::string name,
      std::vector<std::unique_ptr<expression>> arguments, scope& s) {
    return call(location::global(), std::move(name), std::move(arguments), s);
  }

  //! Makes a call expression.
  /*!
    \param name The name of the function to call.
    \param s The scope to find the function in.
  */
  inline std::unique_ptr<call_expression> call(std::string name, scope& s) {
    return call(location::global(), std::move(name), {}, s);
  }

  //! Makes a call expression.
  /*!
    \param name The name of the function to call.
    \param arg1 The argument to pass to the function.
    \param s The scope to find the function in.
  */
  inline std::unique_ptr<call_expression> call(std::string name,
      std::unique_ptr<expression> arg1, scope& s) {
    std::vector<std::unique_ptr<expression>> arguments;
    arguments.push_back(std::move(arg1));
    return call(location::global(), std::move(name), std::move(arguments), s);
  }

  //! Makes a call expression.
  /*!
    \param name The name of the function to call.
    \param arg1 The first argument to pass to the function.
    \param arg2 The second argument to pass to the function.
    \param s The scope to find the function in.
  */
  inline std::unique_ptr<call_expression> call(std::string name,
      std::unique_ptr<expression> arg1, std::unique_ptr<expression> arg2,
      scope& s) {
    std::vector<std::unique_ptr<expression>> arguments;
    arguments.push_back(std::move(arg1));
    arguments.push_back(std::move(arg2));
    return call(location::global(), std::move(name), std::move(arguments), s);
  }

  //! Makes a literal boolean expression.
  /*!
    \param l The location of the expression.
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal_expression(
      location l, bool value) {
    std::string s = [&] {
      if(value) {
        return "true";
      }
      return "false";
    }();
    return std::make_unique<literal_expression>(std::move(l),
      literal(s, bool_data_type::get_instance()));
  }

  //! Makes a literal integer expression.
  /*!
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal_expression(
      bool value) {
    return make_literal_expression(location::global(), value);
  }

  //! Makes a literal integer expression.
  /*!
    \param l The location of the expression.
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal_expression(
      location l, int value) {
    return std::make_unique<literal_expression>(std::move(l),
      literal(std::to_string(value), integer_data_type::get_instance()));
  }

  //! Makes a literal integer expression.
  /*!
    \param value The value to represent.
  */
  inline std::unique_ptr<literal_expression> make_literal_expression(
      int value) {
    return make_literal_expression(location::global(), value);
  }

  //! Makes a variable expression.
  /*!
    \param l The location of the expression.
    \param name The name of the variable.
    \param s The scope the variable can be found in.
  */
  inline std::unique_ptr<variable_expression> make_variable_expression(
      location l, const std::string& name, const scope& s) {
    auto v = s.find<variable>(name);
    if(v == nullptr) {
      throw variable_not_found_error(l, name);
    }
    return std::make_unique<variable_expression>(l, std::move(v));
  }

  //! Makes a variable expression.
  /*!
    \param name The name of the variable.
    \param s The scope the variable can be found in.
  */
  inline std::unique_ptr<variable_expression> make_variable_expression(
      const std::string& name, const scope& s) {
    return make_variable_expression(location::global(), name, s);
  }

  //! Returns an expression that is either a variable term or a function term.
  /*!
    \param l The location of the term.
    \param name The name of the term.
    \param s The scope to find the term in.
  */
  inline std::unique_ptr<expression> find_term(location l,
      const std::string& name, const scope& s) {
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
