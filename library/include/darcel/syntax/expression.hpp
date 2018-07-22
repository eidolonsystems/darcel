#ifndef DARCEL_EXPRESSION_HPP
#define DARCEL_EXPRESSION_HPP
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! The base class for a syntax node representing an expression.
  class Expression : public Statement {
    public:
      using Statement::Statement;
  };

  inline void SyntaxNodeVisitor::visit(const Expression& node) {
    visit(static_cast<const Statement&>(node));
  }
}

#endif
