#ifndef DARCEL_ENUM_EXPRESSION_HPP
#define DARCEL_ENUM_EXPRESSION_HPP
#include <utility>
#include "darcel/data_types/enum_data_type.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! An expression that evaluates to an enum symbol.
  class EnumExpression final : public Expression {
    public:

      //! Constructs an enum expression.
      /*!
        \param l The location of the token representing the enum value.
        \param e The enum data type being accessed.
        \param index The index of the symbol being accessed.
      */
      EnumExpression(Location l, std::shared_ptr<EnumDataType> e, int index);

      //! Returns the enum data type being accessed.
      const std::shared_ptr<EnumDataType>& get_enum() const;

      //! Returns the index of the symbol being accessed.
      int get_index() const;

      void apply(SyntaxNodeVisitor& visitor) const override;

    private:
      std::shared_ptr<EnumDataType> m_enum;
      int m_index;
  };

  inline EnumExpression::EnumExpression(Location l,
      std::shared_ptr<EnumDataType> e, int index)
      : Expression(std::move(l)),
        m_enum(std::move(e)),
        m_index(index) {}

  inline const std::shared_ptr<EnumDataType>&
      EnumExpression::get_enum() const {
    return m_enum;
  }

  inline int EnumExpression::get_index() const {
    return m_index;
  }

  inline void EnumExpression::apply(SyntaxNodeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline void SyntaxNodeVisitor::visit(const EnumExpression& node) {
    visit(static_cast<const Expression&>(node));
  }
}

#endif
