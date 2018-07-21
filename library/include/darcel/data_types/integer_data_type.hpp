#ifndef DARCEL_INTEGER_DATA_TYPE_HPP
#define DARCEL_INTEGER_DATA_TYPE_HPP
#include <memory>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! Represents an integer data type.
  class IntegerDataType final : public DataType {
    public:

      //! Returns an instance of this type.
      static const std::shared_ptr<IntegerDataType>& get_instance();

      //! Constructs a integer data type.
      IntegerDataType() = default;

      const location& get_location() const override;

      const std::string& get_name() const override;

      void apply(DataTypeVisitor& visitor) const override;

    protected:
      bool is_equal(const DataType& rhs) const override;
  };

  inline const std::shared_ptr<IntegerDataType>&
      IntegerDataType::get_instance() {
    static auto instance = std::make_shared<IntegerDataType>();
    return instance;
  }

  inline const location& IntegerDataType::get_location() const {
    return location::global();
  }

  inline const std::string& IntegerDataType::get_name() const {
    static std::string name = "Int";
    return name;
  }

  inline void IntegerDataType::apply(DataTypeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool IntegerDataType::is_equal(const DataType& rhs) const {
    return true;
  }

  inline void DataTypeVisitor::visit(const IntegerDataType& node) {
    visit(static_cast<const DataType&>(node));
  }
}

#endif
