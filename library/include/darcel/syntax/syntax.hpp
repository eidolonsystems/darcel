#ifndef DARCEL_SYNTAX_HPP
#define DARCEL_SYNTAX_HPP

namespace darcel {
  class expression;
  class statement;
  enum class syntax_error_code;
  class syntax_error;
  class syntax_node;
  class syntax_node_visitor;
  class syntax_parser;
  class terminal_node;
  class token_iterator;
  class unmatched_bracket_syntax_error;
  class void_expression;
}

#endif
