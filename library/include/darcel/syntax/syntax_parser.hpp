#ifndef DARCEL_SYNTAX_PARSER_HPP
#define DARCEL_SYNTAX_PARSER_HPP
#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/lexicon/token.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax_error_code.hpp"
#include "darcel/syntax/syntax_error.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node.hpp"
#include "darcel/syntax/terminal_node.hpp"
#include "darcel/syntax/token_iterator.hpp"
#include "darcel/syntax/unmatched_bracket_syntax_error.hpp"

namespace darcel {

  //! Parses syntax nodes from tokens.
  class syntax_parser {
    public:

      //! Constructs a default syntax parser.
      syntax_parser();

      //! Constructs a syntax parser within an existing scope.
      /*!
        \param s The top-level scope.
      */
      syntax_parser(std::unique_ptr<scope> s);

      //! Returns the top-level scope.
      const scope& get_scope() const;

      //! Feeds this parser a token.
      /*!
        \param t The token to feed.
      */
      void feed(token t);

      //! Returns an iterator to the next terminal token.
      token_iterator get_next_terminal() const;

      //! Parses the next syntax node.
      /*!
        \return The syntax node parsed from the previously fed tokens or
                <code>nullptr</code> iff no syntax node is available.
      */
      std::unique_ptr<syntax_node> parse_node();

    private:
      std::deque<std::unique_ptr<scope>> m_scopes;
      std::vector<token> m_tokens;
      token_iterator m_cursor;
      int m_generic_index;

      syntax_parser(const syntax_parser&) = delete;
      syntax_parser& operator =(const syntax_parser&) = delete;
      scope& get_current_scope();
      scope& push_scope();
      void pop_scope();
      token_iterator get_next_terminal(token_iterator cursor) const;
      std::shared_ptr<function_data_type> parse_function_data_type(
        token_iterator& cursor);
      std::shared_ptr<generic_data_type> parse_generic_data_type(
        token_iterator& cursor);
      std::shared_ptr<data_type> parse_data_type(token_iterator& cursor);
      std::shared_ptr<data_type> expect_data_type(token_iterator& cursor);
      std::unique_ptr<syntax_node> parse_node(token_iterator& cursor);
      std::unique_ptr<terminal_node> parse_terminal_node(
        token_iterator& cursor);
      std::unique_ptr<statement> parse_statement(token_iterator& cursor);
      std::unique_ptr<statement> expect_statement(token_iterator& cursor);
      std::unique_ptr<bind_function_statement> parse_bind_function_statement(
        token_iterator& cursor);
      std::unique_ptr<bind_variable_statement> parse_bind_variable_statement(
        token_iterator& cursor);
      std::unique_ptr<function_expression> parse_function_expression(
        token_iterator& cursor);
      std::unique_ptr<literal_expression> parse_literal_expression(
        token_iterator& cursor);
      std::unique_ptr<variable_expression> parse_variable_expression(
        token_iterator& cursor);
      std::unique_ptr<expression> parse_expression_term(token_iterator& cursor);
      std::unique_ptr<expression> parse_expression(token_iterator& cursor);
      std::unique_ptr<expression> expect_expression(token_iterator& cursor);
  };

  //! Parses an identifier from a token stream.
  /*!
    \param cursor An iterator to the first token to parse.
    \return The symbol represented by the parsed identifier.
  */
  inline const std::string& parse_identifier(token_iterator& cursor) {
    if(cursor.is_empty()) {
      throw syntax_error(syntax_error_code::IDENTIFIER_EXPECTED,
        cursor.get_location());
    }
    return std::visit(
      [&] (auto&& value) -> const std::string& {
        using T = std::decay_t<decltype(value)>;
        if constexpr(std::is_same_v<T, identifier>) {
          ++cursor;
          return value.get_symbol();
        }
        throw syntax_error(syntax_error_code::IDENTIFIER_EXPECTED,
          cursor.get_location());
      },
      cursor->get_instance());
  }

