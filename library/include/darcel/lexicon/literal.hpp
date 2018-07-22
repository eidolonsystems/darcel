#ifndef DARCEL_LITERAL_HPP
#define DARCEL_LITERAL_HPP
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/float_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/data_types/text_data_type.hpp"
#include "darcel/lexicon/bracket.hpp"
#include "darcel/lexicon/keyword.hpp"
#include "darcel/lexicon/lexical_iterator.hpp"
#include "darcel/lexicon/lexicon.hpp"
#include "darcel/lexicon/operation.hpp"
#include "darcel/lexicon/punctuation.hpp"
#include "darcel/syntax/syntax_error.hpp"

namespace darcel {

  //! Stores a literal token.
  class Literal {
    public:

      //! Constructs a literal.
      /*!
        \param value The string representation of the value.
        \param type The literal value's data type.
      */
      Literal(std::string value, std::shared_ptr<DataType> type);

      //! Returns the string representation of the value.
      const std::string& get_value() const;

      //! Returns the literal value's data type.
      const std::shared_ptr<DataType>& get_type() const;

    private:
      std::string m_value;
      std::shared_ptr<DataType> m_type;
  };

  //! Returns the escape character corresponding to a value.
  /*!
    \param c The escape character to translate.
    \return The translated escape character.
  */
  inline std::optional<char> translate_escape_character(char c) {
    if(c == '\\') {
      return '\\';
    } else if(c == 'n') {
      return '\n';
    } else if(c == 'r') {
      return '\r';
    } else if(c == '"') {
      return '"';
    } else if(c == 'b') {
      return '\b';
    } else if(c == 'f') {
      return '\f';
    } else if(c == 't') {
      return '\t';
    }
    return std::nullopt;
  }

  //! Parses a text literal.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The text literal that was parsed.
  */
  inline std::optional<Literal> parse_text_literal(
      LexicalIterator& cursor) {
    if(cursor.is_empty() || *cursor != '"') {
      return std::nullopt;
    }
    auto c = cursor + 1;
    std::string value;
    while(true) {
      if(c.is_empty()) {
        return std::nullopt;
      }
      if(*c == '"') {
        break;
      }
      if(*c == '\\') {
        ++c;
        if(c.is_empty()) {
          return std::nullopt;
        }
        auto escape = translate_escape_character(*c);
        if(escape.has_value()) {
          value += *escape;
        } else {
          throw SyntaxError(SyntaxErrorCode::INVALID_ESCAPE_CHARACTER,
            c.get_location());
        }
      } else {
        value += *c;
      }
      ++c;
    }
    ++c;
    cursor = c;
    return Literal(value, TextDataType::get_instance());
  }

  //! Parses a literal.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The literal that was parsed.
  */
  inline std::optional<Literal> parse_literal(LexicalIterator& cursor) {
    auto is_separator =
      [] (char c) {
        return std::isspace(c) || is_punctuation(c) || is_bracket(c) ||
          is_operation_delimiter(c) || c == '\0';
      };
    auto parse_decimal =
      [] (LexicalIterator& cursor) {
        while(!cursor.is_empty() && std::isdigit(*cursor)) {
          ++cursor;
        }
      };
    if(cursor.is_empty()) {
      return std::nullopt;
    }
    if(prefix_match(cursor, "true")) {
      return Literal("true", BoolDataType::get_instance());
    }
    if(prefix_match(cursor, "false")) {
      return Literal("false", BoolDataType::get_instance());
    }
    if(auto l = parse_text_literal(cursor)) {
      return l;
    }
    auto c = cursor;
    if(std::isdigit(*c)) {
      parse_decimal(c);
      if(c.is_empty()) {
        return std::nullopt;
      }
      if(*c != '.') {
        if(!is_separator(*c)) {
          return std::nullopt;
        }
        auto value = std::string(&*cursor, c - cursor);
        cursor = c;
        return Literal(std::move(value), IntegerDataType::get_instance());
      } else {
        ++c;
        parse_decimal(c);
        if(c.is_empty()) {
          return std::nullopt;
        }
        if(!is_separator(*c)) {
          return std::nullopt;
        }
        auto value = std::string(&*cursor, c - cursor);
        cursor = c;
        return Literal(std::move(value), FloatDataType::get_instance());
      }
    }
    return std::nullopt;
  }

  //! Parses a literal from a string.
  /*!
    \param source The string to parse.
    \return The literal that was parsed.
  */
  inline auto parse_literal(const std::string_view& source) {
    return darcel::parse_literal(
      LexicalIterator(source.data(), source.size() + 1));
  }

  inline std::ostream& operator <<(std::ostream& out, const Literal& value) {
    return out << value.get_value();
  }

  inline bool operator ==(const Literal& lhs, const Literal& rhs) {
    return lhs.get_value() == rhs.get_value() &&
      *lhs.get_type() == *rhs.get_type();
  }

  inline bool operator !=(const Literal& lhs, const Literal& rhs) {
    return !(lhs == rhs);
  }

  inline Literal::Literal(std::string value, std::shared_ptr<DataType> type)
      : m_value(std::move(value)),
        m_type(std::move(type)) {}

  inline const std::string& Literal::get_value() const {
    return m_value;
  }

  inline const std::shared_ptr<DataType>& Literal::get_type() const {
    return m_type;
  }
}

#endif
