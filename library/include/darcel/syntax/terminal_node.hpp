#ifndef DARCEL_TERMINAL_NODE_HPP
#define DARCEL_TERMINAL_NODE_HPP
#include "darcel/lexicon/terminal.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Represents a terminal token.
  class terminal_node : public syntax_node {
    public:
      using syntax_node::syntax_node;

      void apply(syntax_node_visitor& visitor) const override final;
  };

  inline void terminal_node::apply(syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(const terminal_node& node) {
    visit(static_cast<const syntax_node&>(node));
  }
}

#endif
