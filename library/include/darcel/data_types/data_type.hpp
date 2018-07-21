#ifndef DARCEL_DATA_TYPE_HPP
#define DARCEL_DATA_TYPE_HPP
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include "darcel/data_types/data_types.hpp"
#include "darcel/semantics/element.hpp"

namespace darcel {

  //! The base class for a data type.
  class DataType : public Element {
    public:
      using Element::Element;

      //! Tests if two data types are identical.
      /*!
        \param rhs The right hand side to test.
        \return <code>true</code> iff the two data types are identical.
      */
      bool operator ==(const DataType& rhs) const;

      //! Tests if two data types are not identical.
      /*!
        \param rhs The right hand side to test.
        \return <code>true</code> iff the two data types are not identical.
      */
      bool operator !=(const DataType& rhs) const;

      //! Applies a data type visitor to this instance.
      /*!
        \param visitor The visitor to apply.
      */
      virtual void apply(DataTypeVisitor& visitor) const = 0;

    protected:

      //! Tests if two data types are identical.
      /*!
        \param rhs The right hand side to test, guaranteed to not be the same
                   object as <i>this</i> and has the same type as <i>this</i>.
        \return <code>true</code> iff the two data types are identical.
      */
      virtual bool is_equal(const DataType& rhs) const = 0;
  };

  template<typename K>
  struct DataTypeHash {
    std::size_t operator ()(const K& key) const noexcept {
      return std::hash<std::string>()(key->get_name());
    }
  };

  template<typename K>
  struct DataTypeEqualTo {
    bool operator()(const K& lhs, const K& rhs) const noexcept {
      return *lhs == *rhs;
    }
  };

  template<typename K, typename V>
  using DataTypeMap = std::unordered_map<K, V, DataTypeHash<K>,
    DataTypeEqualTo<K>>;

  template<typename K>
  using DataTypeSet = std::unordered_set<K, DataTypeHash<K>,
    DataTypeEqualTo<K>>;

  //! Returns a predicate evaluating whether a data type is equal.
  inline auto equal_to(const DataType& lhs) {
    return [&] (const auto& rhs) {
      return lhs == *rhs;
    };
  }

  //! Tests if a data type is a member of a collection.
  /*!
    \param collection The collection to search.
    \param t The type to find.
    \return <code>true</code> iff <i>t</i> is a member of the <i>collection</i>.
  */
  template<typename T>
  inline auto contains(const T& collection, const DataType& t) {
    return std::find_if(collection.begin(), collection.end(), equal_to(t)) !=
      collection.end();
  }

  inline std::ostream& operator <<(std::ostream& out, const DataType& value) {
    return out << value.get_name();
  }

  inline bool DataType::operator ==(const DataType& rhs) const {
    if(this == &rhs) {
      return true;
    } else if(typeid(*this) != typeid(rhs)) {
      return false;
    }
    return is_equal(rhs);
  }

  inline bool DataType::operator !=(const DataType& rhs) const {
    return !(*this == rhs);
  }
}

#endif
