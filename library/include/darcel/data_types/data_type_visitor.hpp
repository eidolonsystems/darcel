#ifndef DARCEL_DATA_TYPE_VISITOR_HPP
#define DARCEL_DATA_TYPE_VISITOR_HPP
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Implements the visitor pattern for data types.
  class DataTypeVisitor {
    public:
      virtual ~DataTypeVisitor() = default;

      virtual void visit(const BoolDataType& type);

      virtual void visit(const CallableDataType& type);

      virtual void visit(const DataType& type);

      virtual void visit(const EnumDataType& type);

      virtual void visit(const FloatDataType& type);

      virtual void visit(const FunctionDataType& type);

      virtual void visit(const GenericDataType& type);

      virtual void visit(const IntegerDataType& type);

      virtual void visit(const TextDataType& type);

    protected:

      //! Constructs a data type visitor.
      DataTypeVisitor() = default;
  };

  inline void DataTypeVisitor::visit(const DataType& type) {}
}

#endif
