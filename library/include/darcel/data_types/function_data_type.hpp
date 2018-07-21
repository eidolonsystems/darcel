#ifndef DARCEL_FUNCTION_DATA_TYPE_HPP
#define DARCEL_FUNCTION_DATA_TYPE_HPP
#include <memory>
#include <utility>
#include <vector>
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"

namespace darcel {

  //! The data type used to represent functions.
  class FunctionDataType final : public DataType {
    public:

      //! Stores a single parameter.
      struct Parameter {

        //! The name of the parameter.
        std::string m_name;

        //! The parameter's type.
        std::shared_ptr<DataType> m_type;

        //! Constructs a parameter.
        Parameter(std::string name, std::shared_ptr<DataType> type);
      };

      //! Constructs a function data type.
      /*!
        \param parameters The function's parameters.
        \param return_type The return type.
      */
      FunctionDataType(std::vector<Parameter> parameters,
        std::shared_ptr<DataType> return_type);

      //! Returns the list of parameters.
      const std::vector<Parameter>& get_parameters() const;

      //! Returns the return type.
      const std::shared_ptr<DataType>& get_return_type() const;

      const Location& get_location() const override;

      const std::string& get_name() const override;

      void apply(DataTypeVisitor& visitor) const override;

    protected:
      bool is_equal(const DataType& rhs) const override;

    private:
      std::vector<Parameter> m_parameters;
      std::shared_ptr<DataType> m_return_type;
      std::string m_name;
  };

  //! Makes a function data type.
  /*!
    \param parameters The function's parameters.
    \param return_type The return type.
  */
  inline std::shared_ptr<FunctionDataType> make_function_data_type(
      std::initializer_list<FunctionDataType::Parameter> parameters,
      std::shared_ptr<DataType> return_type) {
    return std::make_shared<FunctionDataType>(
      std::vector<FunctionDataType::Parameter>(std::move(parameters)),
      std::move(return_type));
  }

  //! Tests if two function data types have the same signature.
  inline bool equal_signature(const FunctionDataType& lhs,
      const FunctionDataType& rhs) {
    if(lhs.get_parameters().size() != rhs.get_parameters().size()) {
      return false;
    }
    for(std::size_t i = 0; i != lhs.get_parameters().size(); ++i) {
      auto& p1 = *lhs.get_parameters()[i].m_type;
      auto& p2 = *rhs.get_parameters()[i].m_type;
      if(p1 != p2) {
        return false;
      }
    }
    return true;
  }

  inline FunctionDataType::Parameter::Parameter(std::string name,
      std::shared_ptr<DataType> type)
      : m_name(std::move(name)),
        m_type(std::move(type)) {}

  inline FunctionDataType::FunctionDataType(
      std::vector<Parameter> parameters, std::shared_ptr<DataType> return_type)
      : m_parameters(std::move(parameters)),
        m_return_type(std::move(return_type)) {
    m_name = "(";
    auto is_first = true;
    for(auto& parameter : m_parameters) {
      if(!is_first) {
        m_name += ", ";
      } else {
        is_first = false;
      }
      if(!parameter.m_name.empty()) {
        m_name += parameter.m_name + ": ";
      }
      m_name += parameter.m_type->get_name();
    }
    m_name += ") -> " + m_return_type->get_name();
  }

  inline const std::vector<FunctionDataType::Parameter>&
      FunctionDataType::get_parameters() const {
    return m_parameters;
  }

  inline const std::shared_ptr<DataType>&
      FunctionDataType::get_return_type() const {
    return m_return_type;
  }

  inline const Location& FunctionDataType::get_location() const {
    return Location::global();
  }

  inline const std::string& FunctionDataType::get_name() const {
    return m_name;
  }

  inline void FunctionDataType::apply(DataTypeVisitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool FunctionDataType::is_equal(const DataType& rhs) const {
    auto& f = static_cast<const FunctionDataType&>(rhs);
    if(*m_return_type != *f.get_return_type() ||
        m_parameters.size() != f.get_parameters().size()) {
      return false;
    }
    for(std::size_t i = 0; i < m_parameters.size(); ++i) {
      if(*m_parameters[i].m_type != *f.get_parameters()[i].m_type) {
        return false;
      }
    }
    return true;
  }

  inline void DataTypeVisitor::visit(const FunctionDataType& node) {
    visit(static_cast<const DataType&>(node));
  }
}

#endif
