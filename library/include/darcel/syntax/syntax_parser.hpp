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
  class SyntaxParser {
    public:

      //! Constructs a syntax parser.
      SyntaxParser();

      //! Constructs a syntax parser.
      /*!
        \param s The top-level scope.
      */
      SyntaxParser(const Scope& s);

      //! Feeds this parser a token.
      /*!
        \param t The token to feed.
      */
      void feed(Token t);

      //! Returns an iterator to the next terminal token.
      TokenIterator get_next_terminal() const;

      //! Parses the next syntax node.
      /*!
        \return The syntax node parsed from the previously fed tokens or
                <code>nullptr</code> iff no syntax node is available.
      */
      std::unique_ptr<SyntaxNode> parse_node();

    private:
      std::deque<std::unique_ptr<Scope>> m_scopes;
      std::vector<Token> m_tokens;
      TokenIterator m_cursor;
      int m_generic_index;

      SyntaxParser(const SyntaxParser&) = delete;
      SyntaxParser& operator =(const SyntaxParser&) = delete;
      Scope& get_current_scope();
      Scope& push_scope();
      void pop_scope();
      TokenIterator get_next_terminal(TokenIterator cursor) const;
      std::shared_ptr<FunctionDataType> parse_function_data_type(
        TokenIterator& cursor);
      std::shared_ptr<GenericDataType> parse_generic_data_type(
        TokenIterator& cursor);
      std::shared_ptr<DataType> parse_data_type(TokenIterator& cursor);
      std::shared_ptr<DataType> expect_data_type(TokenIterator& cursor);
      std::unique_ptr<SyntaxNode> parse_node(TokenIterator& cursor);
      std::unique_ptr<TerminalNode> parse_terminal_node(
        TokenIterator& cursor);
      std::unique_ptr<Statement> parse_statement(TokenIterator& cursor);
      std::unique_ptr<Statement> expect_statement(TokenIterator& cursor);
      std::unique_ptr<BindEnumStatement> parse_bind_enum_statement(
        TokenIterator& cursor);
      std::unique_ptr<BindFunctionStatement> parse_bind_function_statement(
        TokenIterator& cursor);
      std::unique_ptr<BindVariableStatement> parse_bind_variable_statement(
        TokenIterator& cursor);
      std::unique_ptr<EnumExpression> parse_enum_expression(
        TokenIterator& cursor);
      std::unique_ptr<FunctionExpression> parse_function_expression(
        TokenIterator& cursor);
      std::unique_ptr<LiteralExpression> parse_literal_expression(
        TokenIterator& cursor);
      std::unique_ptr<VariableExpression> parse_variable_expression(
        TokenIterator& cursor);
      std::unique_ptr<Expression> parse_expression_term(TokenIterator& cursor);
      std::unique_ptr<Expression> parse_expression(TokenIterator& cursor);
      std::unique_ptr<Expression> expect_expression(TokenIterator& cursor);
  };

  //! Parses an identifier from a token stream.
  /*!
    \param cursor An iterator to the first token to parse.
    \return The symbol represented by the parsed identifier.
  */
  inline const std::string& parse_identifier(TokenIterator& cursor) {
    if(cursor.is_empty()) {
      throw SyntaxError(SyntaxErrorCode::IDENTIFIER_EXPECTED,
        cursor.get_location());
    }
    return std::visit(
      [&] (auto&& value) -> const std::string& {
        using T = std::decay_t<decltype(value)>;
        if constexpr(std::is_same_v<T, Identifier>) {
          ++cursor;
          return value.get_symbol();
        }
        throw SyntaxError(SyntaxErrorCode::IDENTIFIER_EXPECTED,
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
      TokenIterator& cursor) {
    try {
      return parse_identifier(cursor);
    } catch(const SyntaxError&) {
      return std::nullopt;
    }
  }

  //! Tests if a token represents the end of a syntax node.
  /*!
    \param t The token to test.
    \return <code>true</code> iff <i>t</i> ends a syntax node.
  */
  inline bool is_syntax_node_end(const Token& t) {
    return is_terminal(t);
  }

  //! Ensures that the token represented by an iterator is equal to some other
  //! token, throwing a SyntaxError otherwise.
  /*!
    \param cursor The iterator to test, this iterator is advanced past the
           the location where the expected token is located.
    \param t The token to expect.
  */
  inline void expect(TokenIterator& cursor, const Token::Instance& t) {
    auto c = cursor;
    while(!c.is_empty() && match(*c, Terminal::Type::NEW_LINE)) {
      ++c;
    }
    if(c.is_empty() || c->get_instance() != t) {
      std::visit(
        [&] (auto&& instance) {
          using T = std::decay_t<decltype(instance)>;
          if constexpr(std::is_same_v<T, Punctuation>) {
            if(instance == Punctuation::Mark::COMMA) {
              throw SyntaxError(SyntaxErrorCode::COMMA_EXPECTED,
                cursor.get_location());
            }
          } else if constexpr(std::is_same_v<T, Bracket>) {
            if(instance == Bracket::Type::ROUND_OPEN) {
              throw SyntaxError(SyntaxErrorCode::OPEN_ROUND_BRACKET_EXPECTED,
                cursor.get_location());
            }
          } else if constexpr(std::is_same_v<T, Operation>) {
            if(instance == Operation::Symbol::ASSIGN) {
              throw SyntaxError(SyntaxErrorCode::ASSIGNMENT_EXPECTED,
                cursor.get_location());
            }
          }
        }, t);
    }
    ++c;
    cursor = c;
  }

  inline SyntaxParser::SyntaxParser()
      : m_generic_index(0) {
    m_scopes.push_back(std::make_unique<Scope>());
  }

  inline SyntaxParser::SyntaxParser(const Scope& s)
      : m_generic_index(0) {
    m_scopes.push_back(std::make_unique<Scope>(&s));
  }

  inline void SyntaxParser::feed(Token t) {
    auto position = &*m_cursor - m_tokens.data();
    m_tokens.push_back(std::move(t));
    m_cursor.adjust(m_tokens.data() + position,
      m_cursor.get_size_remaining() + 1);
  }

  inline TokenIterator SyntaxParser::get_next_terminal() const {
    return get_next_terminal(m_cursor);
  }

  inline std::unique_ptr<SyntaxNode> SyntaxParser::parse_node() {
    return parse_node(m_cursor);
  }

  inline Scope& SyntaxParser::get_current_scope() {
    return *m_scopes.back();
  }

  inline Scope& SyntaxParser::push_scope() {
    m_scopes.push_back(std::make_unique<Scope>(&get_current_scope()));
    return get_current_scope();
  }

  inline void SyntaxParser::pop_scope() {
    m_scopes.pop_back();
  }

  inline TokenIterator SyntaxParser::get_next_terminal(
      TokenIterator cursor) const {
    if(cursor.is_empty() || is_terminal(*cursor)) {
      return cursor;
    }
    auto c = cursor;
    auto is_symbol = std::visit(
      [&] (auto&& t) {
        using T = std::decay_t<decltype(t)>;
        if constexpr(std::is_same_v<T, Identifier> ||
            std::is_same_v<T, Keyword> ||
            std::is_same_v<T, Literal> ||
            std::is_same_v<T, Punctuation>) {
          return true;
        }
        return false;
      }, c->get_instance());
    if(is_symbol) {
      ++c;
      return get_next_terminal(c);
    }
    if(std::get_if<Operation>(&c->get_instance())) {
      ++c;
      if(c.is_empty()) {
        return cursor;
      }
      if(match(*c, Terminal::Type::NEW_LINE)) {
        ++c;
      }
      return get_next_terminal(c);
    }
    if(auto open_bracket = std::get_if<Bracket>(&c->get_instance())) {
      if(!is_open(*open_bracket)) {
        return c;
      }
      auto l = c.get_location();
      ++c;
      auto end = get_next_terminal(c);
      if(end.is_empty()) {
        return cursor;
      }
      auto close_bracket = std::get_if<Bracket>(&end->get_instance());
      if(close_bracket == nullptr ||
          get_opposite(*close_bracket) != *open_bracket) {
        throw UnmatchedBracketSyntaxError(l, *open_bracket);
      }
      ++end;
      return get_next_terminal(end);
    }
    return cursor;
  }

  inline std::shared_ptr<FunctionDataType>
      SyntaxParser::parse_function_data_type(TokenIterator& cursor) {
    auto c = cursor;
    expect(c, Bracket::Type::ROUND_OPEN);
    std::vector<FunctionDataType::Parameter> parameters;
    if(!match(*c, Bracket::Type::ROUND_CLOSE)) {
      while(true) {
        auto name_location = c.get_location();
        auto& parameter_name = parse_identifier(c);
        auto existing_parameter = std::find_if(parameters.begin(),
          parameters.end(),
          [&] (auto& p) {
            return p.m_name == parameter_name;
          });
        if(existing_parameter != parameters.end()) {
          throw SyntaxError(
            SyntaxErrorCode::FUNCTION_PARAMETER_ALREADY_DEFINED,
            name_location);
        }
        expect(c, Punctuation::Mark::COLON);
        auto parameter_type = expect_data_type(c);
        parameters.emplace_back(parameter_name, std::move(parameter_type));
        if(match(*c, Bracket::Type::ROUND_CLOSE)) {
          break;
        }
        expect(c, Punctuation::Mark::COMMA);
      }
    }
    ++c;
    expect(c, Punctuation::Mark::ARROW);
    auto return_type = expect_data_type(c);
    cursor = c;
    return std::make_shared<FunctionDataType>(std::move(parameters),
      std::move(return_type));
  }

  inline std::shared_ptr<GenericDataType>
      SyntaxParser::parse_generic_data_type(TokenIterator& cursor) {
    auto c = cursor;
    expect(c, Punctuation::Mark::BACKTICK);
    auto name_location = c.get_location();
    auto name = '`' + parse_identifier(c);
    auto t = get_current_scope().find<GenericDataType>(name);
    if(t == nullptr) {
      auto g = std::make_shared<GenericDataType>(name_location, name,
        m_generic_index);
      ++m_generic_index;
      get_current_scope().add(g);
      cursor = c;
      return g;
    }
    cursor = c;
    return t;
  }

  inline std::shared_ptr<DataType> SyntaxParser::parse_data_type(
      TokenIterator& cursor) {
    if(cursor.is_empty()) {
      return nullptr;
    }
    if(match(*cursor, Bracket::Type::ROUND_OPEN)) {
      return parse_function_data_type(cursor);
    }
    if(match(*cursor, Punctuation::Mark::BACKTICK)) {
      return parse_generic_data_type(cursor);
    }
    auto c = cursor;
    auto name_location = c.get_location();
    auto name = parse_identifier(c);
    auto t = get_current_scope().find<DataType>(name);
    cursor = c;
    return t;
  }

  inline std::shared_ptr<DataType> SyntaxParser::expect_data_type(
      TokenIterator& cursor) {
    auto t = parse_data_type(cursor);
    if(t == nullptr) {
      throw SyntaxError(SyntaxErrorCode::DATA_TYPE_EXPECTED,
        cursor.get_location());
    }
    return t;
  }

  inline std::unique_ptr<SyntaxNode> SyntaxParser::parse_node(
      TokenIterator& cursor) {
    if(cursor.is_empty()) {
      return nullptr;
    }
    if(auto node = parse_statement(cursor)) {
      if(!cursor.is_empty() && match(*cursor, Terminal::Type::NEW_LINE)) {
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
