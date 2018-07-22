#ifndef DARCEL_STATEMENT_HPP
#define DARCEL_STATEMENT_HPP
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! The base class for a syntax node representing a statement.
  class Statement : public SyntaxNode {
    public:
      using SyntaxNode::SyntaxNode;
  };

  inline void SyntaxNodeVisitor::visit(const Statement& node) {
    visit(static_cast<const SyntaxNode&>(node));
  }
}

#endif