  //! Parses an identifier from a token stream.
  /*!
    \param cursor An iterator to the first token to parse.
    \return The symbol represented by the parsed identifier.
  */
  inline std::optional<std::string> try_parse_identifier(
      token_iterator& cursor) {
    try {
      return parse_identifier(cursor);
    } catch(const syntax_error&) {
      return std::nullopt;
    }
  }

  //! Tests if a token represents the end of a syntax node.
  /*!
    \param t The token to test.
    \return <code>true</code> iff <i>t</i> ends a syntax node.
  */
  inline bool is_syntax_node_end(const token& t) {
    return is_terminal(t);
  }

  //! Ensures that the token represented by an iterator is equal to some other
  //! token, throwing a syntax_error otherwise.
  /*!
    \param cursor The iterator to test, this iterator is advanced past the
           the location where the expected token is located.
    \param t The token to expect.
  */
  inline void expect(token_iterator& cursor, const token::instance& t) {
    auto c = cursor;
    while(!c.is_empty() && match(*c, terminal::type::new_line)) {
      ++c;
    }
    if(c.is_empty() || c->get_instance() != t) {
      std::visit(
        [&] (auto&& instance) {
          using T = std::decay_t<decltype(instance)>;
          if constexpr(std::is_same_v<T, punctuation>) {
            if(instance == punctuation::mark::COMMA) {
              throw syntax_error(syntax_error_code::COMMA_EXPECTED,
                cursor.get_location());
            }
          } else if constexpr(std::is_same_v<T, bracket>) {
            if(instance == bracket::type::OPEN_ROUND_BRACKET) {
              throw syntax_error(syntax_error_code::OPEN_ROUND_BRACKET_EXPECTED,
                cursor.get_location());
            }
          } else if constexpr(std::is_same_v<T, operation>) {
            if(instance == operation::symbol::ASSIGN) {
              throw syntax_error(syntax_error_code::ASSIGNMENT_EXPECTED,
                cursor.get_location());
            }
          }
        }, t);
    }
    ++c;
    cursor = c;
  }

  inline syntax_parser::syntax_parser()
      : syntax_parser(std::make_unique<scope>()) {}

  inline syntax_parser::syntax_parser(std::unique_ptr<scope> s) {
    m_scopes.push_back(std::move(s));
  }

  inline const scope& syntax_parser::get_scope() const {
    return *m_scopes.front();
  }

  inline void syntax_parser::feed(token t) {
    auto position = &*m_cursor - m_tokens.data();
    m_tokens.push_back(std::move(t));
    m_cursor.adjust(m_tokens.data() + position,
      m_cursor.get_size_remaining() + 1);
  }

  inline token_iterator syntax_parser::get_next_terminal() const {
    return get_next_terminal(m_cursor);
  }

  inline std::unique_ptr<syntax_node> syntax_parser::parse_node() {
    return parse_node(m_cursor);
  }

  inline scope& syntax_parser::get_current_scope() {
    return *m_scopes.back();
  }

  inline scope& syntax_parser::push_scope() {
    m_scopes.push_back(std::make_unique<scope>(&get_current_scope()));
    return get_current_scope();
  }

  inline void syntax_parser::pop_scope() {
    m_scopes.pop_back();
  }

  inline token_iterator syntax_parser::get_next_terminal(
      token_iterator cursor) const {
    if(cursor.is_empty() || is_terminal(*cursor)) {
      return cursor;
    }
    auto c = cursor;
    auto is_symbol = std::visit(
      [&] (auto&& t) {
        using T = std::decay_t<decltype(t)>;
        if constexpr(std::is_same_v<T, identifier> ||
            std::is_same_v<T, keyword> ||
            std::is_same_v<T, literal> ||
            std::is_same_v<T, punctuation>) {
          return true;
        }
        return false;
      }, c->get_instance());
    if(is_symbol) {
      ++c;
      return get_next_terminal(c);
    }
    if(std::get_if<operation>(&c->get_instance())) {
      ++c;
      if(c.is_empty()) {
        return cursor;
      }
      if(match(*c, terminal::type::new_line)) {
        ++c;
      }
      return get_next_terminal(c);
    }
    if(auto open_bracket = std::get_if<bracket>(&c->get_instance())) {
      if(!is_open(*open_bracket)) {
        return c;
      }
      auto l = c.get_location();
      ++c;
      auto end = get_next_terminal(c);
      if(end.is_empty()) {
        return cursor;
      }
      auto close_bracket = std::get_if<bracket>(&end->get_instance());
      if(close_bracket == nullptr ||
          get_opposite(*close_bracket) != *open_bracket) {
        throw unmatched_bracket_syntax_error(l, *open_bracket);
      }
      ++end;
      return get_next_terminal(end);
    }
    return cursor;
  }

