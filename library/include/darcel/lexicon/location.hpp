#ifndef DARCEL_LOCATION_HPP
#define DARCEL_LOCATION_HPP
#include <ostream>
#include <string>
#include <utility>
#include "darcel/lexicon/lexicon.hpp"

namespace darcel {

  //! Represents a location in darcel source code.
  class location {
    public:

      //! Represents a location used for globally defined elements.
      static const location& global();

      //! Constructs a location.
      /*!
        \param path The path to the file containing the source code.
        \param line_number The source code's line number.
        \param column_number The source code's column number.
      */
      location(std::string path, int line_number, int column_number);

      //! Returns the path to the file containing the source code.
      const std::string& get_path() const;

      //! Returns the source code's line number.
      int get_line_number() const;

      //! Returns the source code's column number.
      int get_column_number() const;

    private:
      std::string m_path;
      int m_line_number;
      int m_column_number;
  };

  //! Tests if two locations are equal.
  /*!
    \param lhs The left hand side.
    \param rhs The right hand side.
  */
  inline bool operator ==(const location& lhs, const location& rhs) {
    return lhs.get_path() == rhs.get_path() &&
      lhs.get_line_number() == rhs.get_line_number() &&
      lhs.get_column_number() == rhs.get_column_number();
  }

  //! Tests if two paths are distinct.
  /*!
    \param lhs The left hand side.
    \param rhs The right hand side.
  */
  inline bool operator !=(const location& lhs, const location& rhs) {
    return !(lhs == rhs);
  }

  inline std::ostream& operator <<(std::ostream& out, const location& value) {
    return out << value.get_path() << ":" << value.get_line_number() << ":" <<
      value.get_column_number();
  }

  inline const location& location::global() {
    static location value("", 0, 0);
    return value;
  }

  inline location::location(std::string path, int line_number,
      int column_number)
      : m_path(std::move(path)),
        m_line_number(line_number),
        m_column_number(column_number) {}

  inline const std::string& location::get_path() const {
    return m_path;
  }

  inline int location::get_line_number() const {
    return m_line_number;
  }

  inline int location::get_column_number() const {
    return m_column_number;
  }
}

#endif
