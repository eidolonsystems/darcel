#ifndef DARCEL_BRACKET_HPP
#define DARCEL_BRACKET_HPP
#include <optional>
#include <ostream>
#include <stdexcept>
#include "darcel/lexicon/lexical_iterator.hpp"
#include "darcel/lexicon/lexicon.hpp"

namespace darcel {

  //! Stores a bracket.
  class Bracket {
    public:

      //! The list of brackets.
      enum class Type {

        //! '('
        ROUND_OPEN,

        //! ')'
        ROUND_CLOSE,
      };

      //! Constructs a bracket.
      /*!
        \param type The type of bracket.
      */
      Bracket(Type t);

      //! Returns the type of bracket represented.
      Type get_type() const;

    private:
      Type m_type;
  };

  //! Returns <code>true</code> iff a character represents a bracket.
  /*!
    \param c The character to test.
  */
  inline bool is_bracket(char c) {
    return c == '(' || c == ')';
  }

  //! Returns <code>true</code> iff the bracket is an opening bracket.
  /*!
    \param b The bracket to test.
    \return <code>true</code> iff the bracket is an opening bracket.
  */
  inline bool is_open(Bracket b) {
    return b.get_type() == Bracket::Type::ROUND_OPEN;
  }

  //! Returns <code>true</code> iff the bracket is a closing bracket.
  /*!
    \param b The bracket to test.
    \return <code>true</code> iff the bracket is a closing bracket.
  */
  inline bool is_close(Bracket b) {
    return b.get_type() == Bracket::Type::ROUND_CLOSE;
  }

  //! Given a bracket, returns the opposite bracket.
  /*!
    \param b The bracket.
    \return The opposite bracket.
  */
  inline Bracket get_opposite(Bracket b) {
    switch(b.get_type()) {
      case Bracket::Type::ROUND_OPEN:
        return Bracket::Type::ROUND_CLOSE;
      case Bracket::Type::ROUND_CLOSE:
        return Bracket::Type::ROUND_OPEN;
    }
    throw std::runtime_error("Invalid bracket.");
  }

  //! Parses a bracket.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The bracket that was parsed.
  */
  inline std::optional<Bracket> parse_bracket(LexicalIterator& cursor) {
    if(!cursor.is_empty()) {
      if(*cursor == '(') {
        ++cursor;
        return Bracket::Type::ROUND_OPEN;
      } else if(*cursor == ')') {
        ++cursor;
        return Bracket::Type::ROUND_CLOSE;
      }
    }
    return std::nullopt;
  }

  //! Parses a bracket from a string.
  /*!
    \param source The string to parse.
    \return The bracket that was parsed.
  */
  inline auto parse_bracket(const std::string_view& source) {
    return darcel::parse_bracket(
      LexicalIterator(source.data(), source.size() + 1));
  }

  inline std::ostream& operator <<(std::ostream& out, const Bracket& value) {
    switch(value.get_type()) {
      case Bracket::Type::ROUND_OPEN:
        return out << '(';
      case Bracket::Type::ROUND_CLOSE:
        return out << ')';
      default:
        throw std::runtime_error("Invalid bracket.");
    }
  }

  inline bool operator ==(const Bracket& lhs, const Bracket& rhs) {
    return lhs.get_type() == rhs.get_type();
  }

  inline bool operator !=(const Bracket& lhs, const Bracket& rhs) {
    return !(lhs == rhs);
  }

  inline Bracket::Bracket(Type t)
      : m_type(t) {}

  inline Bracket::Type Bracket::get_type() const {
    return m_type;
  }
}

#endif
