#ifndef DARCEL_DATA_TYPE_COMPATIBILITY_HPP
#define DARCEL_DATA_TYPE_COMPATIBILITY_HPP
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor_types.hpp"
#include "darcel/data_types/data_types.hpp"
#include "darcel/semantics/scope.hpp"

namespace darcel {

  //! Lists the ways that a value of one data type can be converted into
  //! another.
  enum class DataTypeCompatibility {

    //! The types have no compatibility.
    NONE,

    //! The types are equal.
    EQUAL,

    //! The target can be instantiated to take the source.
    GENERIC,

    //! The target is a subtype of the source.
    SUPER_TYPE
  };

  //! Returns the compatibility from one data type to another.
  /*!
    \param source The source to convert from.
    \param target The target to convert the source to.
    \param s The scope containing the data types.
    \return The type of compatibility from the source to the target.
  */
  inline DataTypeCompatibility get_compatibility(const DataType& source,
      const DataType& target, const Scope& s) {
    struct data_type_compatibility_visitor final : DataTypeVisitor {
      const DataType* m_source;
      const Scope* m_scope;
      DataTypeCompatibility m_compatibility;

      DataTypeCompatibility operator ()(const DataType& source,
          const DataType& target, const Scope& s) {
        m_source = &source;
        m_scope = &s;
        target.apply(*this);
        return m_compatibility;
      }

      void visit(const DataType& type) override {
        if(*m_source == type) {
          m_compatibility = DataTypeCompatibility::EQUAL;
        } else {
          m_compatibility = DataTypeCompatibility::NONE;
        }
      }

      void visit(const FunctionDataType& type) override {
        if(auto callable_type = dynamic_cast<const CallableDataType*>(
            m_source)) {
          auto& definition = m_scope->find(*callable_type->get_function(),
            [&] (auto& definition) {
              return get_compatibility(*definition.get_type(), type,
                *m_scope) == DataTypeCompatibility::EQUAL;
            });
          if(definition != nullptr) {
            m_compatibility = DataTypeCompatibility::SUPER_TYPE;
          } else {
            visit(static_cast<const DataType&>(type));
          }
        } else {
          visit(static_cast<const DataType&>(type));
        }
      }

      void visit(const GenericDataType& type) override {
        if(dynamic_cast<const GenericDataType*>(m_source) == nullptr) {
          m_compatibility = DataTypeCompatibility::GENERIC;
        } else {
          visit(static_cast<const DataType&>(type));
        }
      }
    };
    return data_type_compatibility_visitor()(source, target, s);
  }
}

#endif
