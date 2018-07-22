#ifndef DARCEL_BIND_ENUM_STATEMENT_HPP
#define DARCEL_BIND_ENUM_STATEMENT_HPP
#include "darcel/data_types/enum_data_type.hpp"
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Binds a name to an enum data type.
  class BindEnumStatement final : public Statement {
    public:

      //! Constructs a bind statement.
      /*!
        \param e The enum to bind.
      */
      BindEnumStatement(std::shared_ptr<EnumDataType> e);

      //! Returns the enum.
      const std::shared_ptr<EnumDataType>& get_enum() const;

      void apply(SyntaxNodeVisitor& visitor) const override;

    private:
      std::shared_ptr<EnumDataType> m_enum;
  };

  inline BindEnumStatement::BindEnumStatement(
      std::shared_ptr<EnumDataType> e)
      : Statement(e->get_location()),
        m_enum(std::move(e)) {}

  inline const std::shared_ptr<EnumDataType>&
      BindEnumStatement::get_enum() const {
    return m_enum;
  }

  inline void BindEnumStatement::apply(SyntaxNodeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline void SyntaxNodeVisitor::visit(const BindEnumStatement& node) {
    visit(static_cast<const Statement&>(node));
  }
}

#endif
