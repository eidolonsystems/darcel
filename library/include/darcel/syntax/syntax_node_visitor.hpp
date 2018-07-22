#ifndef DARCEL_SYNTAX_NODE_VISITOR_HPP
#define DARCEL_SYNTAX_NODE_VISITOR_HPP
#include "darcel/syntax/syntax.hpp"

namespace darcel {

  //! Implements the visitor pattern for syntax nodes.
  class SyntaxNodeVisitor {
    public:
      virtual ~SyntaxNodeVisitor() = default;

      virtual void visit(const BindEnumStatement& node);

      virtual void visit(const BindFunctionStatement& node);

      virtual void visit(const BindVariableStatement& node);

      virtual void visit(const CallExpression& node);

      virtual void visit(const EnumExpression& node);

      virtual void visit(const Expression& node);

      virtual void visit(const FunctionExpression& node);

      virtual void visit(const LiteralExpression& node);

      virtual void visit(const Statement& node);

      virtual void visit(const SyntaxNode& node);

      virtual void visit(const TerminalNode& node);

      virtual void visit(const VariableExpression& node);

    protected:

      //! Constructs a syntax node visitor.
      SyntaxNodeVisitor() = default;
  };

  inline void SyntaxNodeVisitor::visit(const SyntaxNode& node) {}
}

#endif
