#ifndef DARCEL_STATEMENT_HPP
#define DARCEL_STATEMENT_HPP
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! The base class for a syntax node representing a statement.
  class statement : public syntax_node {
    public:
      using syntax_node::syntax_node;
  };

  inline void syntax_node_visitor::visit(const statement& node) {
    visit(static_cast<const syntax_node&>(node));
  }
}

#endif
