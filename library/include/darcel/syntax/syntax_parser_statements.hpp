#ifndef DARCEL_SYNTAX_PARSER_STATEMENTS_HPP
#define DARCEL_SYNTAX_PARSER_STATEMENTS_HPP
#include <cassert>
#include "darcel/lexicon/token.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"
#include "darcel/syntax/syntax_parser.hpp"

namespace darcel {
  inline std::unique_ptr<bind_variable_statement>
      syntax_parser::parse_bind_variable_statement(token_iterator& cursor) {
    auto c = cursor;
    if(!match(*c, keyword::word::LET)) {
      return nullptr;
    }
    ++c;
    auto& name = parse_identifier(c);
    expect(c, punctuation::mark::::symbol::ASSIGN);
    auto initializer = expect_expression(c);
    auto expression = std::make_unique<let_expression>(cursor.get_location(),
      name, std::move(initializer));
    cursor = c;
    return expression;
  }

  inline std::unique_ptr<terminal_node> syntax_parser::parse_terminal_node(
      token_iterator& cursor) {
    if(!cursor.is_empty() && match(*cursor, terminal::type::end_of_file)) {
      auto t = std::make_unique<terminal_node>(cursor.get_location());
      ++cursor;
      return t;
    }
    return nullptr;
  }

  inline std::unique_ptr<statement> syntax_parser::parse_statement(
      token_iterator& cursor) {
    auto c = cursor;
    while(!c.is_empty() && match(*c, terminal::type::new_line)) {
      ++c;
    }
    std::unique_ptr<statement> node;
    if((node = parse_bind_variable_statement(c)) != nullptr) {
      if(!is_syntax_node_end(*c)) {
        throw syntax_error(syntax_error_code::NEW_LINE_EXPECTED,
          c.get_location());
      }
      while(!c.is_empty() && match(*c, terminal::type::new_line)) {
        ++c;
      }
      cursor = c;
      return node;
    } else if(auto expression = parse_expression(c)) {
      if(c.is_empty()) {
        return nullptr;
      } else {
        node = std::move(expression);
      }
      if(!is_syntax_node_end(*c)) {
        throw syntax_error(syntax_error_code::NEW_LINE_EXPECTED,
          c.get_location());
      }
      while(!c.is_empty() && match(*c, terminal::type::new_line)) {
        ++c;
      }
      cursor = c;
      return node;
    }
    return nullptr;
  }

  inline std::unique_ptr<statement> syntax_parser::expect_statement(
      token_iterator& cursor) {
    auto s = parse_statement(cursor);
    if(s == nullptr) {
      throw syntax_error(syntax_error_code::STATEMENT_EXPECTED,
        cursor.get_location());
    }
    return s;
  }
}

#endif
