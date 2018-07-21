#ifndef DARCEL_BIND_FUNCTION_STATEMENT_HPP
#define DARCEL_BIND_FUNCTION_STATEMENT_HPP
#include <memory>
#include "darcel/data_types/data_type.hpp"
#include "darcel/semantics/function.hpp"
#include "darcel/semantics/variable.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Binds a new function to an expression.
  class bind_function_statement final : public statement {
    public:

      //! Represents a function parameter.
      struct parameter {

        //! The variable representing the parameter.
        std::shared_ptr<variable> m_variable;

        //! The parameter's data type, if specified.
        std::shared_ptr<DataType> m_type;

        //! Constructs a parameter.
        /*!
          \param v The variable representing the parameter.
        */
        parameter(std::shared_ptr<variable> v);

        //! Constructs a parameter.
        /*!
          \param v The variable representing the parameter.
          \param t The parameter's type.
        */
        parameter(std::shared_ptr<variable> v, std::shared_ptr<DataType> t);
      };

      //! Constructs a bind statement.
      /*!
        \param l The location of the statement.
        \param f The function that the binding belongs to.
        \param p The function parameters.
        \param e The expression to bind to the function.
      */
      bind_function_statement(location l, std::shared_ptr<function> f,
        std::vector<parameter> p, std::unique_ptr<expression> e);

      //! Returns the function (potentially overloaded) that the binding belongs
      //! to.
      const std::shared_ptr<function>& get_function() const;

      //! Returns the function parameters.
      const std::vector<parameter>& get_parameters() const;

      //! Returns the expression that was bound.
      const expression& get_expression() const;

      void apply(syntax_node_visitor& visitor) const override;

    private:
      std::shared_ptr<function> m_function;
      std::vector<parameter> m_parameters;
      std::unique_ptr<expression> m_expression;
  };

  inline bind_function_statement::parameter::parameter(
      std::shared_ptr<variable> v)
      : m_variable(std::move(v)) {}

  inline bind_function_statement::parameter::parameter(
      std::shared_ptr<variable> v, std::shared_ptr<DataType> t)
      : m_variable(std::move(v)),
        m_type(std::move(t)) {}

  inline bind_function_statement::bind_function_statement(location l,
      std::shared_ptr<function> f, std::vector<parameter> p,
      std::unique_ptr<expression> e)
      : statement(std::move(l)),
        m_function(std::move(f)),
        m_parameters(std::move(p)),
        m_expression(std::move(e)) {}

  inline const std::shared_ptr<function>&
      bind_function_statement::get_function() const {
    return m_function;
  }

  inline const std::vector<bind_function_statement::parameter>&
      bind_function_statement::get_parameters() const {
    return m_parameters;
  }

  inline const expression& bind_function_statement::get_expression() const {
    return *m_expression;
  }

  inline void bind_function_statement::apply(
      syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(
      const bind_function_statement& node) {
    visit(static_cast<const statement&>(node));
  }
}

#endif
