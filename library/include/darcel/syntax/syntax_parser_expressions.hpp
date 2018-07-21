#ifndef DARCEL_SYNTAX_PARSER_EXPRESSIONS_HPP
#define DARCEL_SYNTAX_PARSER_EXPRESSIONS_HPP
#include <cassert>
#include <deque>
#include <stack>
#include "darcel/lexicon/token.hpp"
#include "darcel/syntax/arity_syntax_error.hpp"
#include "darcel/syntax/call_expression.hpp"
#include "darcel/syntax/enum_expression.hpp"
#include "darcel/syntax/function_expression.hpp"
#include "darcel/syntax/invalid_member_syntax_error.hpp"
#include "darcel/syntax/literal_expression.hpp"
#include "darcel/syntax/ops.hpp"
#include "darcel/syntax/redefinition_syntax_error.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/syntax/syntax_error.hpp"
#include "darcel/syntax/syntax_parser.hpp"
#include "darcel/syntax/unmatched_bracket_syntax_error.hpp"

namespace darcel {
  inline std::unique_ptr<enum_expression> syntax_parser::parse_enum_expression(
      token_iterator& cursor) {
    auto c = cursor;
    auto name = try_parse_identifier(c);
    if(!name.has_value()) {
      return nullptr;
    }
    auto e = get_current_scope().find<EnumDataType>(*name);
    if(e == nullptr) {
      return nullptr;
    }
    if(!match(*c, Punctuation::Mark::DOT)) {
      return nullptr;
    }
    ++c;
    auto symbol_location = c.get_location();
    auto symbol = parse_identifier(c);
    auto index = get_index(*e, symbol);
    if(index == -1) {
      throw invalid_member_syntax_error(std::move(symbol_location),
        std::move(e), std::move(symbol));
    }
    auto s = std::make_unique<enum_expression>(cursor.get_location(),
      std::move(e), index);
    cursor = c;
    return s;
  }

  inline std::unique_ptr<function_expression>
      syntax_parser::parse_function_expression(token_iterator& cursor) {
    auto c = cursor;
    auto name = try_parse_identifier(c);
    if(!name.has_value()) {
      return nullptr;
    }
    auto f = get_current_scope().find<Function>(*name);
    if(f == nullptr) {
      return nullptr;
    }
    auto node = std::make_unique<function_expression>(cursor.get_location(),
      std::move(f));
    cursor = c;
    return node;
  }

  inline std::unique_ptr<literal_expression> syntax_parser::
      parse_literal_expression(token_iterator& cursor) {
    return std::visit(
      [&] (auto&& value) -> std::unique_ptr<literal_expression> {
        using T = std::decay_t<decltype(value)>;
        if constexpr(std::is_same_v<T, Literal>) {
          auto expression = std::make_unique<literal_expression>(
            cursor.get_location(), value);
          ++cursor;
          return expression;
        }
        return nullptr;
      },
      cursor->get_instance());
  }

  inline std::unique_ptr<variable_expression>
      syntax_parser::parse_variable_expression(token_iterator& cursor) {
    auto c = cursor;
    auto name = try_parse_identifier(c);
    if(!name.has_value()) {
      return nullptr;
    }
    try {
      auto node = make_variable_expression(cursor.get_location(),
        get_current_scope(), *name);
      cursor = c;
      return node;
    } catch(const variable_not_found_error&) {
      return nullptr;
    }
  }

  inline std::unique_ptr<expression> syntax_parser::parse_expression_term(
      token_iterator& cursor) {
    if(auto node = parse_function_expression(cursor)) {
      return node;
    } else if(auto node = parse_literal_expression(cursor)) {
      return node;
    } else if(auto node = parse_variable_expression(cursor)) {
      return node;
    } else if(auto node = parse_enum_expression(cursor)) {
      return node;
    }
    return nullptr;
  }

