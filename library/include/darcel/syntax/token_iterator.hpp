#ifndef DARCEL_TOKEN_ITERATOR_HPP
#define DARCEL_TOKEN_ITERATOR_HPP
#include <cassert>
#include "darcel/syntax/syntax.hpp"
#include "darcel/lexicon/token.hpp"

namespace darcel {

  //! An iterator over tokens used to parse syntax nodes.
  class token_iterator {
    public:

      //! The type of data to parse.
      using value_type = Token;

      //! Constructs an empty token iterator.
      token_iterator();

      //! Constructs a token iterator from a raw pointer.
      /*!
        \param first A pointer to the first object to parse.
        \param size_remaining The number of objects left to parse.
      */
      token_iterator(const Token* first, std::size_t size_remaining);

      //! Constructs a token iterator from a raw pointer.
      /*!
        \param first A pointer to the first object to parse.
        \param size_remaining The number of objects left to parse.
        \param l The location of the first character being parsed.
      */
      token_iterator(const Token* first, std::size_t size_remaining,
        const Location& l);

      //! Returns <code>true</code> iff the size remaining is 0.
      bool is_empty() const;

      //! Returns the number of objects left to parse.
      std::size_t get_size_remaining() const;

      //! Returns the current location.
      Location get_location() const;

      //! Adjusts the pointer.
      /*!
        \param t The updated location to point to.
        \param s The size remaining.
      */
      void adjust(const Token* c, std::size_t s);

      //! Increments this iterator.
      token_iterator& operator ++();

      //! Increments this iterator.
      token_iterator operator ++(int);

      //! Adds an integer to this iterator.
      /*!
        \param rhs The integer to add to the iterator.
        \return An iterator advanced by <i>rhs</i>.
      */
      token_iterator operator +(std::size_t rhs) const;

      //! Advances this iterator.
      /*!
        \param rhs The number of objects to advance by.
        \return <code>*this</code>
      */
      token_iterator& operator +=(std::size_t rhs);

      //! Returns the current object.
      const Token& operator *() const;

      //! Returns the current object.
      const Token* operator ->() const;

    private:
      const Token* m_position;
      std::size_t m_size_remaining;
      std::string m_path;
  };

  //! Returns the difference between two token iterators.
  /*!
    \param lhs The left hand side of the operation.
    \param rhs The right hand side of the operations.
    \return The difference in number of tokens remaining to parse between
            the two iterators.
  */
  inline std::ptrdiff_t operator -(const token_iterator& lhs,
      const token_iterator& rhs) {
    return static_cast<std::ptrdiff_t>(rhs.get_size_remaining()) -
      lhs.get_size_remaining();
  }

  //! Tests if two token iterators point to the same location and have the
  //! same amount of data remaining to parse.
  /*!
    \param lhs The left hand side of the comparison.
    \param rhs The right hand side of the comparison.
    \return <code>true</code> iff both iterators point to the same object in the
            same location and have the same size remaining to parse.
  */
  inline bool operator ==(const token_iterator& lhs,
      const token_iterator& rhs) {
    return lhs.get_size_remaining() == rhs.get_size_remaining() &&
      &*lhs == &*rhs;
  }

  //! Tests if two token iterators are distinct.
  /*!
    \param lhs The left hand side of the comparison.
    \param rhs The right hand side of the comparison.
    \return <code>true</code> iff both iterators point to different objects or
            have different sizes remaining to parse.
  */
  inline bool operator !=(const token_iterator& lhs,
      const token_iterator& rhs) {
    return !(lhs == rhs);
  }

  inline token_iterator::token_iterator()
      : token_iterator(nullptr, 0) {}

  inline token_iterator::token_iterator(const Token* first,
      std::size_t size_remaining)
      : token_iterator(first, size_remaining, Location({}, 0, 0)) {}

  inline token_iterator::token_iterator(const Token* first,
      std::size_t size_remaining, const Location& l)
      : m_position(first),
        m_size_remaining(size_remaining),
        m_path(l.get_path()) {}

  inline bool token_iterator::is_empty() const {
    return m_size_remaining == 0;
  }

  inline std::size_t token_iterator::get_size_remaining() const {
    return m_size_remaining;
  }

  inline Location token_iterator::get_location() const {
    Location l(m_path, m_position->get_line_number(),
      m_position->get_column_number());
    return l;
  }

  inline void token_iterator::adjust(const Token* c, std::size_t s) {
    m_position = c;
    m_size_remaining = s;
  }

  inline token_iterator& token_iterator::operator ++() {
    ++m_position;
    --m_size_remaining;
    return *this;
  }

  inline token_iterator token_iterator::operator ++(int) {
    auto c = *this;
    ++*this;
    return c;
  }

  inline token_iterator token_iterator::operator +(std::size_t rhs) const {
    auto i = *this;
    i += rhs;
    return i;
  }

  inline token_iterator& token_iterator::operator +=(std::size_t rhs) {
    assert(m_size_remaining >= rhs);
    m_position += rhs;
    m_size_remaining -= rhs;
    return *this;
  }

  inline const Token& token_iterator::operator *() const {
    return *m_position;
  }

  inline const Token* token_iterator::operator ->() const {
    return m_position;
  }
}

#endif
