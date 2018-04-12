#ifndef DARCEL_FUNCTION_EXPRESSION_HPP
#define DARCEL_FUNCTION_EXPRESSION_HPP
#include <memory>
#include <utility>
#include "darcel/data_types/overloaded_data_type.hpp"
#include "darcel/semantics/function.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! An expression that evaluates to an overloaded function.
  class function_expression : public expression {
    public:

      //! Constructs a function expression.
      /*!
        \param l The location of the identifier token.
        \param f The function to evaluate.
      */
      function_expression(location l, std::shared_ptr<function> f);

      //! Returns the function to evaluate.
      const std::shared_ptr<function>& get_function() const;

      const std::shared_ptr<data_type>& get_data_type() const override final;

      void apply(syntax_node_visitor& visitor) const override final;

    private:
      std::shared_ptr<function> m_function;
      std::shared_ptr<data_type> m_data_type;
  };

  inline function_expression::function_expression(location l,
      std::shared_ptr<function> f)
      : expression(std::move(l)),
        m_function(std::move(f)) {
    auto t = std::make_shared<overloaded_data_type>(get_location(),
      m_function->get_name());
    for(auto& overload : m_function->get_overloads()) {
      t->add(overload->get_data_type());
    }
    m_data_type = std::move(t);
  }

  inline const std::shared_ptr<function>& function_expression::
      get_function() const {
    return m_function;
  }

  inline const std::shared_ptr<data_type>&
      function_expression::get_data_type() const {
    return m_data_type;
  }

  inline void function_expression::apply(syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(const function_expression& node) {
    visit(static_cast<const expression&>(node));
  }
}

#endif
