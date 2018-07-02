#ifndef DARCEL_ENUM_EXPRESSION_HPP
#define DARCEL_ENUM_EXPRESSION_HPP
#include <utility>
#include "darcel/data_types/enum_data_type.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! An expression that evaluates to an enum symbol.
  class enum_expression final : public expression {
    public:

      //! Constructs an enum expression.
      /*!
        \param l The location of the token representing the enum value.
        \param e The enum data type being accessed.
        \param index The index of the symbol being accessed.
      */
      enum_expression(location l, std::shared_ptr<enum_data_type> e, int index);

      //! Returns the enum data type being accessed.
      std::shared_ptr<enum_data_type> get_enum() const;

      //! Returns the index of the symbol being accessed.
      int get_index() const;

      const std::shared_ptr<data_type>& get_data_type() const override;

      void apply(syntax_node_visitor& visitor) const override;

    private:
      std::shared_ptr<data_type> m_enum;
      int m_index;
  };

  inline enum_expression::enum_expression(location l,
      std::shared_ptr<enum_data_type> e, int index)
      : expression(std::move(l)),
        m_enum(std::move(e)),
        m_index(index) {}

  inline std::shared_ptr<enum_data_type> enum_expression::get_enum() const {
    return std::static_pointer_cast<enum_data_type>(m_enum);
  }

  inline const std::shared_ptr<data_type>& enum_expression::
      get_data_type() const {
    return m_enum;
  }

  inline int enum_expression::get_index() const {
    return m_index;
  }

  inline void enum_expression::apply(syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(const enum_expression& node) {
    visit(static_cast<const expression&>(node));
  }
}

#endif
