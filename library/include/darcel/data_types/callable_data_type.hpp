#ifndef DARCEL_CALLABLE_DATA_TYPE_HPP
#define DARCEL_CALLABLE_DATA_TYPE_HPP
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"
#include "darcel/semantics/function.hpp"

namespace darcel {

  //! A data type used to represent all of a function declaration's overloads.
  class callable_data_type final : public data_type {
    public:

      //! Constructs a callable data type.
      /*!
        \param f The function this data type represents.
      */
      callable_data_type(std::shared_ptr<function> f);

      //! Returns the function represented.
      const std::shared_ptr<function> get_function() const;

      const location& get_location() const override;

      const std::string& get_name() const override;

      void apply(data_type_visitor& visitor) const override;

    protected:
      bool is_equal(const data_type& rhs) const override;

    private:
      std::shared_ptr<function> m_function;
  };

  inline callable_data_type::callable_data_type(std::shared_ptr<function> f)
      : m_function(std::move(f)) {}

  inline const std::shared_ptr<function>
      callable_data_type::get_function() const {
    return m_function;
  }

  inline const location& callable_data_type::get_location() const {
    return m_function->get_location();
  }

  inline const std::string& callable_data_type::get_name() const {
    return m_function->get_name();
  }

  inline void callable_data_type::apply(data_type_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool callable_data_type::is_equal(const data_type& rhs) const {
    auto& other = static_cast<const callable_data_type&>(rhs);
    return m_function == other.get_function();
  }

  inline void data_type_visitor::visit(const callable_data_type& node) {
    visit(static_cast<const data_type&>(node));
  }
}

#endif
