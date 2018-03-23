#ifndef DARCEL_SYNTAX_PARSER_STATEMENTS_HPP
#define DARCEL_SYNTAX_PARSER_STATEMENTS_HPP
#include "darcel/data_types/function_data_type.hpp"
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
    auto name_location = c.get_location();
    auto& name = parse_identifier(c);
    std::vector<function_data_type::parameter> parameters;
    auto is_function = match(*c, bracket::type::OPEN_ROUND_BRACKET);
    if(is_function) {
      ++c;
      expect(c, bracket::type::CLOSE_ROUND_BRACKET);
    }
    expect(c, operation::symbol::ASSIGN);
    auto initializer = expect_expression(c);
    auto existing_element = get_current_scope().find_within(name);
    if(existing_element != nullptr) {
      throw redefinition_syntax_error(name_location, name,
        existing_element->get_location());
    }
    auto type = [&] () -> std::shared_ptr<data_type> {
      if(is_function) {
        return std::make_shared<function_data_type>(std::move(parameters),
          initializer->get_data_type());
      }
      return initializer->get_data_type();
    }();
    auto v = std::make_shared<variable>(cursor.get_location(), name,
      std::move(type));
    get_current_scope().add(v);
    auto statement = std::make_unique<bind_variable_statement>(
      cursor.get_location(), std::move(v), std::move(initializer));
    cursor = c;
    return statement;
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
