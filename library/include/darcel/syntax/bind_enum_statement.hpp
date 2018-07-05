#ifndef DARCEL_BIND_ENUM_STATEMENT_HPP
#define DARCEL_BIND_ENUM_STATEMENT_HPP
#include "darcel/data_types/enum_data_type.hpp"
#include "darcel/syntax/statement.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Binds a name to an enum data type.
  class bind_enum_statement final : public statement {
    public:

      //! Constructs a bind statement.
      /*!
        \param s The scope containing the statement.
        \param e The enum to bind.
      */
      bind_enum_statement(const scope& s, std::shared_ptr<enum_data_type> e);

      //! Returns the enum.
      const std::shared_ptr<enum_data_type>& get_enum() const;

      void apply(syntax_node_visitor& visitor) const override;

    private:
      std::shared_ptr<enum_data_type> m_enum;
  };

  inline bind_enum_statement::bind_enum_statement(const scope& s,
      std::shared_ptr<enum_data_type> e)
      : statement(e->get_location(), s),
        m_enum(std::move(e)) {}

  inline const std::shared_ptr<enum_data_type>&
      bind_enum_statement::get_enum() const {
    return m_enum;
  }

  inline void bind_enum_statement::apply(syntax_node_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline void syntax_node_visitor::visit(const bind_enum_statement& node) {
    visit(static_cast<const statement&>(node));
  }
}

#endif
