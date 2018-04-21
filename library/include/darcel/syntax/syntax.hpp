#ifndef DARCEL_SYNTAX_HPP
#define DARCEL_SYNTAX_HPP

namespace darcel {
  class arity_syntax_error;
  class bind_enum_statement;
  class bind_function_statement;
  class bind_variable_statement;
  class call_expression;
  class invalid_enum_value_syntax_error;
  class invalid_member_syntax_error;
  class enum_expression;
  class expression;
  class function_expression;
  class literal_expression;
  class redefinition_syntax_error;
  class statement;
  enum class syntax_error_code;
  class syntax_error;
  class syntax_node;
  class syntax_node_visitor;
  class syntax_parser;
  class terminal_node;
  class token_iterator;
  class unmatched_bracket_syntax_error;
  class variable_expression;
  class variable_not_found_error;
}

#endif
