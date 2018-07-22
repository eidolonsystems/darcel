#ifndef DARCEL_TERMINAL_NODE_HPP
#define DARCEL_TERMINAL_NODE_HPP
#include "darcel/lexicon/terminal.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Represents a terminal token.
  class TerminalNode final : public SyntaxNode {
    public:
      using SyntaxNode::SyntaxNode;

      void apply(SyntaxNodeVisitor& visitor) const override;
  };

  inline void TerminalNode::apply(SyntaxNodeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline void SyntaxNodeVisitor::visit(const TerminalNode& node) {
    visit(static_cast<const SyntaxNode&>(node));
  }
}

#endif
