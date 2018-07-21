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
  class Operation {
    public:

      //! The list of operation symbols.
      enum class Symbol {

        //! '+'
        PLUS,

        //! '-'
        MINUS,

        //! '*'
        TIMES,

        //! '/'
        DIVIDE,

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
        NOT,

        //! '='
        ASSIGN
      };

      //! Constructs an operation.
      /*!
        \param symbol The symbol representing the operation.
      */
      Operation(Symbol symbol);

      //! Returns the symbol represented.
      Symbol get_symbol() const;

    private:
      Symbol m_symbol;
  };

  //! Returns <code>true</code> iff a character is a symbolic delimiter that
  //! marks the beginning of an operation.
  /*!
    \param c The character to test.
  */
  inline bool is_operation_delimiter(char c) {
    return c == '+' || c == '-' || c == '*' || c == '-' || c == '<' ||
      c == '>' || c == '=';
  }

  //! Parses an operation.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The operation that was parsed.
  */
  inline std::optional<Operation> parse_operation(LexicalIterator& cursor) {
    if(prefix_match(cursor, "and")) {
      return Operation::Symbol::AND;
    }
    if(prefix_match(cursor, "or")) {
      return Operation::Symbol::OR;
    }
    if(prefix_match(cursor, "not")) {
      return Operation::Symbol::NOT;
    }
    if(cursor.get_size_remaining() >= 2) {
      auto symbol =
        [&] {
          if(std::equal(&*cursor, &*cursor + 2, "<=")) {
            return Operation::Symbol::LESS_OR_EQUAL;
          }
          if(std::equal(&*cursor, &*cursor + 2, "==")) {
            return Operation::Symbol::EQUAL;
          }
          if(std::equal(&*cursor, &*cursor + 2, ">=")) {
            return Operation::Symbol::GREATER_OR_EQUAL;
          }
          return static_cast<Operation::Symbol>(-1);
        }();
      if(symbol != static_cast<Operation::Symbol>(-1)) {
        cursor += 2;
        return symbol;
      }
    }
    if(cursor.get_size_remaining() > 1) {
      auto symbol =
        [&] {
          if(*cursor == '+') {
            return Operation::Symbol::PLUS;
          }
          if(*cursor == '-') {
            return Operation::Symbol::MINUS;
          }
          if(*cursor == '*') {
            return Operation::Symbol::TIMES;
          }
          if(*cursor == '/') {
            return Operation::Symbol::DIVIDE;
          }
          if(*cursor == '<') {
            return Operation::Symbol::LESS;
          }
          if(*cursor == '>') {
            return Operation::Symbol::GREATER;
          }
          if(*cursor == '=') {
            return Operation::Symbol::ASSIGN;
          }
          return static_cast<Operation::Symbol>(-1);
        }();
      if(symbol != static_cast<Operation::Symbol>(-1)) {
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
      LexicalIterator(source.data(), source.size() + 1));
  }

  inline std::ostream& operator <<(std::ostream& out, const Operation& value) {
    switch(value.get_symbol()) {
      case Operation::Symbol::PLUS:
        return out << '+';
      case Operation::Symbol::MINUS:
        return out << '-';
      case Operation::Symbol::TIMES:
        return out << '*';
      case Operation::Symbol::DIVIDE:
        return out << '/';
      case Operation::Symbol::LESS:
        return out << '<';
      case Operation::Symbol::LESS_OR_EQUAL:
        return out << "<=";
      case Operation::Symbol::EQUAL:
        return out << "==";
      case Operation::Symbol::GREATER_OR_EQUAL:
        return out << ">=";
      case Operation::Symbol::GREATER:
        return out << '>';
      case Operation::Symbol::AND:
        return out << "and";
      case Operation::Symbol::OR:
        return out << "or";
      case Operation::Symbol::NOT:
        return out << "not";
      case Operation::Symbol::ASSIGN:
        return out << "=";
      default:
        throw std::runtime_error("Invalid operation.");
    }
  }

  inline bool operator ==(const Operation& lhs, const Operation& rhs) {
    return lhs.get_symbol() == rhs.get_symbol();
  }

  inline bool operator !=(const Operation& lhs, const Operation& rhs) {
    return !(lhs == rhs);
  }

  inline Operation::Operation(Symbol symbol)
      : m_symbol(symbol) {}

  inline Operation::Symbol Operation::get_symbol() const {
    return m_symbol;
  }
}

#endif
