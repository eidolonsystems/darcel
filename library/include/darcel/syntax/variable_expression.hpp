#ifndef DARCEL_VARIABLE_EXPRESSION_HPP
#define DARCEL_VARIABLE_EXPRESSION_HPP
#include <memory>
#include <utility>
#include "darcel/semantics/variable.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! An expression that evaluates to a variable.
  class VariableExpression final : public Expression {
    public:

      //! Constructs a variable expression.
      /*!
        \param l The location of the identifier token.
        \param v The variable to evaluate.
      */
      VariableExpression(Location l, std::shared_ptr<Variable> v);

      //! Returns the variable to evaluate.
      const std::shared_ptr<Variable>& get_variable() const;

      void apply(SyntaxNodeVisitor& visitor) const override;

    private:
      std::shared_ptr<Variable> m_variable;
  };

  inline VariableExpression::VariableExpression(Location l,
      std::shared_ptr<Variable> v)
      : Expression(std::move(l)),
        m_variable(std::move(v)) {}

  inline const std::shared_ptr<Variable>& VariableExpression::
      get_variable() const {
    return m_variable;
  }

  inline void VariableExpression::apply(SyntaxNodeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline void SyntaxNodeVisitor::visit(const VariableExpression& node) {
    visit(static_cast<const Expression&>(node));
  }
}

#endif
