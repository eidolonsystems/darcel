#ifndef DARCEL_LITERAL_EXPRESSION_HPP
#define DARCEL_LITERAL_EXPRESSION_HPP
#include <utility>
#include "darcel/lexicon/literal.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Represents an expression evaluating to a literal value.
  class LiteralExpression final : public Expression {
    public:

      //! Constructs a LiteralExpression.
      /*!
        \param l The location of the token representing the literal value.
        \param literal The literal to evaluate to.
      */
      LiteralExpression(Location l, Literal literal);

      //! Returns the literal that is evaluated.
      const Literal& get_literal() const;

      void apply(SyntaxNodeVisitor& visitor) const override;

    private:
      Literal m_literal;
  };

  inline LiteralExpression::LiteralExpression(Location l, Literal literal)
      : Expression(std::move(l)),
        m_literal(std::move(literal)) {}

  inline const Literal& LiteralExpression::get_literal() const {
    return m_literal;
  }

  inline void LiteralExpression::apply(SyntaxNodeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline void SyntaxNodeVisitor::visit(const LiteralExpression& node) {
    visit(static_cast<const Expression&>(node));
  }
}

#endif
