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
  inline std::unique_ptr<BindEnumStatement>
      SyntaxParser::parse_bind_enum_statement(TokenIterator& cursor) {
    auto c = cursor;
    if(!match(*c, Keyword::Word::LET)) {
      return nullptr;
    }
    ++c;
    auto name_location = c.get_location();
    auto& name = parse_identifier(c);
    expect(c, Operation::Symbol::ASSIGN);
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
        throw RedefinitionSyntaxError(symbol_location, name,
          existing_symbol->second);
      }
      auto value = [&] {
        if(match(*c, Operation::Symbol::ASSIGN)) {
          ++c;
          auto expression_token = c.get_location();
          auto value_expression = dynamic_pointer_cast<LiteralExpression>(
            expect_expression(c));
          if(value_expression == nullptr) {
            throw SyntaxError(SyntaxErrorCode::EXPRESSION_EXPECTED,
              expression_token);
          }
          if(*value_expression->get_literal().get_type() !=
              *IntegerDataType::get_instance()) {
            throw SyntaxError(SyntaxErrorCode::INTEGER_EXPRESSION_EXPECTED,
              expression_token);
          }
          auto value = std::stoi(value_expression->get_literal().get_value());
          if(value < next_value) {
            throw InvalidEnumValueSyntaxError(expression_token, next_value);
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
      expect(c, Punctuation::Mark::COMMA);
    }
    ++c;
    auto statement = bind_enum(cursor.get_location(), get_current_scope(), name,
      std::move(symbols));
    cursor = c;
    return statement;
  }

  inline std::unique_ptr<BindFunctionStatement>
      SyntaxParser::parse_bind_function_statement(TokenIterator& cursor) {
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
    std::vector<BindFunctionStatement::parameter> parameters;
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
          throw SyntaxError(
            SyntaxErrorCode::FUNCTION_PARAMETER_ALREADY_DEFINED,
            name_location);
        }
        auto v = std::make_shared<Variable>(name_location, parameter_name);
        if(match(*c, Punctuation::Mark::COLON)) {
          ++c;
          auto t = expect_data_type(c);
          parameters.emplace_back(std::move(v), std::move(t));
        } else {
          parameters.emplace_back(std::move(v));
        }
        if(match(*c, Bracket::Type::ROUND_CLOSE)) {
          break;
        }
        expect(c, Punctuation::Mark::COMMA);
      }
    }
    ++c;
    expect(c, Operation::Symbol::ASSIGN);
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
        auto parent = get_current_scope().find<Function>(name);
        auto f = [&] {
          if(parent == nullptr) {
            return std::make_shared<Function>(name_location, name);
          } else {
            return std::make_shared<Function>(name_location, std::move(parent));
          }
        }();
        get_current_scope().add(f);
        return f;
      }
      auto f = std::dynamic_pointer_cast<Function>(existing_element);
      if(f == nullptr) {
        throw RedefinitionSyntaxError(name_location, name,
          existing_element->get_location());
      }
      return f;
    }();
    auto statement = std::make_unique<BindFunctionStatement>(
      cursor.get_location(), std::move(f), std::move(parameters),
      std::move(initializer));
    cursor = c;
    return statement;
  }

  inline std::unique_ptr<BindVariableStatement>
      SyntaxParser::parse_bind_variable_statement(TokenIterator& cursor) {
    auto c = cursor;
    if(!match(*c, Keyword::Word::LET)) {
      return nullptr;
    }
    ++c;
    auto name_location = c.get_location();
    auto& name = parse_identifier(c);
    expect(c, Operation::Symbol::ASSIGN);
    auto initializer = expect_expression(c);
    auto statement = bind_variable(cursor.get_location(), get_current_scope(),
      name, std::move(initializer));
    cursor = c;
    return statement;
  }

  inline std::unique_ptr<TerminalNode> SyntaxParser::parse_terminal_node(
      TokenIterator& cursor) {
    if(!cursor.is_empty() && match(*cursor, Terminal::Type::END_OF_FILE)) {
      auto t = std::make_unique<TerminalNode>(cursor.get_location());
      ++cursor;
      return t;
    }
    return nullptr;
  }

  inline std::unique_ptr<Statement> SyntaxParser::parse_statement(
      TokenIterator& cursor) {
    auto c = cursor;
    while(!c.is_empty() && match(*c, Terminal::Type::NEW_LINE)) {
      ++c;
    }
    std::unique_ptr<Statement> node;
    if((node = parse_bind_function_statement(c)) != nullptr ||
        (node = parse_bind_enum_statement(c)) != nullptr ||
        (node = parse_bind_variable_statement(c)) != nullptr) {
      if(!is_syntax_node_end(*c)) {
        throw SyntaxError(SyntaxErrorCode::NEW_LINE_EXPECTED,
          c.get_location());
      }
      while(!c.is_empty() && match(*c, Terminal::Type::NEW_LINE)) {
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
        throw SyntaxError(SyntaxErrorCode::NEW_LINE_EXPECTED,
          c.get_location());
      }
      while(!c.is_empty() && match(*c, Terminal::Type::NEW_LINE)) {
        ++c;
      }
      cursor = c;
      return node;
    }
    return nullptr;
  }

  inline std::unique_ptr<Statement> SyntaxParser::expect_statement(
      TokenIterator& cursor) {
    auto s = parse_statement(cursor);
    if(s == nullptr) {
      throw SyntaxError(SyntaxErrorCode::STATEMENT_EXPECTED,
        cursor.get_location());
    }
    return s;
  }
}

#endif
