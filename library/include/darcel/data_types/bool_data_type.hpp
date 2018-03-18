#ifndef DARCEL_BOOL_DATA_TYPE_HPP
#define DARCEL_BOOL_DATA_TYPE_HPP
#include <memory>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents a boolean data type.
  class bool_data_type : public data_type {
    public:

      //! Returns an instance of this type.
      static const std::shared_ptr<bool_data_type>& get_instance();

      //! Constructs a bool data type.
      bool_data_type() = default;

      const location& get_location() const override final;

      const std::string& get_name() const override final;

      void apply(data_type_visitor& visitor) const override final;

    protected:
      bool is_equal(const data_type& rhs) const override final;
  };

  inline const std::shared_ptr<bool_data_type>& bool_data_type::get_instance() {
    static auto instance = std::make_shared<bool_data_type>();
    return instance;
  }

  inline const location& bool_data_type::get_location() const {
    return location::global();
  }

  inline const std::string& bool_data_type::get_name() const {
    static std::string name = "Bool";
    return name;
  }

  inline void bool_data_type::apply(data_type_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool bool_data_type::is_equal(const data_type& rhs) const {
    return true;
  }

  inline void data_type_visitor::visit(const bool_data_type& node) {
    visit(static_cast<const data_type&>(node));
  }
}

#endif
