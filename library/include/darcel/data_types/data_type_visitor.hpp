#ifndef DARCEL_DATA_TYPE_VISITOR_HPP
#define DARCEL_DATA_TYPE_VISITOR_HPP
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Implements the visitor pattern for data types.
  class data_type_visitor {
    public:
      virtual ~data_type_visitor() = default;

      virtual void visit(const bool_data_type& type);

      virtual void visit(const callable_data_type& type);

      virtual void visit(const data_type& type);

      virtual void visit(const enum_data_type& type);

      virtual void visit(const float_data_type& type);

      virtual void visit(const function_data_type& type);

      virtual void visit(const generic_data_type& type);

      virtual void visit(const integer_data_type& type);

      virtual void visit(const text_data_type& type);

    protected:

      //! Constructs a data type visitor.
      data_type_visitor() = default;
  };

  inline void data_type_visitor::visit(const data_type& type) {}
}

#endif
