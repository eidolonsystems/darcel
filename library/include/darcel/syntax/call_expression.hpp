#ifndef DARCEL_CALL_EXPRESSION_HPP
#define DARCEL_CALL_EXPRESSION_HPP
#include <memory>
#include <utility>
#include <vector>
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Represents applying a callable expression to its parameters,
  //! ie. a function call.
  class CallExpression final : public Expression {
    public:

      //! Constructs a CallExpression.
      /*!
        \param l The location of the opening bracket.
        \param callable The expression to call.
        \param arguments The arguments to apply to the <i>callable</i>.
      */
      CallExpression(Location l, std::unique_ptr<Expression> callable,
        std::vector<std::unique_ptr<Expression>> arguments);

      //! Returns the callable expression.
      const Expression& get_callable() const;

      //! Returns the arguments to apply to the callable.
      const std::vector<std::unique_ptr<Expression>>& get_arguments() const;

      void apply(SyntaxNodeVisitor& visitor) const override;

    private:
      std::unique_ptr<Expression> m_callable;
      std::vector<std::unique_ptr<Expression>> m_arguments;
  };

  inline CallExpression::CallExpression(Location l,
      std::unique_ptr<Expression> callable,
      std::vector<std::unique_ptr<Expression>> arguments)
      : Expression(std::move(l)),
        m_callable(std::move(callable)),
        m_arguments(std::move(arguments)) {}

  inline const Expression& CallExpression::get_callable() const {
    return *m_callable;
  }

  inline const std::vector<std::unique_ptr<Expression>>&
      CallExpression::get_arguments() const {
    return m_arguments;
  }

  inline void CallExpression::apply(SyntaxNodeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline void SyntaxNodeVisitor::visit(const CallExpression& node) {
    visit(static_cast<const Expression&>(node));
  }
}

#endif
