#ifndef DARCEL_SYNTAX_NODE_VISITOR_HPP
#define DARCEL_SYNTAX_NODE_VISITOR_HPP
#include "darcel/syntax/syntax.hpp"

namespace darcel {

  //! Implements the visitor pattern for syntax nodes.
  class syntax_node_visitor {
    public:
      virtual ~syntax_node_visitor() = default;

      virtual void visit(const bind_variable_statement& node);

      virtual void visit(const call_expression& node);

      virtual void visit(const expression& node);

      virtual void visit(const literal_expression& node);

      virtual void visit(const statement& node);

      virtual void visit(const syntax_node& node);

      virtual void visit(const terminal_node& node);

      virtual void visit(const variable_expression& node);

    protected:

      //! Constructs a syntax node visitor.
      syntax_node_visitor() = default;
  };

  inline void syntax_node_visitor::visit(const syntax_node& node) {}
}

#endif
