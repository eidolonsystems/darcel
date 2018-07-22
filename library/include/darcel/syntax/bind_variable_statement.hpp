#ifndef DARCEL_BIND_VARIABLE_STATEMENT_HPP
#define DARCEL_BIND_VARIABLE_STATEMENT_HPP
#include "darcel/semantics/variable.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Binds a new variable to an expression.
  class BindVariableStatement final : public Statement {
    public:

      //! Constructs a bind statement.
      /*!
        \param l The location of the statement.
        \param v The variable to bind.
        \param e The expression to bind to the variable.
      */
      BindVariableStatement(Location l, std::shared_ptr<Variable> v,
        std::unique_ptr<Expression> e);

      //! Returns the variable.
      const std::shared_ptr<Variable>& get_variable() const;

      //! Returns the expression that was bound.
      const Expression& get_expression() const;

      void apply(SyntaxNodeVisitor& visitor) const override;

    private:
      std::shared_ptr<Variable> m_variable;
      std::unique_ptr<Expression> m_expression;
  };

  inline BindVariableStatement::BindVariableStatement(Location l,
      std::shared_ptr<Variable> v, std::unique_ptr<Expression> e)
      : Statement(std::move(l)),
        m_variable(std::move(v)),
        m_expression(std::move(e)) {}

  inline const std::shared_ptr<Variable>&
      BindVariableStatement::get_variable() const {
    return m_variable;
  }

  inline const Expression& BindVariableStatement::get_expression() const {
    return *m_expression;
  }

  inline void BindVariableStatement::apply(
      SyntaxNodeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline void SyntaxNodeVisitor::visit(
      const BindVariableStatement& node) {
    visit(static_cast<const Statement&>(node));
  }
}

#endif
