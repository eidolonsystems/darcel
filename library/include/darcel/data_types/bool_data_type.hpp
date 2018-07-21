#ifndef DARCEL_BOOL_DATA_TYPE_HPP
#define DARCEL_BOOL_DATA_TYPE_HPP
#include <memory>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents a boolean data type.
  class BoolDataType final : public DataType {
    public:

      //! Returns an instance of this type.
      static const std::shared_ptr<BoolDataType>& get_instance();

      //! Constructs a bool data type.
      BoolDataType() = default;

      const Location& get_location() const override;

      const std::string& get_name() const override;

      void apply(DataTypeVisitor& visitor) const override;

    protected:
      bool is_equal(const DataType& rhs) const override;
  };

  inline const std::shared_ptr<BoolDataType>& BoolDataType::get_instance() {
    static auto instance = std::make_shared<BoolDataType>();
    return instance;
  }

  inline const Location& BoolDataType::get_location() const {
    return Location::global();
  }

  inline const std::string& BoolDataType::get_name() const {
    static std::string name = "Bool";
    return name;
  }

  inline void BoolDataType::apply(DataTypeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool BoolDataType::is_equal(const DataType& rhs) const {
    return true;
  }

  inline void DataTypeVisitor::visit(const BoolDataType& node) {
    visit(static_cast<const DataType&>(node));
  }
}

#endif
