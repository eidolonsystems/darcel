#ifndef DARCEL_IDENTIFIER_HPP
#define DARCEL_IDENTIFIER_HPP
#include <cctype>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include "darcel/lexicon/lexical_iterator.hpp"
#include "darcel/lexicon/lexicon.hpp"

namespace darcel {

  //! Stores an identifier.
  class identifier {
    public:

      //! Constructs an identifier.
      /*!
        \param symbol The identifier's symbol.
      */
      identifier(std::string symbol);

      //! Returns the symbol.
      const std::string& get_symbol() const;

    private:
      std::shared_ptr<std::string> m_symbol;
  };

  //! Parses an identifier.
  /*!
    \param cursor An iterator to the first character to parse, this iterator
           will be adjusted to one past the last character that was parsed.
    \return The identifier that was parsed.
  */
  inline std::optional<identifier> parse_identifier(lexical_iterator& cursor) {
    if(cursor.is_empty()) {
      return std::nullopt;
    }
    auto c = cursor;
    if(!std::isalpha(*c) && *c != '_') {
      return std::nullopt;
    }
    ++c;
    while(!c.is_empty()) {
      if(!std::isalnum(*c) && *c != '_') {
        std::string identifier(&*cursor, (c - cursor));
        cursor = c;
        return identifier;
      }
      ++c;
    }
    return std::nullopt;
  }

  //! Parses an identifier from a string.
  /*!
    \param source The string to parse.
    \return The identifier that was parsed.
  */
  inline auto parse_identifier(const std::string_view& source) {
    return darcel::parse_identifier(
      lexical_iterator(source.data(), source.size() + 1));
  }

  inline std::ostream& operator <<(std::ostream& out, const identifier& value) {
    return out << value.get_symbol();
  }

  inline bool operator ==(const identifier& lhs, const identifier& rhs) {
    return lhs.get_symbol() == rhs.get_symbol();
  }

  inline bool operator !=(const identifier& lhs, const identifier& rhs) {
    return !(lhs == rhs);
  }

  inline identifier::identifier(std::string symbol)
      : m_symbol(std::make_shared<std::string>(std::move(symbol))) {}

  inline const std::string& identifier::get_symbol() const {
    return *m_symbol;
  }
}

#endif
