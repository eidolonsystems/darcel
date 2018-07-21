#ifndef DARCEL_UNMATCHED_BRACKET_SYNTAX_ERROR_HPP
#define DARCEL_UNMATCHED_BRACKET_SYNTAX_ERROR_HPP
#include "darcel/lexicon/bracket.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"

namespace darcel {

  //! Indicates one side of a bracket is missing the opposite side
  //! (ie. an open bracket missing a closing bracket or vice versa).
  class unmatched_bracket_syntax_error : public syntax_error {
    public:

      //! Constructs an unmatched bracket syntax error.
      /*!
        \param error_location The location of the unmatched bracket.
        \param bracket The bracket at the error location.
      */
      unmatched_bracket_syntax_error(Location error_location,
        Bracket bracket);

      //! Returns the unmatched bracket.
      Bracket get_unmatched_bracket() const;

    private:
      Bracket m_bracket;
  };

  inline unmatched_bracket_syntax_error::unmatched_bracket_syntax_error(
      Location error_location, Bracket bracket)
      : syntax_error(syntax_error_code::UNMATCHED_BRACKET,
          std::move(error_location)),
        m_bracket(std::move(bracket)) {}

  inline Bracket unmatched_bracket_syntax_error::get_unmatched_bracket() const {
    return m_bracket;
  }
}

#endif
