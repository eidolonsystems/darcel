#ifndef DARCEL_EXPRESSION_HPP
#define DARCEL_EXPRESSION_HPP
#include "darcel/data_types/data_type.hpp"
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! The base class for a syntax node representing an expression.
  class expression : public statement {
    public:
      using statement::statement;

      //! Returns the data type this expression evaluates to.
      virtual const std::shared_ptr<data_type>& get_data_type() const = 0;
  };

  inline void syntax_node_visitor::visit(const expression& node) {
    visit(static_cast<const statement&>(node));
  }
}

#endif
