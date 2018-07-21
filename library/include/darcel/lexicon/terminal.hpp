#ifndef DARCEL_TERMINAL_HPP
#define DARCEL_TERMINAL_HPP
#include <optional>
#include <ostream>
#include "darcel/lexicon/lexical_iterator.hpp"
#include "darcel/lexicon/lexicon.hpp"

namespace darcel {

  //! A token that represents the end of a statement.
  class Terminal {
    public:

      // Enumerates the types of terminal tokens.
      enum class Type {

        //! The token represents a new line.
        NEW_LINE,

        //! The token marks the end of the file.
        END_OF_FILE
      };

      //! Constructs a terminal.
      /*!
        \param type The type of terminal.
      */
      Terminal(Type type);

      //! Returns the type of terminal.
      Type get_type() const;

    private:
      Type m_type;
  };

  //! Parses a terminal token.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The terminal token that was parsed.
  */
  inline std::optional<Terminal> parse_terminal(LexicalIterator& cursor) {
    if(!cursor.is_empty()) {
      if(*cursor == '\0') {
        ++cursor;
        return Terminal(Terminal::Type::END_OF_FILE);
      } else if(*cursor == '\n') {
        ++cursor;
        return Terminal(Terminal::Type::NEW_LINE);
      }
    }
    return std::nullopt;
  }

  //! Parses a terminal from a string.
  /*!
    \param source The string to parse.
    \return The terminal that was parsed.
  */
  inline auto parse_terminal(const std::string_view& source) {
    return darcel::parse_terminal(
      LexicalIterator(source.data(), source.size()));
  }

  inline std::ostream& operator <<(std::ostream& out, const Terminal& value) {
    if(value.get_type() == Terminal::Type::NEW_LINE) {
      return out << '\n';
    }
    return out;
  }

  inline bool operator ==(const Terminal& lhs, const Terminal& rhs) {
    return lhs.get_type() == rhs.get_type();
  }

  inline bool operator !=(const Terminal& lhs, const Terminal& rhs) {
    return !(lhs == rhs);
  }

  inline Terminal::Terminal(Type type)
      : m_type(type) {}

  inline Terminal::Type Terminal::get_type() const {
    return m_type;
  }
}

#endif