  inline std::unique_ptr<expression> syntax_parser::parse_expression(
      token_iterator& cursor) {
    struct op_token {
      op m_op;
      Location m_location;
    };
    std::deque<std::unique_ptr<expression>> expressions;
    std::stack<op_token> operators;
    auto build_call_expression =
      [&] (const op_token& o) {
        auto arity = get_arity(o.m_op);
        if(static_cast<int>(expressions.size()) < arity) {
          throw arity_syntax_error(o.m_location, expressions.size(), o.m_op);
        }
        std::vector<std::unique_ptr<expression>> arguments;
        for(auto i = 0; i < arity; ++i) {
          arguments.push_back(nullptr);
        }
        while(arity != 0) {
          arguments[arity - 1] = std::move(expressions.back());
          expressions.pop_back();
          --arity;
        }
        auto& function_name = get_function_name(o.m_op);
        auto f = get_current_scope().find<Function>(function_name);
        if(f == nullptr) {
          throw syntax_error(syntax_error_code::FUNCTION_NOT_FOUND,
            o.m_location);
        }
        auto callable = std::make_unique<function_expression>(o.m_location,
          std::move(f));
        auto call_expression = call(o.m_location, std::move(callable),
          std::move(arguments));
        expressions.push_back(std::move(call_expression));
      };
    auto c = cursor;
    enum class parse_mode {
      TERM,
      OPERATOR
    };
    auto mode = parse_mode::TERM;
    while(true) {
      if(mode == parse_mode::TERM) {
        if(match(*c, Bracket::Type::ROUND_OPEN)) {
          operators.push({op::OPEN_BRACKET, c.get_location()});
          ++c;
        } else if(auto node = parse_expression_term(c)) {
          expressions.push_back(std::move(node));
          mode = parse_mode::OPERATOR;
        } else {
          break;
        }
      } else {
        if(match(*c, Bracket::Type::ROUND_OPEN)) {
          auto call_location = c.get_location();
          ++c;
          std::vector<std::unique_ptr<expression>> arguments;
          if(!match(*c, Bracket::Type::ROUND_CLOSE)) {
            while(true) {
              auto argument = parse_expression(c);
              if(argument == nullptr) {
                throw syntax_error(syntax_error_code::EXPRESSION_EXPECTED,
                  c.get_location());
              }
              arguments.push_back(std::move(argument));
              if(match(*c, Bracket::Type::ROUND_CLOSE)) {
                break;
              }
              expect(c, Punctuation::Mark::COMMA);
            }
          }
          auto callable = std::move(expressions.back());
          expressions.pop_back();
          auto call_expression = call(call_location, std::move(callable),
            std::move(arguments));
          expressions.push_back(std::move(call_expression));
          ++c;
        } else if(c->get_type() == Token::Type::OPERATION) {
          auto& instance = std::get<Operation>(c->get_instance());
          auto o = get_binary_op(instance);
          while(!operators.empty() &&
              (operators.top().m_op != op::OPEN_BRACKET &&
              operators.top().m_op != op::CLOSE_BRACKET &&
              (get_precedence(operators.top().m_op) > get_precedence(o) ||
              get_precedence(operators.top().m_op) == get_precedence(o) &&
              get_associativity(o) == associativity::LEFT_TO_RIGHT))) {
            build_call_expression(operators.top());
            operators.pop();
          }
          operators.push({o, c.get_location()});
          ++c;
          mode = parse_mode::TERM;
        } else if(match(*c, Bracket::Type::ROUND_CLOSE)) {
          auto found_open_bracket = false;
          while(!operators.empty()) {
            auto o = operators.top();
            operators.pop();
            if(o.m_op == op::OPEN_BRACKET) {
              found_open_bracket = true;
              break;
            } else {
              build_call_expression(o);
            }
          }
          if(!found_open_bracket) {
            break;
          }
          ++c;
        } else {
          break;
        }
      }
    }
    while(!operators.empty()) {
      auto o = operators.top();
      operators.pop();
      if(o.m_op == op::OPEN_BRACKET || o.m_op == op::CLOSE_BRACKET) {
        auto bracket =
          [&] {
            if(o.m_op == op::OPEN_BRACKET) {
              return Bracket::Type::ROUND_OPEN;
            }
            return Bracket::Type::ROUND_CLOSE;
          }();
        throw unmatched_bracket_syntax_error(o.m_location, bracket);
      }
      build_call_expression(o);
    }
    if(expressions.empty()) {
      return nullptr;
    }
    auto e = std::move(expressions.front());
    expressions.pop_front();
    assert(expressions.empty());
    cursor = c;
    return e;
  }

  inline std::unique_ptr<expression> syntax_parser::expect_expression(
      token_iterator& cursor) {
    auto e = parse_expression(cursor);
    if(e == nullptr) {
      throw syntax_error(syntax_error_code::EXPRESSION_EXPECTED,
        cursor.get_location());
    }
    return e;
  }
}

#endif
