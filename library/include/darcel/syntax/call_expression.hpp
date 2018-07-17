#ifndef DARCEL_CALL_EXPRESSION_HPP
#define DARCEL_CALL_EXPRESSION_HPP
#include <memory>
#include <utility>
#include <vector>
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Represents applying a callable expression to its parameters,
  //! ie. a function call.
  class call_expression final : public expression {
    public:

      //! Constructs a call_expression.
      /*!
        \param l The location of the opening bracket.
        \param callable The expression to call.
        \param arguments The arguments to apply to the <i>callable</i>.
      */
      call_expression(location l, std::unique_ptr<expression> callable,
        std::vector<std::unique_ptr<expression>> arguments);

      //! Returns the callable expression.
      const expression& get_callable() const;

      //! Returns the arguments to apply to the callable.
      const std::vector<std::unique_ptr<expression>>& get_arguments() const;

      void apply(syntax_node_visitor& visitor) const override;

    private:
      std::unique_ptr<expression> m_callable;
      std::vector<std::unique_ptr<expression>> m_arguments;
  };

  inline call_expression::call_expression(location l,
      std::unique_ptr<expression> callable,
      std::vector<std::unique_ptr<expression>> arguments)
      : expression(std::move(l)),
        m_callable(std::move(callable)),
        m_arguments(std::move(arguments)) {}

  inline const expression& call_expression::get_callable() const {
    return *m_callable;
  }

  inline const std::vector<std::unique_ptr<expression>>&
      call_expression::get_arguments() const {
    return m_arguments;
  }

  inline void call_expression::apply(syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(const call_expression& node) {
    visit(static_cast<const expression&>(node));
  }
}

#endif
