#ifndef DARCEL_PUNCTUATION_HPP
#define DARCEL_PUNCTUATION_HPP
#include <optional>
#include <ostream>
#include <stdexcept>
#include "darcel/lexicon/lexical_iterator.hpp"
#include "darcel/lexicon/lexicon.hpp"

namespace darcel {

  //! Stores a punctuation mark.
  class Punctuation {
    public:

      //! The list of punctuation marks.
      enum class Mark {

        //! ':'
        COLON,

        //! '::'
        PATH,

        //! ','
        COMMA,

        //! '.'
        DOT,

        //! '|'
        BAR,

        //! '`'
        BACKTICK,

        //! '->'
        ARROW
      };

      //! Constructs a punctuation.
      /*!
        \param mark The mark to represent.
      */
      Punctuation(Mark mark);

      //! Returns the punctuation mark represented.
      Mark get_mark() const;

    private:
      Mark m_mark;
  };

  //! Returns <code>true</code> iff a character represents a punctuation.
  /*!
    \param c The character to test.
  */
  inline bool is_punctuation(char c) {
    return c == ':' || c == ',' || c == '.' || c == '|' || c == '`' || c == '-';
  }

  //! Parses a punctuation.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The punctuation that was parsed.
  */
  inline std::optional<Punctuation> parse_punctuation(
      LexicalIterator& cursor) {
    if(!cursor.is_empty()) {
      if(*cursor == ':') {
        auto c = cursor + 1;
        if(c.is_empty()) {
          return std::nullopt;
        } else if(*c == ':') {
          ++c;
          cursor = c;
          return Punctuation::Mark::PATH;
        }
        cursor = c;
        return Punctuation::Mark::COLON;
      } else if(*cursor == ',') {
        ++cursor;
        return Punctuation::Mark::COMMA;
      } else if(*cursor == '.') {
        ++cursor;
        return Punctuation::Mark::DOT;
      } else if(*cursor == '|') {
        ++cursor;
        return Punctuation::Mark::BAR;
      } else if(*cursor == '`') {
        ++cursor;
        return Punctuation::Mark::BACKTICK;
      } else if(*cursor == '-') {
        auto c = cursor + 1;
        if(c.is_empty()) {
          return std::nullopt;
        } else if(*c == '>') {
          ++c;
          cursor = c;
          return Punctuation::Mark::ARROW;
        }
      }
    }
    return std::nullopt;
  }

  //! Parses a punctuation from a string.
  /*!
    \param source The string to parse.
    \return The punctuation that was parsed.
  */
  inline auto parse_punctuation(const std::string_view& source) {
    return darcel::parse_punctuation(
      LexicalIterator(source.data(), source.size() + 1));
  }

  inline std::ostream& operator <<(std::ostream& out,
      const Punctuation& value) {
    switch(value.get_mark()) {
      case Punctuation::Mark::COLON:
        return out << ':';
      case Punctuation::Mark::PATH:
        return out << "::";
      case Punctuation::Mark::COMMA:
        return out << ',';
      case Punctuation::Mark::DOT:
        return out << '.';
      case Punctuation::Mark::BAR:
        return out << '|';
      case Punctuation::Mark::BACKTICK:
        return out << '`';
      case Punctuation::Mark::ARROW:
        return out << "->";
      default:
        throw std::runtime_error("Invalid punctuation mark.");
    }
  }

  inline bool operator ==(const Punctuation& lhs, const Punctuation& rhs) {
    return lhs.get_mark() == rhs.get_mark();
  }

  inline bool operator !=(const Punctuation& lhs, const Punctuation& rhs) {
    return !(lhs == rhs);
  }

  inline Punctuation::Punctuation(Mark mark)
      : m_mark(mark) {}

  inline Punctuation::Mark Punctuation::get_mark() const {
    return m_mark;
  }
}

#endif
