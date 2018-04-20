#ifndef DARCEL_LEXICAL_ITERATOR_HPP
#define DARCEL_LEXICAL_ITERATOR_HPP
#include <cassert>
#include <string>
#include "darcel/lexicon/lexicon.hpp"
#include "darcel/lexicon/location.hpp"

namespace darcel {

  //! An iterator over characters used to parse lexical tokens.
  class lexical_iterator {
    public:

      //! The type of data to parse.
      using value_type = char;

      //! Constructs an empty lexical iterator.
      lexical_iterator();

      //! Constructs a lexical iterator from a raw pointer.
      /*!
        \param first A pointer to the first object to parse.
        \param size_remaining The number of objects left to parse.
      */
      lexical_iterator(const char* first, std::size_t size_remaining);

      //! Constructs a lexical iterator from a raw pointer.
      /*!
        \param first A pointer to the first object to parse.
        \param size_remaining The number of objects left to parse.
        \param l The location of the first character being parsed.
      */
      lexical_iterator(const char* first, std::size_t size_remaining,
        const location& l);

      //! Returns <code>true</code> iff the size remaining is 0.
      bool is_empty() const;

      //! Returns the number of objects left to parse.
      std::size_t get_size_remaining() const;

      //! Returns the current location.
      location get_location() const;

      //! Adjusts the pointer.
      /*!
        \param c The updated location to point to.
        \param s The size remaining.
      */
      void adjust(const char* c, std::size_t s);

      //! Increments this iterator.
      lexical_iterator& operator ++();

      //! Increments this iterator.
      lexical_iterator operator ++(int);

      //! Adds an integer to this iterator.
      /*!
        \param rhs The integer to add to the iterator.
        \return An iterator advanced by <i>rhs</i>.
      */
      lexical_iterator operator +(std::size_t rhs) const;

      //! Advances this iterator.
      /*!
        \param rhs The number of objects to advance by.
        \return <code>*this</code>
      */
      lexical_iterator& operator +=(std::size_t rhs);

      //! Returns the current object.
      const char& operator *() const;

    private:
      const char* m_position;
      std::size_t m_size_remaining;
      std::string m_path;
      int m_line_number;
      int m_column_number;
  };

  //! Returns the difference between two lexical iterators.
  /*!
    \param lhs The left hand side of the operation.
    \param rhs The right hand side of the operations.
    \return The difference in number of characters remaining to parse between
            the two iterators.
  */
  inline std::ptrdiff_t operator -(const lexical_iterator& lhs,
      const lexical_iterator& rhs) {
    return static_cast<std::ptrdiff_t>(rhs.get_size_remaining()) -
      lhs.get_size_remaining();
  }

  //! Tests if two lexical iterators point to the same location and have the
  //! same amount of data remaining to parse.
  /*!
    \param lhs The left hand side of the comparison.
    \param rhs The right hand side of the comparison.
    \return <code>true</code> iff both iterators point to the same object in the
            same location and have the same size remaining to parse.
  */
  inline bool operator ==(const lexical_iterator& lhs,
      const lexical_iterator& rhs) {
    return lhs.get_size_remaining() == rhs.get_size_remaining() &&
      &*lhs == &*rhs;
  }

  //! Tests if two lexical iterators are distinct.
  /*!
    \param lhs The left hand side of the comparison.
    \param rhs The right hand side of the comparison.
    \return <code>true</code> iff both iterators point to different objects or
            have different sizes remaining to parse.
  */
  inline bool operator !=(const lexical_iterator& lhs,
      const lexical_iterator& rhs) {
    return !(lhs == rhs);
  }

  inline lexical_iterator::lexical_iterator()
      : lexical_iterator(nullptr, 0) {}

  inline lexical_iterator::lexical_iterator(const char* first,
      std::size_t size_remaining)
      : lexical_iterator(first, size_remaining, location({}, 1, 1)) {}

  inline lexical_iterator::lexical_iterator(const char* first,
      std::size_t size_remaining, const location& l)
      : m_position(first),
        m_size_remaining(size_remaining),
        m_path(l.get_path()),
        m_line_number(l.get_line_number()),
        m_column_number(l.get_column_number()) {}

  inline bool lexical_iterator::is_empty() const {
    return m_size_remaining == 0;
  }

  inline std::size_t lexical_iterator::get_size_remaining() const {
    return m_size_remaining;
  }

  inline location lexical_iterator::get_location() const {
    location l(m_path, m_line_number, m_column_number);
    return l;
  }

  inline void lexical_iterator::adjust(const char* c, std::size_t s) {
    m_position = c;
    m_size_remaining = s;
  }

  inline lexical_iterator& lexical_iterator::operator ++() {
    assert(m_size_remaining != 0);
    if(*m_position == '\n') {
      ++m_line_number;
      m_column_number = 1;
    } else {
      ++m_column_number;
    }
    ++m_position;
    --m_size_remaining;
    return *this;
  }

  inline lexical_iterator lexical_iterator::operator ++(int) {
    auto c = *this;
    ++*this;
    return c;
  }

  inline lexical_iterator lexical_iterator::operator +(std::size_t rhs) const {
    auto i = *this;
    i += rhs;
    return i;
  }

  inline lexical_iterator& lexical_iterator::operator +=(std::size_t rhs) {
    assert(m_size_remaining >= rhs);
    while(rhs != 0) {
      ++(*this);
      --rhs;
    }
    return *this;
  }

  inline const char& lexical_iterator::operator *() const {
    return *m_position;
  }
}

#endif
