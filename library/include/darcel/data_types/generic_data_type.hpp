#ifndef DARCEL_GENERIC_DATA_TYPE_HPP
#define DARCEL_GENERIC_DATA_TYPE_HPP
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents a generic data type.
  class generic_data_type : public data_type {
    public:

      //! Constructs a generic data type.
      /*!
        \param l The location where the generic was declared.
        \param name The name of the generic.
      */
      generic_data_type(location l, std::string name);

      const location& get_location() const override final;

      const std::string& get_name() const override final;

      void apply(data_type_visitor& visitor) const override final;

    protected:
      bool is_equal(const data_type& rhs) const override final;

    private:
      location m_location;
      std::string m_name;
  };

  inline generic_data_type::generic_data_type(location l, std::string name)
      : m_location(std::move(l)),
        m_name(std::move(name)) {}

  inline const location& generic_data_type::get_location() const {
    return m_location;
  }

  inline const std::string& generic_data_type::get_name() const {
    return m_name;
  }

  inline void generic_data_type::apply(data_type_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool generic_data_type::is_equal(const data_type& rhs) const {
    return this == &rhs;
  }

  inline void data_type_visitor::visit(const generic_data_type& node) {
    visit(static_cast<const data_type&>(node));
  }
}

#endif
