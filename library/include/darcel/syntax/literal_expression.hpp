#ifndef DARCEL_LITERAL_EXPRESSION_HPP
#define DARCEL_LITERAL_EXPRESSION_HPP
#include <utility>
#include "darcel/lexicon/literal.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Represents an expression evaluating to a literal value.
  class literal_expression final : public expression {
    public:

      //! Constructs a literal_expression.
      /*!
        \param l The location of the token representing the literal value.
        \param literal The literal to evaluate to.
      */
      literal_expression(location l, literal literal);

      //! Returns the literal that is evaluated.
      const literal& get_literal() const;

      const std::shared_ptr<data_type>& get_data_type() const override;

      void apply(syntax_node_visitor& visitor) const override;

    private:
      literal m_literal;
  };

  inline literal_expression::literal_expression(location l, literal literal)
      : expression(std::move(l)),
        m_literal(std::move(literal)) {}

  inline const literal& literal_expression::get_literal() const {
    return m_literal;
  }

  inline const std::shared_ptr<data_type>& literal_expression::
      get_data_type() const {
    return m_literal.get_type();
  }

  inline void literal_expression::apply(syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(const literal_expression& node) {
    visit(static_cast<const expression&>(node));
  }
}

#endif
