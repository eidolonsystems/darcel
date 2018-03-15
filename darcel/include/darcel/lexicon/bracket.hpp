#ifndef DARCEL_BRACKET_HPP
#define DARCEL_BRACKET_HPP
#include <optional>
#include <ostream>
#include <stdexcept>
#include "darcel/lexicon/lexical_iterator.hpp"
#include "darcel/lexicon/lexicon.hpp"

namespace darcel {

  //! Stores a bracket.
  class bracket {
    public:

      //! The list of brackets.
      enum class type {

        //! '('
        OPEN_ROUND_BRACKET,

        //! ')'
        CLOSE_ROUND_BRACKET,
      };

      //! Constructs a bracket.
      /*!
        \param type The type of bracket.
      */
      bracket(type t);

      //! Returns the type of bracket represented.
      type get_type() const;

    private:
      type m_type;
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
  inline bool is_open(bracket b) {
    return b.get_type() == bracket::type::OPEN_ROUND_BRACKET;
  }

  //! Returns <code>true</code> iff the bracket is a closing bracket.
  /*!
    \param b The bracket to test.
    \return <code>true</code> iff the bracket is a closing bracket.
  */
  inline bool is_close(bracket b) {
    return b.get_type() == bracket::type::CLOSE_ROUND_BRACKET;
  }

  //! Given a bracket, returns the opposite bracket.
  /*!
    \param b The bracket.
    \return The opposite bracket.
  */
  inline bracket get_opposite(bracket b) {
    switch(b.get_type()) {
      case bracket::type::OPEN_ROUND_BRACKET:
        return bracket::type::CLOSE_ROUND_BRACKET;
      case bracket::type::CLOSE_ROUND_BRACKET:
        return bracket::type::OPEN_ROUND_BRACKET;
    }
    throw std::runtime_error("Invalid bracket.");
  }

  //! Parses a bracket.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The bracket that was parsed.
  */
  inline std::optional<bracket> parse_bracket(lexical_iterator& cursor) {
    if(!cursor.is_empty()) {
      if(*cursor == '(') {
        ++cursor;
        return bracket::type::OPEN_ROUND_BRACKET;
      } else if(*cursor == ')') {
        ++cursor;
        return bracket::type::CLOSE_ROUND_BRACKET;
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
      lexical_iterator(source.data(), source.size() + 1));
  }

  inline std::ostream& operator <<(std::ostream& out, const bracket& value) {
    switch(value.get_type()) {
      case bracket::type::OPEN_ROUND_BRACKET:
        return out << '(';
      case bracket::type::CLOSE_ROUND_BRACKET:
        return out << ')';
      default:
        throw std::runtime_error("Invalid bracket.");
    }
  }

  inline bool operator ==(const bracket& lhs, const bracket& rhs) {
    return lhs.get_type() == rhs.get_type();
  }

  inline bool operator !=(const bracket& lhs, const bracket& rhs) {
    return !(lhs == rhs);
  }

  inline bracket::bracket(type t)
      : m_type(t) {}

  inline bracket::type bracket::get_type() const {
    return m_type;
  }
}

#endif
