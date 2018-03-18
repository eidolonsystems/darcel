#ifndef DARCEL_TEXT_DATA_TYPE_HPP
#define DARCEL_TEXT_DATA_TYPE_HPP
#include <memory>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents a text segment.
  class text_data_type : public data_type {
    public:

      //! Returns an instance of this type.
      static const std::shared_ptr<text_data_type>& get_instance();

      //! Constructs a text data type.
      text_data_type() = default;

      const location& get_location() const override final;

      const std::string& get_name() const override final;

      void apply(data_type_visitor& visitor) const override final;

    protected:
      bool is_equal(const data_type& rhs) const override final;
  };

  inline const std::shared_ptr<text_data_type>&
      text_data_type::get_instance() {
    static auto instance = std::make_shared<text_data_type>();
    return instance;
  }

  inline const location& text_data_type::get_location() const {
    return location::global();
  }

  inline const std::string& text_data_type::get_name() const {
    static std::string name = "Text";
    return name;
  }

  inline void text_data_type::apply(data_type_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool text_data_type::is_equal(const data_type& rhs) const {
    return true;
  }

  inline void data_type_visitor::visit(const text_data_type& node) {
    visit(static_cast<const data_type&>(node));
  }
}

#endif
