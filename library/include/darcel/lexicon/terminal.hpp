#ifndef DARCEL_TERMINAL_HPP
#define DARCEL_TERMINAL_HPP
#include <optional>
#include <ostream>
#include "darcel/lexicon/lexical_iterator.hpp"
#include "darcel/lexicon/lexicon.hpp"

namespace darcel {

  //! A token that represents the end of a statement.
  class terminal {
    public:

      // Enumerates the types of terminal tokens.
      enum class type {

        //! The token represents a new line.
        new_line,

        //! The token marks the end of the file.
        end_of_file
      };

      //! Constructs a terminal.
      /*!
        \param type The type of terminal.
      */
      terminal(type type);

      //! Returns the type of terminal.
      type get_type() const;

    private:
      type m_type;
  };

  //! Parses a terminal token.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The terminal token that was parsed.
  */
  inline std::optional<terminal> parse_terminal(LexicalIterator& cursor) {
    if(!cursor.is_empty()) {
      if(*cursor == '\0') {
        ++cursor;
        return terminal(terminal::type::end_of_file);
      } else if(*cursor == '\n') {
        ++cursor;
        return terminal(terminal::type::new_line);
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

  inline std::ostream& operator <<(std::ostream& out, const terminal& value) {
    if(value.get_type() == terminal::type::new_line) {
      return out << '\n';
    }
    return out;
  }

  inline bool operator ==(const terminal& lhs, const terminal& rhs) {
    return lhs.get_type() == rhs.get_type();
  }

  inline bool operator !=(const terminal& lhs, const terminal& rhs) {
    return !(lhs == rhs);
  }

  inline terminal::terminal(type type)
      : m_type(type) {}

  inline terminal::type terminal::get_type() const {
    return m_type;
  }
}

#endif
