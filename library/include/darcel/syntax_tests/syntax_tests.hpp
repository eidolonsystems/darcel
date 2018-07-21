#ifndef DARCEL_SYNTAX_TESTER_HPP
#define DARCEL_SYNTAX_TESTER_HPP
#include "darcel/lexicon/token_parser.hpp"
#include "darcel/syntax/syntax_parser.hpp"

namespace darcel {
namespace tests {
  template<std::size_t N>
  void incremental_feed(syntax_parser& p, const char (&s)[N]) {
    TokenParser t;
    t.feed(s);
    while(auto token = t.parse_token()) {
      if(match(*token, Terminal::Type::END_OF_FILE)) {
        p.feed(std::move(*token));
        break;
      }
      p.feed(std::move(*token));
    }
  }

  template<std::size_t N>
  void feed(syntax_parser& p, const char (&s)[N]) {
    char q[N + 1];
    std::copy(std::begin(s), std::end(s), std::begin(q));
    q[N] = '\0';
    incremental_feed(p, q);
  }
}
}

#endif