  inline std::shared_ptr<function_data_type>
      syntax_parser::parse_function_data_type(token_iterator& cursor) {
    auto c = cursor;
    expect(c, bracket::type::OPEN_ROUND_BRACKET);
    std::vector<function_data_type::parameter> parameters;
    if(!match(*c, bracket::type::CLOSE_ROUND_BRACKET)) {
      while(true) {
        auto name_location = c.get_location();
        auto& parameter_name = parse_identifier(c);
        auto existing_parameter = std::find_if(parameters.begin(),
          parameters.end(),
          [&] (auto& p) {
            return p.m_name == parameter_name;
          });
        if(existing_parameter != parameters.end()) {
          throw syntax_error(
            syntax_error_code::FUNCTION_PARAMETER_ALREADY_DEFINED,
            name_location);
        }
        expect(c, punctuation::mark::COLON);
        auto parameter_type = expect_data_type(c);
        parameters.emplace_back(parameter_name, std::move(parameter_type));
        if(match(*c, bracket::type::CLOSE_ROUND_BRACKET)) {
          break;
        }
        expect(c, punctuation::mark::COMMA);
      }
    }
    ++c;
    expect(c, punctuation::mark::ARROW);
    auto return_type = expect_data_type(c);
    cursor = c;
    return std::make_shared<function_data_type>(std::move(parameters),
      std::move(return_type));
  }

  inline std::shared_ptr<generic_data_type>
      syntax_parser::parse_generic_data_type(token_iterator& cursor) {
    auto c = cursor;
    expect(c, punctuation::mark::BACKTICK);
    auto name_location = c.get_location();
    auto name = '`' + parse_identifier(c);
    auto t = get_current_scope().find<generic_data_type>(name);
    if(t == nullptr) {
      auto g = std::make_shared<generic_data_type>(name_location, name,
        m_generic_index);
      ++m_generic_index;
      get_current_scope().add(g);
      cursor = c;
      return g;
    }
    cursor = c;
    return t;
  }

  inline std::shared_ptr<data_type> syntax_parser::parse_data_type(
      token_iterator& cursor) {
    if(cursor.is_empty()) {
      return nullptr;
    }
    if(match(*cursor, bracket::type::OPEN_ROUND_BRACKET)) {
      return parse_function_data_type(cursor);
    }
    if(match(*cursor, punctuation::mark::BACKTICK)) {
      return parse_generic_data_type(cursor);
    }
    auto c = cursor;
    auto name_location = c.get_location();
    auto name = parse_identifier(c);
    auto t = get_current_scope().find<data_type>(name);
    cursor = c;
    return t;
  }

  inline std::shared_ptr<data_type> syntax_parser::expect_data_type(
      token_iterator& cursor) {
    auto t = parse_data_type(cursor);
    if(t == nullptr) {
      throw syntax_error(syntax_error_code::DATA_TYPE_EXPECTED,
        cursor.get_location());
    }
    return t;
  }

  inline std::unique_ptr<syntax_node> syntax_parser::parse_node(
      token_iterator& cursor) {
    if(cursor.is_empty()) {
      return nullptr;
    }
    if(auto node = parse_statement(cursor)) {
      if(!cursor.is_empty() && match(*cursor, terminal::type::new_line)) {
        ++cursor;
      }
      return node;
    } else if(auto node = parse_terminal_node(cursor)) {
      return node;
    }
    return nullptr;
  }
}

#include "syntax_parser_expressions.hpp"
#include "syntax_parser_statements.hpp"
#endif
