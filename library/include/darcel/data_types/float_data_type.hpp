#ifndef DARCEL_FLOAT_DATA_TYPE_HPP
#define DARCEL_FLOAT_DATA_TYPE_HPP
#include <memory>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents a floating point data type.
  class FloatDataType final : public DataType {
    public:

      //! Returns an instance of this type.
      static const std::shared_ptr<FloatDataType>& get_instance();

      //! Constructs a float data type.
      FloatDataType() = default;

      const Location& get_location() const override;

      const std::string& get_name() const override;

      void apply(DataTypeVisitor& visitor) const override;

    protected:
      bool is_equal(const DataType& rhs) const override;
  };

  inline const std::shared_ptr<FloatDataType>& FloatDataType::get_instance() {
    static auto instance = std::make_shared<FloatDataType>();
    return instance;
  }

  inline const Location& FloatDataType::get_location() const {
    return Location::global();
  }

  inline const std::string& FloatDataType::get_name() const {
    static std::string name = "Float";
    return name;
  }

  inline void FloatDataType::apply(DataTypeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool FloatDataType::is_equal(const DataType& rhs) const {
    return true;
  }

  inline void DataTypeVisitor::visit(const FloatDataType& node) {
    visit(static_cast<const DataType&>(node));
  }
}

#endif
