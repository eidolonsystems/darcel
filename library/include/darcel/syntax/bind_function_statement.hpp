#ifndef DARCEL_BIND_FUNCTION_STATEMENT_HPP
#define DARCEL_BIND_FUNCTION_STATEMENT_HPP
#include "darcel/semantics/function.hpp"
#include "darcel/semantics/variable.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Binds a new function to an expression.
  class bind_function_statement : public statement {
    public:

      //! Constructs a bind statement.
      /*!
        \param l The location of the statement.
        \param f The function that the binding belongs to.
        \param o The specific overload to bind.
        \param p The function parameters.
        \param e The expression to bind to the function.
      */
      bind_function_statement(location l, std::shared_ptr<function> f,
        std::shared_ptr<variable> o, std::vector<std::shared_ptr<variable>> p,
        std::unique_ptr<expression> e);

      //! Returns the function (potentially overloaded) that the binding belongs
      //! to.
      const std::shared_ptr<function>& get_function() const;

      //! Returns the specific overload that was bound.
      const std::shared_ptr<variable>& get_overload() const;

      //! Returns the function parameters.
      const std::vector<std::shared_ptr<variable>>& get_parameters() const;

      //! Returns the expression that was bound.
      const expression& get_expression() const;

      void apply(syntax_node_visitor& visitor) const override final;

    private:
      std::shared_ptr<function> m_function;
      std::shared_ptr<variable> m_overload;
      std::vector<std::shared_ptr<variable>> m_parameters;
      std::unique_ptr<expression> m_expression;
  };

  inline bind_function_statement::bind_function_statement(location l,
      std::shared_ptr<function> f, std::shared_ptr<variable> o,
      std::vector<std::shared_ptr<variable>> p, std::unique_ptr<expression> e)
      : statement(std::move(l)),
        m_function(std::move(f)),
        m_overload(std::move(o)),
        m_parameters(std::move(p)),
        m_expression(std::move(e)) {}

  inline const std::shared_ptr<function>&
      bind_function_statement::get_function() const {
    return m_function;
  }

  inline const std::shared_ptr<variable>&
      bind_function_statement::get_overload() const {
    return m_overload;
  }

  inline const std::vector<std::shared_ptr<variable>>&
      bind_function_statement::get_parameters() const {
    return m_parameters;
  }

  inline const expression& bind_function_statement::get_expression() const {
    return *m_expression;
  }

  inline void bind_function_statement::apply(
      syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(
      const bind_function_statement& node) {
    visit(static_cast<const statement&>(node));
  }
}

#endif
