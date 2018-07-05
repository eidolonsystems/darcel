#ifndef DARCEL_BIND_VARIABLE_STATEMENT_HPP
#define DARCEL_BIND_VARIABLE_STATEMENT_HPP
#include "darcel/semantics/variable.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Binds a new variable to an expression.
  class bind_variable_statement final : public statement {
    public:

      //! Constructs a bind statement.
      /*!
        \param l The location of the statement.
        \param s The scope containing the statement.
        \param v The variable to bind.
        \param e The expression to bind to the variable.
      */
      bind_variable_statement(location l, const scope& s,
        std::shared_ptr<variable> v, std::unique_ptr<expression> e);

      //! Returns the variable.
      const std::shared_ptr<variable>& get_variable() const;

      //! Returns the expression that was bound.
      const expression& get_expression() const;

      void apply(syntax_node_visitor& visitor) const override;

    private:
      std::shared_ptr<variable> m_variable;
      std::unique_ptr<expression> m_expression;
  };

  inline bind_variable_statement::bind_variable_statement(location l,
      const scope& s, std::shared_ptr<variable> v,
      std::unique_ptr<expression> e)
      : statement(std::move(l), s),
        m_variable(std::move(v)),
        m_expression(std::move(e)) {}

  inline const std::shared_ptr<variable>&
      bind_variable_statement::get_variable() const {
    return m_variable;
  }

  inline const expression& bind_variable_statement::get_expression() const {
    return *m_expression;
  }

  inline void bind_variable_statement::apply(
      syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(
      const bind_variable_statement& node) {
    visit(static_cast<const statement&>(node));
  }
}

#endif
