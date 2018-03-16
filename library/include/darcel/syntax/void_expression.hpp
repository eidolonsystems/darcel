#ifndef DARCEL_VOID_EXPRESSION_HPP
#define DARCEL_VOID_EXPRESSION_HPP
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! An expression that evaluates to a void.
  class void_expression : public expression {
    public:
      using expression::expression;

      void apply(syntax_node_visitor& visitor) const override final;
  };

  inline void void_expression::apply(syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(const void_expression& node) {
    visit(static_cast<const expression&>(node));
  }
}

#endif
