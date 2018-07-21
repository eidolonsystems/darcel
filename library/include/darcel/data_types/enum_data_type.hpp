#ifndef DARCEL_ENUM_DATA_TYPE_HPP
#define DARCEL_ENUM_DATA_TYPE_HPP
#include <memory>
#include <vector>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents a list of symbolic constants.
  class EnumDataType final : public DataType {
    public:

      //! The symbols belonging to the enum.
      struct Symbol {

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
      EnumDataType(Location l, std::string name, std::vector<Symbol> symbols);

      //! Returns the symbols belonging to this enum.
      const std::vector<Symbol>& get_symbols() const;

      const Location& get_location() const override;

      const std::string& get_name() const override;

      void apply(DataTypeVisitor& visitor) const override;

    protected:
      bool is_equal(const DataType& rhs) const override;

    private:
      Location m_location;
      std::string m_name;
      std::vector<Symbol> m_symbols;
  };

  //! Returns the index of a symbol belonging to an enum.
  /*!
    \param e The enum to search.
    \param symbol The symbol belonging to the enum.
    \return The index of the symbol within <i>e</i>.
  */
  inline int get_index(const EnumDataType& e, const std::string& symbol) {
    for(auto i = 0; i != static_cast<int>(e.get_symbols().size()); ++i) {
      if(e.get_symbols()[i].m_name == symbol) {
        return i;
      }
    }
    return -1;
  }

  inline EnumDataType::EnumDataType(Location l, std::string name,
      std::vector<Symbol> symbols)
      : m_location(std::move(l)),
        m_name(std::move(name)),
        m_symbols(std::move(symbols)) {}

  inline const std::vector<EnumDataType::Symbol>&
      EnumDataType::get_symbols() const {
    return m_symbols;
  }

  inline const Location& EnumDataType::get_location() const {
    return m_location;
  }

  inline const std::string& EnumDataType::get_name() const {
    return m_name;
  }

  inline void EnumDataType::apply(DataTypeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool EnumDataType::is_equal(const DataType& rhs) const {
    return this == &rhs;
  }

  inline void DataTypeVisitor::visit(const EnumDataType& node) {
    visit(static_cast<const DataType&>(node));
  }
}

#endif
