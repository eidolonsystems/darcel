#ifndef DARCEL_TOKEN_HPP
#define DARCEL_TOKEN_HPP
#include <ostream>
#include <utility>
#include <variant>
#include "darcel/lexicon/bracket.hpp"
#include "darcel/lexicon/identifier.hpp"
#include "darcel/lexicon/keyword.hpp"
#include "darcel/lexicon/literal.hpp"
#include "darcel/lexicon/lexicon.hpp"
#include "darcel/lexicon/operation.hpp"
#include "darcel/lexicon/punctuation.hpp"
#include "darcel/lexicon/terminal.hpp"

namespace darcel {

  //! Stores a single lexical token.
  class Token {
    public:

      //! The types of tokens represented.
      enum class Type {

        //! A keyword.
        KEYWORD,

        //! A punctuation mark.
        PUNCTUATION,

        //! A bracket.
        BRACKET,

        //! An operation.
        OPERATION,

        //! A symbolic identifier.
        IDENTIFIER,

        //! A literal value.
        LITERAL,

        //! The end of the module.
        TERMINAL
      };

      //! A variant that stores the token's data.
      using Instance = std::variant<Keyword, Punctuation, Bracket, Operation,
        Identifier, Literal, Terminal>;

      //! Constructs a keyword token.
      /*!
        \param instance The token's data.
        \param line_number The line number.
        \param column_number The column number.
      */
      Token(Instance instance, int line_number, int column_number);

      //! Returns the data associated with this token.
      const Instance& get_instance() const;

      //! Returns the type of token.
      Type get_type() const;

      //! Returns the line number.
      int get_line_number() const;

      //! Returns the column number.
      int get_column_number() const;

    private:
      Instance m_instance;
      int m_line_number;
      int m_column_number;
  };

  //! Tests if a token represents a terminal token.
  /*!
    \param t The token to test.
    \return <code>true</code> iff the token is a terminal symbol.
  */
  inline bool is_terminal(const Token& t) {
    return t.get_type() == Token::Type::TERMINAL;
  }

  //! Tests if a token represents a keyword token.
  /*!
    \param t The token to test.
    \return <code>true</code> iff the token is a keyword.
  */
  inline bool is_keyword(const Token& t) {
    return t.get_type() == Token::Type::KEYWORD;
  }

  //! Tests if a token represents a particular instance.
  /*!
    \param t The token to test.
    \param i The instance to match.
    \return <code>true</code> iff the token matches the specified instance.
  */
  inline bool match(const Token& t, const Token::Instance& i) {
    return t.get_instance() == i;
  }

  inline std::ostream& operator <<(std::ostream& out, const Token& value) {
    return std::visit(
      [&] (auto& value) -> decltype(auto) {
        return out << value;
      },
      value.get_instance());
  }

  inline Token::Token(Instance instance, int line_number, int column_number)
      : m_instance(std::move(instance)),
        m_line_number(line_number),
        m_column_number(column_number) {}

  inline const Token::Instance& Token::get_instance() const {
    return m_instance;
  }

  inline Token::Type Token::get_type() const {
    return static_cast<Type>(m_instance.index());
  }

  inline int Token::get_line_number() const {
    return m_line_number;
  }

  inline int Token::get_column_number() const {
    return m_column_number;
  }
}

#endif
