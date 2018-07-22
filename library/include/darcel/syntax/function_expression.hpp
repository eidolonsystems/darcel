#ifndef DARCEL_FUNCTION_EXPRESSION_HPP
#define DARCEL_FUNCTION_EXPRESSION_HPP
#include <memory>
#include <utility>
#include "darcel/semantics/function.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! An expression that evaluates to an overloaded function.
  class FunctionExpression final : public Expression {
    public:

      //! Constructs a function expression.
      /*!
        \param l The location of the identifier token.
        \param f The function to evaluate.
      */
      FunctionExpression(Location l, std::shared_ptr<Function> f);

      //! Returns the function to evaluate.
      const std::shared_ptr<Function>& get_function() const;

      void apply(SyntaxNodeVisitor& visitor) const override;

    private:
      std::shared_ptr<Function> m_function;
  };

  inline FunctionExpression::FunctionExpression(Location l,
      std::shared_ptr<Function> f)
      : Expression(std::move(l)),
        m_function(std::move(f)) {}

  inline const std::shared_ptr<Function>& FunctionExpression::
      get_function() const {
    return m_function;
  }

  inline void FunctionExpression::apply(SyntaxNodeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline void SyntaxNodeVisitor::visit(const FunctionExpression& node) {
    visit(static_cast<const Expression&>(node));
  }
}

#endif
