#ifndef DARCEL_EXPRESSION_HPP
#define DARCEL_EXPRESSION_HPP
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! The base class for a syntax node representing an expression.
  class expression : public statement {
    public:
      using statement::statement;
  };

  inline void syntax_node_visitor::visit(const expression& node) {
    visit(static_cast<const statement&>(node));
  }
}

#endif
