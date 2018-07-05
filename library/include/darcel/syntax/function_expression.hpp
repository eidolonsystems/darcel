#ifndef DARCEL_FUNCTION_EXPRESSION_HPP
#define DARCEL_FUNCTION_EXPRESSION_HPP
#include <memory>
#include <utility>
#include "darcel/semantics/function.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! An expression that evaluates to an overloaded function.
  class function_expression final : public expression {
    public:

      //! Constructs a function expression.
      /*!
        \param l The location of the identifier token.
        \param s The scope containing the expression.
        \param f The function to evaluate.
      */
      function_expression(location l, const scope& s,
        std::shared_ptr<function> f);

      //! Returns the function to evaluate.
      const std::shared_ptr<function>& get_function() const;

      void apply(syntax_node_visitor& visitor) const override;

    private:
      std::shared_ptr<function> m_function;
  };

  inline function_expression::function_expression(location l, const scope& s,
      std::shared_ptr<function> f)
      : expression(std::move(l), s),
        m_function(std::move(f)) {}

  inline const std::shared_ptr<function>& function_expression::
      get_function() const {
    return m_function;
  }

  inline void function_expression::apply(syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(const function_expression& node) {
    visit(static_cast<const expression&>(node));
  }
}

#endif
