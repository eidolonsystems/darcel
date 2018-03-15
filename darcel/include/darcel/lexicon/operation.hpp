#ifndef DARCEL_OPERATION_HPP
#define DARCEL_OPERATION_HPP
#include <algorithm>
#include <ostream>
#include <stdexcept>
#include "darcel/lexicon/keyword.hpp"
#include "darcel/lexicon/lexical_iterator.hpp"
#include "darcel/lexicon/lexicon.hpp"

namespace darcel {

  //! Stores an operation symbol.
  class operation {
    public:

      //! The list of operation symbols.
      enum class symbol {

        //! '+'
        PLUS,

        //! '-'
        MINUS,

        //! '*'
        TIMES,

        //! '/'
        DIVIDE,

        //! '='
        ASSIGN,

        //! '<'
        LESS,

        //! '<='
        LESS_OR_EQUAL,

        //! '=='
        EQUAL,

        //! '>='
        GREATER_OR_EQUAL,

        //! '>'
        GREATER,

        //! 'and'
        AND,

        //! 'or'
        OR,

        //! 'not'
        NOT
      };

      //! Constructs an operation.
      /*!
        \param symbol The symbol representing the operation.
      */
      operation(symbol symbol);

      //! Returns the symbol represented.
      symbol get_symbol() const;

    private:
      symbol m_symbol;
  };

  //! Returns <code>true</code> iff a character is a symbolic delimiter that
  //! marks the beginning of an operation.
  /*!
    \param c The character to test.
  */
  inline bool is_operation_delimiter(char c) {
    return c == '+' || c == '-' || c == '*' || c == '-' || c == '=' ||
      c == '<' || c == '>';
  }

  //! Parses an operation.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The operation that was parsed.
  */
  inline std::optional<operation> parse_operation(lexical_iterator& cursor) {
    if(prefix_match(cursor, "and")) {
      return operation::symbol::AND;
    }
    if(prefix_match(cursor, "or")) {
      return operation::symbol::OR;
    }
    if(prefix_match(cursor, "not")) {
      return operation::symbol::NOT;
    }
    if(cursor.get_size_remaining() >= 2) {
      auto symbol =
        [&] {
          if(std::equal(&*cursor, &*cursor + 2, "<=")) {
            return operation::symbol::LESS_OR_EQUAL;
          }
          if(std::equal(&*cursor, &*cursor + 2, "==")) {
            return operation::symbol::EQUAL;
          }
          if(std::equal(&*cursor, &*cursor + 2, ">=")) {
            return operation::symbol::GREATER_OR_EQUAL;
          }
          return static_cast<operation::symbol>(-1);
        }();
      if(symbol != static_cast<operation::symbol>(-1)) {
        cursor += 2;
        return symbol;
      }
    }
    if(cursor.get_size_remaining() > 1) {
      auto symbol =
        [&] {
          if(*cursor == '+') {
            return operation::symbol::PLUS;
          }
          if(*cursor == '-') {
            return operation::symbol::MINUS;
          }
          if(*cursor == '*') {
            return operation::symbol::TIMES;
          }
          if(*cursor == '/') {
            return operation::symbol::DIVIDE;
          }
          if(*cursor == '=') {
            return operation::symbol::ASSIGN;
          }
          if(*cursor == '<') {
            return operation::symbol::LESS;
          }
          if(*cursor == '>') {
            return operation::symbol::GREATER;
          }
          return static_cast<operation::symbol>(-1);
        }();
      if(symbol != static_cast<operation::symbol>(-1)) {
        ++cursor;
        return symbol;
      }
    }
    return std::nullopt;
  }

  //! Parses an operation from a string.
  /*!
    \param source The string to parse.
    \return The operation that was parsed.
  */
  inline auto parse_operation(const std::string_view& source) {
    return darcel::parse_operation(
      lexical_iterator(source.data(), source.size() + 1));
  }

  //! Tests if an operation represents assignment (=, +=, etc...).
  /*!
    \param operation The operation to test.
    \return <code>true</code> iff the operation is an assignment.
  */
  inline bool is_assignment(operation o) {
    return o.get_symbol() == operation::symbol::ASSIGN;
  }

  inline std::ostream& operator <<(std::ostream& out, const operation& value) {
    switch(value.get_symbol()) {
      case operation::symbol::PLUS:
        return out << '+';
      case operation::symbol::MINUS:
        return out << '-';
      case operation::symbol::TIMES:
        return out << '*';
      case operation::symbol::DIVIDE:
        return out << '/';
      case operation::symbol::ASSIGN:
        return out << '=';
      case operation::symbol::LESS:
        return out << '<';
      case operation::symbol::LESS_OR_EQUAL:
        return out << "<=";
      case operation::symbol::EQUAL:
        return out << "==";
      case operation::symbol::GREATER_OR_EQUAL:
        return out << ">=";
      case operation::symbol::GREATER:
        return out << '>';
      case operation::symbol::AND:
        return out << "and";
      case operation::symbol::OR:
        return out << "or";
      case operation::symbol::NOT:
        return out << "not";
      default:
        throw std::runtime_error("Invalid operation.");
    }
  }

  inline bool operator ==(const operation& lhs, const operation& rhs) {
    return lhs.get_symbol() == rhs.get_symbol();
  }

  inline bool operator !=(const operation& lhs, const operation& rhs) {
    return !(lhs == rhs);
  }

  inline operation::operation(symbol symbol)
      : m_symbol(symbol) {}

  inline operation::symbol operation::get_symbol() const {
    return m_symbol;
  }
}

#endif
