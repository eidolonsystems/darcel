#ifndef DARCEL_SYNTAX_PARSER_EXPRESSIONS_HPP
#define DARCEL_SYNTAX_PARSER_EXPRESSIONS_HPP
#include <cassert>
#include <deque>
#include <stack>
#include "darcel/lexicon/token.hpp"
#include "darcel/syntax/arity_syntax_error.hpp"
#include "darcel/syntax/call_expression.hpp"
#include "darcel/syntax/function_expression.hpp"
#include "darcel/syntax/literal_expression.hpp"
#include "darcel/syntax/ops.hpp"
#include "darcel/syntax/redefinition_syntax_error.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_builders.hpp"
#include "darcel/syntax/syntax_error.hpp"
#include "darcel/syntax/syntax_parser.hpp"
#include "darcel/syntax/unmatched_bracket_syntax_error.hpp"

namespace darcel {
  inline std::unique_ptr<function_expression>
      syntax_parser::parse_function_expression(token_iterator& cursor) {
    auto c = cursor;
    auto name = try_parse_identifier(c);
    if(!name.has_value()) {
      return nullptr;
    }
    auto f = get_current_scope().find<function>(*name);
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
        if constexpr(std::is_same_v<T, literal>) {
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
      auto node = make_variable_expression(cursor.get_location(), *name,
        get_current_scope());
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
    }
    return nullptr;
  }

  inline std::unique_ptr<expression> syntax_parser::parse_expression(
      token_iterator& cursor) {
    struct op_token {
      op m_op;
      location m_location;
    };
    std::deque<std::unique_ptr<expression>> expressions;
    std::stack<op_token> operators;
    auto build_call_expression =
      [&] (const op_token& o) {
        auto arity = get_arity(o.m_op);
        if(static_cast<int>(expressions.size()) < arity) {
          throw arity_syntax_error(o.m_location, expressions.size(), o.m_op);
        }
        std::vector<std::unique_ptr<expression>> parameters;
        for(auto i = 0; i < arity; ++i) {
          parameters.push_back(nullptr);
        }
        while(arity != 0) {
          parameters[arity - 1] = std::move(expressions.back());
          expressions.pop_back();
          --arity;
        }
        auto& function_name = get_function_name(o.m_op);
        auto f = get_current_scope().find<function>(function_name);
        if(f == nullptr) {
          throw syntax_error(syntax_error_code::FUNCTION_NOT_FOUND,
            o.m_location);
        }
        std::vector<function_data_type::parameter> types;
        std::transform(parameters.begin(), parameters.end(),
          std::back_inserter(types),
          [] (auto& p) {
            return function_data_type::parameter("", p->get_data_type());
          });
        auto callable = find_overload(*f, types);
        if(callable == nullptr) {
          throw syntax_error(syntax_error_code::FUNCTION_NOT_FOUND,
            o.m_location);
        }
        auto call = std::make_unique<call_expression>(o.m_location,
          std::make_unique<variable_expression>(o.m_location, callable),
          std::move(parameters));
        expressions.push_back(std::move(call));
      };
    auto c = cursor;
    enum class parse_mode {
      TERM,
      OPERATOR
    };
    auto mode = parse_mode::TERM;
    while(true) {
      if(mode == parse_mode::TERM) {
        if(match(*c, bracket::type::OPEN_ROUND_BRACKET)) {
          operators.push({op::OPEN_BRACKET, c.get_location()});
          ++c;
        } else if(auto node = parse_expression_term(c)) {
          expressions.push_back(std::move(node));
          mode = parse_mode::OPERATOR;
        } else {
          break;
        }
      } else {
        if(match(*c, bracket::type::OPEN_ROUND_BRACKET)) {
          auto call_location = c.get_location();
          ++c;
          std::vector<std::unique_ptr<expression>> parameters;
          if(!match(*c, bracket::type::CLOSE_ROUND_BRACKET)) {
            while(true) {
              auto parameter = parse_expression(c);
              if(parameter == nullptr) {
                throw syntax_error(syntax_error_code::EXPRESSION_EXPECTED,
                  c.get_location());
              }
              parameters.push_back(std::move(parameter));
              if(match(*c, bracket::type::CLOSE_ROUND_BRACKET)) {
                break;
              }
              expect(c, punctuation::mark::COMMA);
            }
          }
          auto callable = std::move(expressions.back());
          expressions.pop_back();
          if(auto f = dynamic_cast<const function_expression*>(
              callable.get())) {
            std::vector<function_data_type::parameter> types;
            std::transform(parameters.begin(), parameters.end(),
              std::back_inserter(types),
              [] (auto& p) {
                return function_data_type::parameter("", p->get_data_type());
              });
            auto overload = find_overload(*f->get_function(), types);
            if(overload == nullptr) {
              throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND,
                call_location);
            }
            callable = std::make_unique<variable_expression>(
              f->get_location(), std::move(overload));
          } else if(auto type = std::dynamic_pointer_cast<function_data_type>(
              callable->get_data_type())) {
            if(type->get_parameters().size() != parameters.size()) {
              throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND,
                call_location);
            }
            for(std::size_t i = 0; i < parameters.size(); ++i) {
              if(*parameters[i]->get_data_type() !=
                  *type->get_parameters()[i].m_type) {
                throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND,
                  call_location);
              }
            }
          } else {
            throw syntax_error(syntax_error_code::EXPRESSION_NOT_CALLABLE,
              call_location);
          }
          auto call = std::make_unique<call_expression>(call_location,
            std::move(callable), std::move(parameters));
          expressions.push_back(std::move(call));
          ++c;
        } else if(c->get_type() == token::type::OPERATION) {
          auto& instance = std::get<operation>(c->get_instance());
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
        } else if(match(*c, bracket::type::CLOSE_ROUND_BRACKET)) {
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
              return bracket::type::OPEN_ROUND_BRACKET;
            }
            return bracket::type::CLOSE_ROUND_BRACKET;
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
