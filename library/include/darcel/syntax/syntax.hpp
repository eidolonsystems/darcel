#ifndef DARCEL_SYNTAX_HPP
#define DARCEL_SYNTAX_HPP

namespace darcel {
  class AritySyntaxError;
  class BindEnumStatement;
  class BindFunctionStatement;
  class BindVariableStatement;
  class CallExpression;
  class InvalidEnumValueSyntaxError;
  class InvalidMemberSyntaxError;
  class EnumExpression;
  class Expression;
  class FunctionExpression;
  class LiteralExpression;
  class RedefinitionSyntaxError;
  class Statement;
  enum class SyntaxErrorCode;
  class SyntaxError;
  class SyntaxNode;
  class SyntaxNodeVisitor;
  class SyntaxParser;
  class TerminalNode;
  class TokenIterator;
  class UnmatchedBracketSyntaxError;
  class VariableExpression;
  class VariableNotFoundError;
}

#endif
