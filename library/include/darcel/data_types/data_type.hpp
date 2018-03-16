#ifndef DARCEL_DATA_TYPE_HPP
#define DARCEL_DATA_TYPE_HPP
#include <ostream>
#include "darcel/data_types/data_types.hpp"
#include "darcel/semantics/element.hpp"

namespace darcel {

  //! The base class for a data type.
  class data_type : public element {
    public:
      using element::element;

      //! Tests if two data types are identical.
      /*!
        \param rhs The right hand side to test.
        \return <code>true</code> iff the two data types are identical.
      */
      bool operator ==(const data_type& rhs) const;

      //! Tests if two data types are not identical.
      /*!
        \param rhs The right hand side to test.
        \return <code>true</code> iff the two data types are not identical.
      */
      bool operator !=(const data_type& rhs) const;

    protected:

      //! Tests if two data types are identical.
      /*!
        \param rhs The right hand side to test, guaranteed to not be the same
                   object as <i>this</i> and has the same type as <i>this</i>.
        \return <code>true</code> iff the two data types are identical.
      */
      virtual bool is_equal(const data_type& rhs) const = 0;
  };

  inline std::ostream& operator <<(std::ostream& out, const data_type& value) {
    return out << value.get_name();
  }

  inline bool data_type::operator ==(const data_type& rhs) const {
    if(this == &rhs) {
      return true;
    } else if(typeid(*this) != typeid(rhs)) {
      return false;
    }
    return is_equal(rhs);
  }

  inline bool data_type::operator !=(const data_type& rhs) const {
    return !(*this == rhs);
  }
}

#endif
