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
  class BindFunctionStatement final : public Statement {
    public:

      //! Represents a function parameter.
      struct parameter {

        //! The variable representing the parameter.
        std::shared_ptr<Variable> m_variable;

        //! The parameter's data type, if specified.
        std::shared_ptr<DataType> m_type;

        //! Constructs a parameter.
        /*!
          \param v The variable representing the parameter.
        */
        parameter(std::shared_ptr<Variable> v);

        //! Constructs a parameter.
        /*!
          \param v The variable representing the parameter.
          \param t The parameter's type.
        */
        parameter(std::shared_ptr<Variable> v, std::shared_ptr<DataType> t);
      };

      //! Constructs a bind statement.
      /*!
        \param l The location of the statement.
        \param f The function that the binding belongs to.
        \param p The function parameters.
        \param e The expression to bind to the function.
      */
      BindFunctionStatement(Location l, std::shared_ptr<Function> f,
        std::vector<parameter> p, std::unique_ptr<Expression> e);

      //! Returns the function (potentially overloaded) that the binding belongs
      //! to.
      const std::shared_ptr<Function>& get_function() const;

      //! Returns the function parameters.
      const std::vector<parameter>& get_parameters() const;

      //! Returns the expression that was bound.
      const Expression& get_expression() const;

      void apply(SyntaxNodeVisitor& visitor) const override;

    private:
      std::shared_ptr<Function> m_function;
      std::vector<parameter> m_parameters;
      std::unique_ptr<Expression> m_expression;
  };

  inline BindFunctionStatement::parameter::parameter(
      std::shared_ptr<Variable> v)
      : m_variable(std::move(v)) {}

  inline BindFunctionStatement::parameter::parameter(
      std::shared_ptr<Variable> v, std::shared_ptr<DataType> t)
      : m_variable(std::move(v)),
        m_type(std::move(t)) {}

  inline BindFunctionStatement::BindFunctionStatement(Location l,
      std::shared_ptr<Function> f, std::vector<parameter> p,
      std::unique_ptr<Expression> e)
      : Statement(std::move(l)),
        m_function(std::move(f)),
        m_parameters(std::move(p)),
        m_expression(std::move(e)) {}

  inline const std::shared_ptr<Function>&
      BindFunctionStatement::get_function() const {
    return m_function;
  }

  inline const std::vector<BindFunctionStatement::parameter>&
      BindFunctionStatement::get_parameters() const {
    return m_parameters;
  }

  inline const Expression& BindFunctionStatement::get_expression() const {
    return *m_expression;
  }

  inline void BindFunctionStatement::apply(
      SyntaxNodeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline void SyntaxNodeVisitor::visit(
      const BindFunctionStatement& node) {
    visit(static_cast<const Statement&>(node));
  }
}

#endif
