#ifndef DARCEL_DATA_TYPE_COMPATIBILITY_HPP
#define DARCEL_DATA_TYPE_COMPATIBILITY_HPP
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor_types.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Lists the ways that a value of one data type can be converted into
  //! another.
  enum class data_type_compatibility {

    //! The types have no compatibility.
    NONE,

    //! The types are equal.
    EQUAL,

    //! The target can be instantiated to take the source.
    GENERIC
  };

  //! Returns the compatibility from one data type to another.
  /*!
    \param source The source to convert from.
    \param target The target to convert the source to.
    \return The type of compatibility from the source to the target.
  */
  inline data_type_compatibility get_compatibility(
      const data_type& source, const data_type& target) {
    struct data_type_compatibility_visitor final : data_type_visitor {
      const data_type* m_source;
      data_type_compatibility m_compatibility;

      data_type_compatibility operator ()(const data_type& source,
          const data_type& target) {
        m_source = &source;
        target.apply(*this);
        return m_compatibility;
      }

      void visit(const data_type& type) override {
        if(*m_source == type) {
          m_compatibility = data_type_compatibility::EQUAL;
        } else {
          m_compatibility = data_type_compatibility::NONE;
        }
      }

      void visit(const generic_data_type& type) override {
        if(*m_source == type) {
          m_compatibility = data_type_compatibility::EQUAL;
        } else if(dynamic_cast<const generic_data_type*>(m_source) == nullptr) {
          m_compatibility = data_type_compatibility::GENERIC;
        } else {
          m_compatibility = data_type_compatibility::NONE;
        }
      }
    };
    return data_type_compatibility_visitor()(source, target);
  }
}

#endif
