#ifndef DARCEL_ENUM_DATA_TYPE_HPP
#define DARCEL_ENUM_DATA_TYPE_HPP
#include <memory>
#include <vector>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents a list of symbolic constants.
  class enum_data_type final : public data_type {
    public:

      //! The symbols belonging to the enum.
      struct symbol {

        //! The name of the symbol.
        std::string m_name;

        //! The symbol's value.
        int m_value;
      };

      //! Constructs an enum data type.
      /*!
        \param l The location of its definition.
        \param name The name of the data type.
        \param symbols The enum's symbols.
      */
      enum_data_type(location l, std::string name, std::vector<symbol> symbols);

      //! Returns the symbols belonging to this enum.
      const std::vector<symbol>& get_symbols() const;

      const location& get_location() const override;

      const std::string& get_name() const override;

      void apply(data_type_visitor& visitor) const override;

    protected:
      bool is_equal(const data_type& rhs) const override;

    private:
      location m_location;
      std::string m_name;
      std::vector<symbol> m_symbols;
  };

  //! Returns the index of a symbol belonging to an enum.
  /*!
    \param e The enum to search.
    \param symbol The symbol belonging to the enum.
    \return The index of the symbol within <i>e</i>.
  */
  inline int get_index(const enum_data_type& e, const std::string& symbol) {
    for(auto i = 0; i != static_cast<int>(e.get_symbols().size()); ++i) {
      if(e.get_symbols()[i].m_name == symbol) {
        return i;
      }
    }
    return -1;
  }

  inline enum_data_type::enum_data_type(location l, std::string name,
      std::vector<symbol> symbols)
      : m_location(std::move(l)),
        m_name(std::move(name)),
        m_symbols(std::move(symbols)) {}

  inline const std::vector<enum_data_type::symbol>&
      enum_data_type::get_symbols() const {
    return m_symbols;
  }

  inline const location& enum_data_type::get_location() const {
    return m_location;
  }

  inline const std::string& enum_data_type::get_name() const {
    return m_name;
  }

  inline void enum_data_type::apply(data_type_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool enum_data_type::is_equal(const data_type& rhs) const {
    return this == &rhs;
  }

  inline void data_type_visitor::visit(const enum_data_type& node) {
    visit(static_cast<const data_type&>(node));
  }
}

#endif
