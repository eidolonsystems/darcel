#ifndef DARCEL_FLOAT_DATA_TYPE_HPP
#define DARCEL_FLOAT_DATA_TYPE_HPP
#include <memory>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents a floating point data type.
  class float_data_type : public data_type {
    public:

      //! Returns an instance of this type.
      static const std::shared_ptr<float_data_type>& get_instance();

      //! Constructs a float data type.
      float_data_type() = default;

      const location& get_location() const override final;

      const std::string& get_name() const override final;

      void apply(data_type_visitor& visitor) const override final;

    protected:
      bool is_equal(const data_type& rhs) const override final;
  };

  inline const std::shared_ptr<float_data_type>&
      float_data_type::get_instance() {
    static auto instance = std::make_shared<float_data_type>();
    return instance;
  }

  inline const location& float_data_type::get_location() const {
    return location::global();
  }

  inline const std::string& float_data_type::get_name() const {
    static std::string name = "Float";
    return name;
  }

  inline void float_data_type::apply(data_type_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool float_data_type::is_equal(const data_type& rhs) const {
    return true;
  }

  inline void data_type_visitor::visit(const float_data_type& node) {
    visit(static_cast<const data_type&>(node));
  }
}

#endif
