#ifndef DARCEL_SYNTAX_PARSER_STATEMENTS_HPP
#define DARCEL_SYNTAX_PARSER_STATEMENTS_HPP
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/lexicon/token.hpp"
#include "darcel/syntax/bind_enum_statement.hpp"
#include "darcel/syntax/bind_function_statement.hpp"
#include "darcel/syntax/bind_variable_statement.hpp"
#include "darcel/syntax/invalid_enum_value_syntax_error.hpp"
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"
#include "darcel/syntax/syntax_parser.hpp"
#include "darcel/utilities/utilities.hpp"

namespace darcel {
  inline std::unique_ptr<bind_enum_statement>
      syntax_parser::parse_bind_enum_statement(token_iterator& cursor) {
    auto c = cursor;
    if(!match(*c, Keyword::Word::LET)) {
      return nullptr;
    }
    ++c;
    auto name_location = c.get_location();
    auto& name = parse_identifier(c);
    expect(c, operation::symbol::ASSIGN);
    if(!match(*c, Keyword::Word::ENUM)) {
      return nullptr;
    }
    ++c;
    expect(c, Bracket::Type::ROUND_OPEN);
    std::vector<EnumDataType::Symbol> symbols;
    std::unordered_map<std::string, Location> locations;
    auto next_value = 0;
    while(!match(*c, Bracket::Type::ROUND_CLOSE)) {
      auto symbol_location = c.get_location();
      auto name = parse_identifier(c);
      auto existing_symbol = locations.find(name);
      if(existing_symbol != locations.end()) {
        throw redefinition_syntax_error(symbol_location, name,
          existing_symbol->second);
      }
      auto value = [&] {
        if(match(*c, operation::symbol::ASSIGN)) {
          ++c;
          auto expression_token = c.get_location();
          auto value_expression = dynamic_pointer_cast<literal_expression>(
            expect_expression(c));
          if(value_expression == nullptr) {
            throw syntax_error(syntax_error_code::EXPRESSION_EXPECTED,
              expression_token);
          }
          if(*value_expression->get_literal().get_type() !=
              *IntegerDataType::get_instance()) {
            throw syntax_error(syntax_error_code::INTEGER_EXPRESSION_EXPECTED,
              expression_token);
          }
          auto value = std::stoi(value_expression->get_literal().get_value());
          if(value < next_value) {
            throw invalid_enum_value_syntax_error(expression_token, next_value);
          }
          return value;
        } else {
          return next_value;
        }
      }();
      next_value = value + 1;
      symbols.push_back({name, value});
      locations.insert(std::make_pair(name, symbol_location));
      if(match(*c, Bracket::Type::ROUND_CLOSE)) {
        break;
      }
      expect(c, punctuation::mark::COMMA);
    }
    ++c;
    auto statement = bind_enum(cursor.get_location(), get_current_scope(), name,
      std::move(symbols));
    cursor = c;
    return statement;
  }

  inline std::unique_ptr<bind_function_statement>
      syntax_parser::parse_bind_function_statement(token_iterator& cursor) {
    auto c = cursor;
    if(!match(*c, Keyword::Word::LET)) {
      return nullptr;
    }
    ++c;
    auto name_location = c.get_location();
    auto& name = parse_identifier(c);
    if(!match(*c, Bracket::Type::ROUND_OPEN)) {
      return nullptr;
    }
    ++c;
    std::vector<bind_function_statement::parameter> parameters;
    push_scope();
    if(!match(*c, Bracket::Type::ROUND_CLOSE)) {
      while(true) {
        auto name_location = c.get_location();
        auto& parameter_name = parse_identifier(c);
        auto existing_parameter = std::find_if(parameters.begin(),
          parameters.end(),
          [&] (auto& p) {
            return p.m_variable->get_name() == parameter_name;
          });
        if(existing_parameter != parameters.end()) {
          throw syntax_error(
            syntax_error_code::FUNCTION_PARAMETER_ALREADY_DEFINED,
            name_location);
        }
        auto v = std::make_shared<variable>(name_location, parameter_name);
        if(match(*c, punctuation::mark::COLON)) {
          ++c;
          auto t = expect_data_type(c);
          parameters.emplace_back(std::move(v), std::move(t));
        } else {
          parameters.emplace_back(std::move(v));
        }
        if(match(*c, Bracket::Type::ROUND_CLOSE)) {
          break;
        }
        expect(c, punctuation::mark::COMMA);
      }
    }
    ++c;
    expect(c, operation::symbol::ASSIGN);
    for(auto& parameter : parameters) {
      get_current_scope().add(parameter.m_variable);
    }
    push_scope();
    auto initializer = expect_expression(c);
    pop_scope();
    pop_scope();
    auto f = [&] {
      auto existing_element = get_current_scope().find_within(name);
      if(existing_element == nullptr) {
        auto parent = get_current_scope().find<function>(name);
        auto f = [&] {
          if(parent == nullptr) {
            return std::make_shared<function>(name_location, name);
          } else {
            return std::make_shared<function>(name_location, std::move(parent));
          }
        }();
        get_current_scope().add(f);
        return f;
      }
      auto f = std::dynamic_pointer_cast<function>(existing_element);
      if(f == nullptr) {
        throw redefinition_syntax_error(name_location, name,
          existing_element->get_location());
      }
      return f;
    }();
    auto statement = std::make_unique<bind_function_statement>(
      cursor.get_location(), std::move(f), std::move(parameters),
      std::move(initializer));
    cursor = c;
    return statement;
  }

  inline std::unique_ptr<bind_variable_statement>
      syntax_parser::parse_bind_variable_statement(token_iterator& cursor) {
    auto c = cursor;
    if(!match(*c, Keyword::Word::LET)) {
      return nullptr;
    }
    ++c;
    auto name_location = c.get_location();
    auto& name = parse_identifier(c);
    expect(c, operation::symbol::ASSIGN);
    auto initializer = expect_expression(c);
    auto statement = bind_variable(cursor.get_location(), get_current_scope(),
      name, std::move(initializer));
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
    if((node = parse_bind_function_statement(c)) != nullptr ||
        (node = parse_bind_enum_statement(c)) != nullptr ||
        (node = parse_bind_variable_statement(c)) != nullptr) {
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
