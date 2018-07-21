#ifndef DARCEL_TOKEN_PARSER_HPP
#define DARCEL_TOKEN_PARSER_HPP
#include <array>
#include <optional>
#include <vector>
#include "darcel/lexicon/lexical_iterator.hpp"
#include "darcel/lexicon/lexicon.hpp"
#include "darcel/lexicon/token.hpp"

namespace darcel {

  //! Parses tokens from text.
  class TokenParser {
    public:

      //! Constructs a token_parser.
      TokenParser();

      //! Provides the parser with additional text.
      /*!
        \param data A pointer to the first character to feed.
        \param size The number of characters to feed.
      */
      void feed(const char* data, std::size_t size);

      //! Provides the parser with additional text.
      /*!
        \param data The text to feed to this parser.
      */
      template<std::size_t N>
      void feed(const char (&data)[N]);

      //! Parses the next token.
      /*!
        \return The token parsed from the previously fed text.
      */
      std::optional<Token> parse_token();

    private:
      std::vector<char> m_data;
      LexicalIterator m_cursor;
      bool m_parsed_new_line;
      bool m_parsed_line_continuation;
      std::array<int, 1> m_bracket_count;

      TokenParser(const TokenParser&) = delete;
      TokenParser& operator =(const TokenParser&) = delete;
      void update_bracket_count(const Bracket& p);
      bool is_new_line_signifcant() const;
  };

  inline TokenParser::TokenParser()
      : m_parsed_new_line(true),
        m_parsed_line_continuation(false) {
    m_bracket_count.fill(0);
  }

  inline void TokenParser::feed(const char* data, std::size_t size) {
    auto position = &*m_cursor - m_data.data();
    m_data.insert(m_data.end(), data, data + size);
    m_cursor.adjust(m_data.data() + position,
      m_cursor.get_size_remaining() + size);
  }

  template<std::size_t N>
  void TokenParser::feed(const char (&data)[N]) {
    feed(data, N - 1);
  }

  inline std::optional<Token> TokenParser::parse_token() {
    while(!m_cursor.is_empty()) {
      if(std::isspace(*m_cursor)) {
        if(*m_cursor == '\n') {
          if(is_new_line_signifcant()) {
            Token t(Terminal::Type::NEW_LINE,
              m_cursor.get_location().get_line_number(),
              m_cursor.get_location().get_column_number());
            ++m_cursor;
            m_parsed_new_line = true;
            return t;
          }
        }
        ++m_cursor;
      } else {
        break;
      }
    }
    if(m_cursor.is_empty()) {
      return std::nullopt;
    }
    auto parsed_line_continuation = m_parsed_line_continuation;
    m_parsed_line_continuation = false;
    m_parsed_new_line = false;
    auto l = m_cursor.get_location();
    if(auto keyword = parse_keyword(m_cursor)) {
      return Token(std::move(*keyword), l.get_line_number(),
        l.get_column_number());
    } else if(auto punctuation = parse_punctuation(m_cursor)) {
      return Token(std::move(*punctuation), l.get_line_number(),
        l.get_column_number());
    } else if(auto bracket = parse_bracket(m_cursor)) {
      update_bracket_count(*bracket);
      return Token(std::move(*bracket), l.get_line_number(),
        l.get_column_number());
    } else if(auto operation = parse_operation(m_cursor)) {
      m_parsed_line_continuation = true;
      return Token(std::move(*operation), l.get_line_number(),
        l.get_column_number());
    } else if(auto literal = parse_literal(m_cursor)) {
      return Token(std::move(*literal), l.get_line_number(),
        l.get_column_number());
    } else if(auto identifier = parse_identifier(m_cursor)) {
      return Token(std::move(*identifier), l.get_line_number(),
        l.get_column_number());
    } else if(auto terminal = parse_terminal(m_cursor)) {
      return Token(std::move(*terminal), l.get_line_number(),
        l.get_column_number());
    }
    m_parsed_line_continuation = parsed_line_continuation;
    return std::nullopt;
  }

  inline void TokenParser::update_bracket_count(const Bracket& b) {
    switch(b.get_type()) {
      case Bracket::Type::ROUND_OPEN:
        ++m_bracket_count[0];
        return;
      case Bracket::Type::ROUND_CLOSE:
        --m_bracket_count[0];
        return;
    }
  }

  inline bool TokenParser::is_new_line_signifcant() const {
    if(m_parsed_line_continuation || m_parsed_new_line) {
      return false;
    }
    for(auto& count : m_bracket_count) {
      if(count != 0) {
        return false;
      }
    }
    return true;
  }
}

#endif
