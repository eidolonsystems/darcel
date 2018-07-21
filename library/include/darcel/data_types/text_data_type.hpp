#ifndef DARCEL_TEXT_DATA_TYPE_HPP
#define DARCEL_TEXT_DATA_TYPE_HPP
#include <memory>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents a text segment.
  class TextDataType final : public DataType {
    public:

      //! Returns an instance of this type.
      static const std::shared_ptr<TextDataType>& get_instance();

      //! Constructs a text data type.
      TextDataType() = default;

      const location& get_location() const override;

      const std::string& get_name() const override;

      void apply(DataTypeVisitor& visitor) const override;

    protected:
      bool is_equal(const DataType& rhs) const override;
  };

  inline const std::shared_ptr<TextDataType>& TextDataType::get_instance() {
    static auto instance = std::make_shared<TextDataType>();
    return instance;
  }

  inline const location& TextDataType::get_location() const {
    return location::global();
  }

  inline const std::string& TextDataType::get_name() const {
    static std::string name = "Text";
    return name;
  }

  inline void TextDataType::apply(DataTypeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool TextDataType::is_equal(const DataType& rhs) const {
    return true;
  }

  inline void DataTypeVisitor::visit(const TextDataType& node) {
    visit(static_cast<const DataType&>(node));
  }
}

#endif
